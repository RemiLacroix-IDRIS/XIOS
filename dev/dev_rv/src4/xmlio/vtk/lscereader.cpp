
#ifndef LSCE_EXPORTS
#       include "lscereader.hpp"
#else
#       include "vtkLSCEReader.h"
#endif //LSCE_EXPORTS


// Bibliothèque VTK
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkPointLocator.h"

#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendScaleActor.h>

#include "vtkMath.h"

#include "inetcdf4_impl.hpp"
#include "inetcdf4_adv_impl.hpp"

// Bibliothèque C++
#include <algorithm>

// Type Fortran
#define ARRAY(valuetype, numdims) boost::shared_ptr<boost::multi_array<valuetype, numdims> >

#define ARRAY_ASSIGN(value, valuetype, numdims, extent)\
   value.reset(new boost::multi_array<valuetype, numdims>(boost::extents extent)) 

#define ARRAY_CREATE(value, valuetype, numdims, extent)\
   ARRAY(valuetype, numdims) value =                   \
   ARRAY(valuetype, numdims)(new boost::multi_array<valuetype, numdims>(boost::extents extent))

// Type C
#define ARRAY_C_ASSIGN(value, valuetype, numdims, extent)\
   value = ARRAY(valuetype, numdims)                     \
   (new boost::multi_array<valuetype, numdims>(boost::extents extent, c_storage_order()))

#define ARRAY_C_CREATE(value, valuetype, numdims, extent)\
   ARRAY_C_ASSIGN(ARRAY(valuetype, numdims) value, valuetype, numdims, extent)


#ifndef LSCE_EXPORTS
namespace xmlioserver
{
   namespace vtk
   {
#endif //LSCE_EXPORTS
       
      /// ////////////////////// Définitions ////////////////////// ///
      vtkStandardNewMacro(vtkLSCEReader);

      vtkLSCEReader::vtkLSCEReader(void)
         : FileName(), CurGridType(RECTILINEAR), VarNames()
         , A3D(true), ATemporal(true), ACell(true), AGridDef(false)
         , Reader()
      {
         this->SetNumberOfInputPorts (0);
         this->SetNumberOfOutputPorts(1);
      }

      vtkLSCEReader::~vtkLSCEReader(void)
      {

      }

      ///---------------------------------------------------------------

      void vtkLSCEReader::SetFileName(const vtkStdString & fileName)
      {
         this->FileName = fileName;
         this->Reader   = boost::shared_ptr<xmlioserver::io::CINetCDF4Adv>
                                       (new xmlioserver::io::CINetCDF4Adv(fileName));
         this->Modified();
      }
      
      void vtkLSCEReader::SetVariable(const vtkStdString & variable)
      {
         this->RemoveAllSelectedVariables();
         if (Reader->hasVariable(variable))
            this->AddVariableToSelection(variable);
         this->Modified();
      }

      const char * vtkLSCEReader::GetFileName(void) const
      {
         return (this->FileName.c_str());
      }

      void vtkLSCEReader::PrintSelf(ostream& os, vtkIndent indent)
      {
        this->Superclass::PrintSelf(os,indent);
      }

      //----------------------------------------------------------------
      void vtkLSCEReader::AddVariableToSelection(const vtkStdString & varName)
      {
         if (this->Reader->isRectilinear(varName))
            this->SetGridType (RECTILINEAR);
         else if (this->Reader->isCurvilinear(varName))
            this->SetGridType (CURVILINEAR);
         else
            this->SetGridType (UNSTRUCTURED);

         this->VarNames.insert(varName);
      }

      void vtkLSCEReader::RemoveSelectedVariable(const vtkStdString & varName)
      {
         std::set<vtkStdString>::iterator it = this->VarNames.find(varName);
         if (it != this->VarNames.end()) this->VarNames.erase(it);
      }

      void vtkLSCEReader::RemoveAllSelectedVariables(void)
      {
         this->VarNames.clear();
      }

      const std::set<vtkStdString> &
            vtkLSCEReader::GetSelectedVariables(void) const
      {
         return (this->VarNames);
      }

      bool vtkLSCEReader::HasSelectedVariable(void) const
      {
         return (this->VarNames.size() != 0);
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::SetGridType (GridType type)
      {
         if ((this->IsUnstructured() && (type == CURVILINEAR)) ||
             (this->IsUnstructured() && (type == RECTILINEAR)) ||
             (this->IsCurvilinear()  && (type == RECTILINEAR)))
            this->RemoveAllSelectedVariables();
         this->CurGridType = type;
      }

      bool vtkLSCEReader::IsUnstructured(void) const
      {
         return (this->CurGridType == UNSTRUCTURED);
      }

      bool vtkLSCEReader::IsCurvilinear(void) const
      {
         return (this->CurGridType == CURVILINEAR);
      }

      bool vtkLSCEReader::IsRectilinear(void) const
      {
         return (this->CurGridType == RECTILINEAR);
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::AcceptTemporalOnly(bool value)
      {
         this->ATemporal = value;
      }

      void vtkLSCEReader::Accept3DOnly(bool value)
      {
         this->A3D = value;
      }

      void vtkLSCEReader::AcceptCellOnly(bool value)
      {
         this->ACell = value;
      }

      //----------------------------------------------------------------

      int vtkLSCEReader::ProcessRequest
                           (vtkInformation       *  request,
                            vtkInformationVector ** vtkNotUsed(inputVector),
                            vtkInformationVector *  outputVector)
      {
         try
         {
          
            if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
            {
               return (this->RequestData(NULL, NULL, outputVector));
            }

            if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
            {
            //   return (this->RequestDataObject(NULL, NULL, outputVector));
            }

            if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
            {
                 return (this->RequestInformation(NULL, NULL, outputVector));
            }

            if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
            {
               return (this->RequestUpdateExtent(NULL, NULL, outputVector));
            }
         }
         catch (xmlioserver::CException & _exception)
         {
            std::cerr << _exception.getMessage() << std::endl;
            return (-1);
         }
         return (1);
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::ShowVariable(const vtkStdString & filename,
                                       const vtkStdString & varname)
      {
         vtkSmartPointer<vtkLSCEReader> lscereader =
               vtkSmartPointer<vtkLSCEReader>::New();

         lscereader->SetFileName(filename);
         lscereader->AddVariableToSelection(varname);
         lscereader->Update();

         // Création d'un mappeur et d'un acteur.
         vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
         mapper->SetInputConnection(lscereader->GetOutput()->GetProducerPort());
         mapper->SetScalarRange  (1000, 10000);

         vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
         actor->SetMapper(mapper);
         actor->GetProperty()->SetRepresentationToWireframe();

         // Création d'une fenêtre de rendu.
         vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
         vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
         renderWindow->AddRenderer(renderer);
         vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
         renderWindowInteractor->SetRenderWindow(renderWindow);


         vtkSmartPointer<vtkLegendScaleActor> legendScaleActor =
            vtkSmartPointer<vtkLegendScaleActor>::New();

         // Ajout de l'acteur à la scène.
         renderer->AddActor(actor);
         renderer->AddActor(legendScaleActor);

         renderer->GradientBackgroundOn();
         renderer->SetBackground(1,1,1);
         renderer->SetBackground2(0,0,0);

         // Rendu et intéraction
         renderWindow->Render();
         renderWindowInteractor->Start();
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::CreateRectilinearGrid(vtkRectilinearGrid * grid,
                                                vtkInformation     * outInfo,
                                                vtkFloatArray      * xspacing,
                                                vtkFloatArray      * yspacing,
                                                vtkFloatArray      * zspacing,
                                                vtkIntArray        * dimensions)
      {
            int extent[6] = { 0, dimensions->GetValue(0) - 1
                            , 0, dimensions->GetValue(1) - 1
                            , 0, dimensions->GetValue(2) - 1};
            outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);
            grid->SetExtent (extent);
            grid->SetDimensions (dimensions->GetValue(0),
                                 dimensions->GetValue(1),
                                 dimensions->GetValue(2));
            grid->SetXCoordinates (xspacing);
            grid->SetYCoordinates (yspacing);
            grid->SetZCoordinates (zspacing);
      }

      void vtkLSCEReader::CreateStructuredGrid(vtkStructuredGrid * grid,
                                               vtkInformation    * outInfo,
                                               vtkPoints         * points,
                                               vtkIntArray       * dimensions)
      {
            int extent[6] = { 0, dimensions->GetValue(0) - 1
                            , 0, dimensions->GetValue(1) - 1
                            , 0, dimensions->GetValue(2) - 1};
            outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);
            grid->SetExtent (extent);
            grid->SetDimensions (dimensions->GetValue(0),
                                 dimensions->GetValue(1),
                                 dimensions->GetValue(2));
            grid->SetPoints(points);
      }

      void vtkLSCEReader::CreateUnstructuredGrid(vtkUnstructuredGrid * grid,
                                                 vtkInformation      * outInfo,
                                                 vtkPoints           * points,
                                                 vtkCellArray        * cells,
                                                 int                   celltype)
      {
         grid->SetPoints(points);
         grid->SetCells(celltype, cells);
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::GetRectilinearConnectivity(int dimZ, int dimY, int dimX, vtkCellArray * cells)
      {
         if (dimZ == 1 && dimY >  1 && dimX >  1) // 2D
         {
            vtkIdType pts[4] ;

            for (int i = 0, k = 0; i < (dimX - 1); i++)
               for (int j = 0; j < (dimY - 1); j++, k++)
            {
               pts[0] =  i * dimY + j;
               pts[1] =  i * dimY + j + 1;
               pts[2] =  (i+1) * dimY + j + 1;
               pts[3] =  (i+1) * dimY + j;
               cells->InsertNextCell(4,  pts);
            }
         }
         else if (dimZ > 1 && dimY >  1 && dimX >  1) // 3D
         {
            vtkIdType pts[8];
            for (int i = 0, l = 0; i < (dimX - 1); i++)
               for (int j = 0; j < (dimY - 1); j++)
                  for (int k = 0; k < (dimZ - 1); k++, l++)
            {
               pts[0] =  i * dimY * dimZ + j * dimZ + k;
               pts[1] =  i * dimY * dimZ + j * dimZ + k + 1;
               pts[2] =  (i+1) * dimY * dimZ + j * dimZ + k + 1;
               pts[3] =  (i+1) * dimY * dimZ + j * dimZ + k;

               pts[4] =  i * dimY * dimZ  + (j + 1) * dimZ + k ;
               pts[5] =  i * dimY * dimZ  + (j + 1) * dimZ + k +1;
               pts[6] =  (i+1) * dimY * dimZ + (j + 1) * dimZ + k + 1;
               pts[7] =  (i+1) * dimY * dimZ + (j + 1) * dimZ + k ;
               cells->InsertNextCell(8,  pts);
            }
         }
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::CreateSimpleGrid (int xi, int xf,
                                            int yi, int yf,
                                            int zi, int zf,
                                            vtkFloatArray * xspacing,
                                            vtkFloatArray * yspacing,
                                            vtkFloatArray * zspacing,
                                            vtkIntArray   * dimensions)
      {
         dimensions->InsertNextValue (xf - xi + 1); // X
         dimensions->InsertNextValue (yf - yi + 1); // Y
         dimensions->InsertNextValue (zf - zi + 1); // Z
         for (int i = xi; i <= xf; i++)
            xspacing->InsertNextValue (i);
         for (int j = yi; j <= yf; j++)
            yspacing->InsertNextValue (j);
         for (int k = zi; k <= zf; k++)
            zspacing->InsertNextValue (k);
      }

      void vtkLSCEReader::CreateSimpleGrid (int xi, int xf,
                                            int yi, int yf,
                                            int zi, int zf,
                                            vtkPoints   * points,
                                            vtkIntArray * dimensions)
      {
         dimensions->InsertNextValue (xf - xi + 1); // X
         dimensions->InsertNextValue (yf - yi + 1); // Y
         dimensions->InsertNextValue (zf - zi + 1); // Z
         for (int i = xi; i <= xf; i++)
            for (int j = yi; j <= yf; j++)
               for (int k = zi; k <= zf; k++)
                  points->InsertNextPoint(i, j, k);
      }

      void vtkLSCEReader::CreateSimpleGrid (int xi, int xf,
                                            int yi, int yf,
                                            int zi, int zf,
                                            vtkPoints    * points,
                                            vtkCellArray * cells,
                                            vtkIntArray  * dimensions)
      {
         this->CreateSimpleGrid (xi, xf, yi, yf, zi, zf, points, dimensions);
         int dimX = dimensions->GetValue(0),
             dimY = dimensions->GetValue(1),
             dimZ = dimensions->GetValue(2);
         this->GetRectilinearConnectivity(dimZ, dimY, dimX, cells);
      }

      //----------------------------------------------------------------
      void vtkLSCEReader::GetSpacings(const vtkStdString & coordinate,
                                      bool bounds, vtkFloatArray * spacing)
      {
         std::size_t size = (Reader->getDimensions(&coordinate).begin())->second;
         ARRAY_CREATE(data_arr, float, 1, [1]);
         if (Reader->hasVariable(coordinate))
         {
            if (bounds)
            {
               if (Reader->hasBounds(coordinate))
               { // CellData, variable de coordonnée avec attributes bounds.
                  vtkStdString boundid = Reader->getBoundsId(coordinate);
                  float f;
                  Reader->readData(*data_arr, boundid);
                  spacing->InsertNextValue (f = (*data_arr)[0]);
                  for (std::size_t j = 1; j < (2 * size); j++) // << Super laid
                     if (f != (*data_arr)[j])
                        spacing->InsertNextValue (f = (*data_arr)[j]);
               }
               else
               { // CellData, variable de coordonnée sans attributes bounds.
                  Reader->readData(*data_arr, coordinate);
                  spacing->InsertNextValue ((*data_arr)[0] - ((*data_arr)[1] - (*data_arr)[0])/2);
                  for (std::size_t j = 0; j < (size-1); j++)
                     spacing->InsertNextValue ((*data_arr)[j] + ((*data_arr)[j+1]-(*data_arr)[j])/2);
                  spacing->InsertNextValue ((*data_arr)[size-2] + ((*data_arr)[size-1] - (*data_arr)[size-2])/2);
               }
            }
            else
            { // PointData, variable de coordonnée.
               Reader->readData(*data_arr, coordinate);
               for (std::size_t j = 0; j < (size-1); j++)
                  spacing->InsertNextValue ((*data_arr)[j]);
            }
         }
         else
         {
            if (bounds) // CellData, dimension de coordonnée.
               for (float i = -0.5; i < ((float)size + 1.); i = i + 1.)
                  spacing->InsertNextValue (i);
            else // PointData, dimension de coordonnée.
               for (float j = 0.; j < (float)size; j = j + 1.)
                  spacing->InsertNextValue (j);
         }
      }
      //----------------------------------------------------------------

      void vtkLSCEReader::AddPoint(vtkPoints * points, float *  value, bool proj)
      {
         if (proj)
         {
            double h = 1.0;
            double cartesianCoord[3];
            value[0] =  0.017453292519943295 * value[0];
            value[1] =  0.017453292519943295 * value[1];

            cartesianCoord[0] = h * cos(value[0]) * cos(value[1]);
            cartesianCoord[1] = h * sin(value[0]) * cos(value[1]);
            cartesianCoord[2] = h * sin(value[1]);
            points->InsertNextPoint (cartesianCoord);
         }
         else
         {
            points->InsertNextPoint (value);
         }
      }

      //----------------------------------------------------------------
      
      void vtkLSCEReader::GetDimensions(const vtkStdString & xcoordinate,
                                        const vtkStdString & ycoordinate,
                                        const vtkStdString & zcoordinate,
                                        vtkIntArray * dimensions, bool bounds)
      {
         int a = (bounds) ? 1 : 0;
         dimensions->InsertNextValue
            ((Reader->getDimensions(&xcoordinate).begin())->second + a);
         dimensions->InsertNextValue
            ((Reader->getDimensions(&ycoordinate).begin())->second + a);
         if (zcoordinate.size() > 0)
         {
            dimensions->InsertNextValue
               ((Reader->getDimensions(&zcoordinate).begin())->second + a);
         }
         else
         {
            dimensions->InsertNextValue(1);
         }
      }

      //----------------------------------------------------------------
      
      void vtkLSCEReader::GetDimensions(const vtkStdString & xcoordinate,
                                        const vtkStdString & zcoordinate,
                                        vtkIntArray * dimensions, bool bounds)
      {
         int a = (bounds) ? 1 : 0;
         dimensions->InsertNextValue
            (Reader->getDimensions(&xcoordinate)[Reader->getLonCoordName(xcoordinate)] + a);
         dimensions->InsertNextValue
            (Reader->getDimensions(&xcoordinate)[Reader->getLatCoordName(xcoordinate)] + a);
         if (zcoordinate.size() > 0)
         {
            dimensions->InsertNextValue
               ((Reader->getDimensions(&zcoordinate).begin())->second + a);
         }
         else
         {
            dimensions->InsertNextValue(1);
         }
      }

      //----------------------------------------------------------------
      void vtkLSCEReader::GetPoints(const vtkStdString & xcoordinate,
                                    const vtkStdString & ycoordinate,
                                    const vtkStdString & zcoordinate,
                                    bool bounds, bool proj,
                                    vtkPoints * points, vtkIntArray * dimensions)
      {
         float value[3];
         bool is2D = true;
         ARRAY_CREATE(xvalue, float, 1, [1]);
         ARRAY_CREATE(yvalue, float, 1, [1]);
         vtkSmartPointer<vtkFloatArray> zspacing = vtkSmartPointer<vtkFloatArray>::New();
         if (zcoordinate.size() > 0)
         {
            GetSpacings(zcoordinate, bounds, zspacing);
            is2D = false;
         }
         else
            zspacing->InsertNextValue(0);

         dimensions->InsertNextValue
            (Reader->getDimensions(&xcoordinate)[Reader->getLonCoordName(xcoordinate)]);
         dimensions->InsertNextValue
            (Reader->getDimensions(&xcoordinate)[Reader->getLatCoordName(xcoordinate)]);
         dimensions->InsertNextValue(zspacing->GetNumberOfTuples());

         if (!bounds)
         {
            Reader->readData(*xvalue, xcoordinate);
            Reader->readData(*yvalue, ycoordinate);

            for (int i = 0; i < dimensions->GetValue(0); i++)
               for (int j = 0; j < dimensions->GetValue(1); j++)
                  for (int k = 0; k < dimensions->GetValue(2); k++)
                  {
                     value[0] = (*xvalue)[i*dimensions->GetValue(1) + j];
                     value[1] = (*yvalue)[i*dimensions->GetValue(1) + j];
                     value[2] = zspacing->GetValue(k);
                     AddPoint(points, value, proj);
                  }
            return;
         }

         dimensions->SetValue(0, dimensions->GetValue(0) + 1);
         dimensions->SetValue(1, dimensions->GetValue(1) + 1);

         if (Reader->hasBounds(xcoordinate))
         {
            vtkStdString boundid = Reader->getBoundsId(xcoordinate);
            Reader->readData(*xvalue, boundid);
         }
         if (Reader->hasBounds(ycoordinate))
         {
            vtkStdString boundid = Reader->getBoundsId(ycoordinate);
            Reader->readData(*yvalue, boundid);
         }
         int ydim = dimensions->GetValue(0) - 1 ,
             xdim = dimensions->GetValue(1) - 1;

         for (int i = 0; i <= xdim; i++)
            for (int j = 0; j <= ydim; j++)
               for (int k = 0; k < dimensions->GetValue(2); k++)
               {
                  if ((j != ydim) && (i != xdim))
                  {
                     value[0] = (*xvalue)[4 * (i * ydim + j) + 0];
                     value[1] = (*yvalue)[4 * (i * ydim + j) + 0];
                     value[2] = zspacing->GetValue(k);
                     AddPoint(points, value, proj);

                     continue;
                  }
                  else if ((j == ydim) && (i != xdim))
                  {
                     value[0] = (*xvalue)[4 * (i * ydim + j - 1) + 1];
                     value[1] = (*yvalue)[4 * (i * ydim + j - 1) + 1];
                     value[2] = zspacing->GetValue(k);
                     AddPoint(points, value, proj);

                     continue;
                  }
                  else if ((j != ydim) && (i == xdim))
                  {
                     value[0] = (*xvalue)[4 * ((i-1) * ydim + j) + 3];
                     value[1] = (*yvalue)[4 * ((i-1) * ydim + j) + 3];
                     value[2] = zspacing->GetValue(k);
                     AddPoint(points, value, proj);

                     continue;
                  }
                  else if ((j == ydim) && (i == xdim))
                  {
                     value[0] = (*xvalue)[4 * ((i-1) * ydim + j - 1) + 2];
                     value[1] = (*yvalue)[4 * ((i-1) * ydim + j - 1) + 2];
                     value[2] = zspacing->GetValue(k);
                     AddPoint(points, value, proj);

                     continue;
                  }
               }
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::GetCellsAndPoints(const vtkStdString & xcoordinate,
                                            const vtkStdString & ycoordinate,
                                            const vtkStdString & zcoordinate,
                                            bool bounds, bool proj, bool clean, std::size_t nbvertex,
                                            vtkCellArray * cells, vtkPoints * points,
                                            vtkIntArray * dimensions)
      {
         float  value[3];
         double h = 1.0;
         double cartesianCoord[3], vbounds[6], range[2];
         vtkIdType pts[nbvertex] ;
         bool is2D = true;
         ARRAY_CREATE(xvalue, float, 1, [1]);
         ARRAY_CREATE(yvalue, float, 1, [1]);
         vtkSmartPointer<vtkPointLocator> locator = vtkSmartPointer<vtkPointLocator>::New();
         vtkSmartPointer<vtkFloatArray> zspacing  = vtkSmartPointer<vtkFloatArray>::New();
         
         if (zcoordinate.size() > 0)
         {
            GetSpacings(zcoordinate, bounds, zspacing);
            is2D = false;
         }
         else
            zspacing->InsertNextValue(0);
         
         zspacing->GetRange (range);

         std::size_t size = (*Reader->getDimensions(&xcoordinate).begin()).second;
         if (Reader->hasBounds(xcoordinate))
         {
            vtkStdString boundid = Reader->getBoundsId(xcoordinate);
            Reader->readData(*xvalue, boundid);
            if (!proj)
            {
               vbounds[0] = *std::min_element(xvalue->begin(), xvalue->end());
               vbounds[1] = *std::max_element(xvalue->begin(), xvalue->end());
            }
            else
            {
               vbounds[0] = -h;
               vbounds[1] = h;
            }
         }
         if (Reader->hasBounds(ycoordinate))
         {
            vtkStdString boundid = Reader->getBoundsId(ycoordinate);
            Reader->readData(*yvalue, boundid);
            if (!proj)
            {
               vbounds[2] = *std::min_element(yvalue->begin(), yvalue->end());
               vbounds[3] = *std::max_element(yvalue->begin(), yvalue->end());
            }
            else
            {
               vbounds[2] = -h;
               vbounds[3] = h;
            }
         }
         if (!proj)
         {
            vbounds[4] = range[0];
            vbounds[5] = range[1];
         }
         else
         {
            vbounds[4] = -h;
            vbounds[5] = h;
         }
         locator->InitPointInsertion(points, vbounds, size * zspacing->GetNumberOfTuples() * nbvertex); 
         
         for (std::size_t u = 0; u < size; u++)
         {
            for (int v = 0; v < zspacing->GetNumberOfTuples(); v++)
            {
               for (std::size_t w = 0; w < nbvertex; w++)
               {
                  value[0] = (*xvalue)[nbvertex * u + w];
                  value[1] = (*yvalue)[nbvertex * u + w];
                  value[2] = zspacing->GetValue(v);
                  pts[w] = nbvertex * u + w;

                  if (proj)
                  {
                     value[0] = 0.017453292519943295 * value[0];
                     value[1] = 0.017453292519943295 * value[1];

                     cartesianCoord[0] = h * cos(value[0]) * cos(value[1]);
                     cartesianCoord[1] = h * sin(value[0]) * cos(value[1]);
                     cartesianCoord[2] = h * sin(value[1]);
                     if (clean) locator->InsertUniquePoint (cartesianCoord, pts[w]);
                     else points->InsertPoint (pts[w], cartesianCoord);
                  }
                  else
                  {
                     cartesianCoord[0] = value[0];
                     cartesianCoord[1] = value[1];
                     cartesianCoord[2] = value[2];
                     if (clean) locator->InsertUniquePoint (cartesianCoord, pts[w]);
                     else points->InsertPoint (pts[w], cartesianCoord);
                  }
               }
               cells->InsertNextCell(nbvertex,  pts);
            }
         }
      }

      //----------------------------------------------------------------

      void vtkLSCEReader::AddScalarData(vtkDataSet * output,
                                        const vtkStdString & varname,
                                        std::size_t record, bool bounds)
      {
         vtkSmartPointer<vtkDoubleArray> data = vtkDoubleArray::New();
         data->SetName(varname.c_str());
         ARRAY_CREATE(data_arr, double, 1, [1]);
         Reader->readData(*data_arr, varname, record);
         Reader->replaceMissingValue(varname, *data_arr, vtkMath::Nan());
         
         
         data->SetNumberOfValues (data_arr->size());
         for (std::size_t i = 0; i < data_arr->size(); i++)
            data->SetValue (i, (*data_arr)[i]);

         if (bounds)
         {
            output->GetCellData()->AddArray(data);
            output->GetCellData()->SetActiveScalars (varname.c_str());
         }
         else
         {
            output->GetPointData()->AddArray(data);
            output->GetPointData()->SetActiveScalars (varname.c_str());
         }
      }

      //----------------------------------------------------------------
      
      int vtkLSCEReader::RequestData
                           (vtkInformation       *  vtkNotUsed(request),
                            vtkInformationVector ** vtkNotUsed(inputVector),
                            vtkInformationVector *  outputVector)
      {
         vtkInformation *  outInfo = outputVector->GetInformationObject(0);
         vtkDataSet     *  output  = vtkDataSet::GetData(outInfo);
         
         if (!output)
         {
            switch (this->CurGridType)
            {
               case(RECTILINEAR)  :
                  output = vtkRectilinearGrid::New();
                  break;
               case(CURVILINEAR)  :
                  output = vtkStructuredGrid::New();
                  break;
               default :
                  output = vtkUnstructuredGrid::New();
                  break;
            }
            output->SetPipelineInformation(outInfo);
            output->Delete();
         }

         vtkUnstructuredGrid * uoutput  = vtkUnstructuredGrid::SafeDownCast(output);
         vtkRectilinearGrid  * routput  = vtkRectilinearGrid::SafeDownCast(output);
         vtkStructuredGrid   * soutput  = vtkStructuredGrid::SafeDownCast(output);
         
        double time = 0.0;
        if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS()))
           time = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS())[0];
        
         if (routput && !this->VarNames.empty())
         { // Grille rectiliniéaire
            vtkSmartPointer<vtkFloatArray>
                           xspacing = vtkSmartPointer<vtkFloatArray>::New(),
                           yspacing = vtkSmartPointer<vtkFloatArray>::New(),
                           zspacing = vtkSmartPointer<vtkFloatArray>::New();
            vtkSmartPointer<vtkIntArray> dims = vtkSmartPointer<vtkIntArray>::New();

            //CreateSimpleGrid (-1, 10, -1, 10, -1, 10, xspacing, yspacing, zspacing, dims);
            GetSpacings(Reader->getLonCoordName(*VarNames.begin()), true, xspacing);
            GetSpacings(Reader->getLatCoordName(*VarNames.begin()), true, yspacing);
            GetSpacings(Reader->getVertCoordName(*VarNames.begin()), true, zspacing);
            /*std::cout << zspacing->GetNumberOfTuples() << std::endl;*/

            dims->InsertNextValue(xspacing->GetNumberOfTuples());
            dims->InsertNextValue(yspacing->GetNumberOfTuples());
            dims->InsertNextValue(zspacing->GetNumberOfTuples());
            
            CreateRectilinearGrid(routput, outInfo, xspacing, yspacing, zspacing, dims);
            AddScalarData(routput, *VarNames.begin(), 0, true);

            this->AGridDef = true;
         }

         if (soutput && !this->VarNames.empty())
         { // Grille structurée
             
            vtkSmartPointer<vtkPoints>   pts  = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkIntArray> dims = vtkSmartPointer<vtkIntArray>::New();
            
            //CreateSimpleGrid (-1, 10, -1, 10, -1, 10, pts, dims);
            GetPoints(Reader->getLonCoordName(*VarNames.begin()),
                      Reader->getLatCoordName(*VarNames.begin()),
                      Reader->getVertCoordName(*VarNames.begin()), true, true, pts, dims);

            CreateStructuredGrid(soutput, outInfo, pts, dims);
            AddScalarData(soutput, *VarNames.begin(), 0, true);

            this->AGridDef = true;
         }

         if (uoutput  && !this->VarNames.empty())
         { // Grille non-structurée
            vtkSmartPointer<vtkPoints>    pts   = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkIntArray>  dims  = vtkSmartPointer<vtkIntArray>::New();
            vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
            // VTK_QUAD, VTK_HEXAHEDRON, VTK_POLYGON
            //CreateSimpleGrid (0, 12, 0, 12, 0, 12, pts, cells, dims);
            //std::cout << Reader->getVertCoordName(*VarNames.begin()) << std::endl;

            GetCellsAndPoints(Reader->getLonCoordName(*VarNames.begin()),
                              Reader->getLatCoordName(*VarNames.begin()),
                              Reader->getVertCoordName(*VarNames.begin()), true, true, true,
                              Reader->getNbVertex(*VarNames.begin()), cells, pts, dims);

            CreateUnstructuredGrid(uoutput, outInfo, pts, cells, VTK_POLYGON);
         
            AddScalarData(uoutput, *VarNames.begin(), this->GetRecord(*VarNames.begin(), time), true);
            
            this->AGridDef = true;
         }
         return (1);
      }

      //----------------------------------------------------------------
      
      void vtkLSCEReader::GetTimeInformation
               (const vtkStdString & variable, vtkDoubleArray * values, double * timeRange)
      {
         std::string timename = this->Reader->getTimeCoordName(variable);
         if (this->Reader->hasVariable(timename))
         {
            ARRAY_CREATE(data_arr, double, 1, [1]);
            Reader->readData(*data_arr, timename);
            
            values->SetNumberOfValues (data_arr->size());
            for (std::size_t i = 0; i < data_arr->size(); i++)
               values->SetValue (i, (*data_arr)[i]);
         }
         else
         {
            std::size_t nbtimestep = this->Reader->getNbOfTimestep();
            values->SetNumberOfValues (nbtimestep);
            for (std::size_t i = 0; i < nbtimestep; i++)
               values->SetValue (i, i);
         }
         timeRange[0] = values->GetValue(0);
         timeRange[1] = values->GetValue(values->GetNumberOfTuples()-1);
      }
      
      std::size_t vtkLSCEReader::GetRecord(const vtkStdString & variable, const double & value)
      {
         std::string timename = this->Reader->getTimeCoordName(variable);
         if (this->Reader->hasVariable(timename))
         {
            ARRAY_CREATE(data_arr, double, 1, [1]);
            Reader->readData(*data_arr, timename);
            return (std::find(data_arr->begin(), data_arr->end(), value) - data_arr->begin());
         }
         else
         {
            return (value);
         }
      }

      //----------------------------------------------------------------
      
      int vtkLSCEReader::RequestInformation
                           (vtkInformation       *  vtkNotUsed(request),
                            vtkInformationVector ** vtkNotUsed(inputVector),
                            vtkInformationVector *  outputVector)
      {
         vtkInformation *  outInfo = outputVector->GetInformationObject(0);
         vtkDataSet     *  output  = vtkDataSet::GetData(outInfo);

         this->AGridDef = false;
         
         if (!this->VarNames.empty())
         {
            switch (this->CurGridType)
            {
               case(RECTILINEAR)  :
                  output = vtkRectilinearGrid::New();
                  break;
               case(CURVILINEAR)  :
                  output = vtkStructuredGrid::New();
                  break;
               default :
                  output = vtkUnstructuredGrid::New();
                  break;
            }
            output->SetPipelineInformation(outInfo);
            output->Delete();
         }

         vtkRectilinearGrid  * routput  = vtkRectilinearGrid::SafeDownCast(output);
         vtkStructuredGrid   * soutput  = vtkStructuredGrid::SafeDownCast(output);
         
         if (!this->VarNames.empty())
         { // Informations temporelles
            if (this->Reader->hasTemporalDim() && this->Reader->isTemporal(*VarNames.begin()))
            {
               vtkSmartPointer<vtkDoubleArray> timeStepValues = vtkSmartPointer<vtkDoubleArray>::New();
               double timeRange[2];
               this->GetTimeInformation(*VarNames.begin(), timeStepValues, timeRange);

               outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),
                            timeStepValues->GetPointer(0),
                            timeStepValues->GetNumberOfTuples());
               outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
            }
            else
            {
               outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
               outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
            }
            
         }
         
         if (routput && !this->VarNames.empty())
         { // Grille rectiliniéaire
            vtkSmartPointer<vtkIntArray> dims = vtkSmartPointer<vtkIntArray>::New();
            this->GetDimensions(Reader->getLonCoordName(*VarNames.begin()),
                                Reader->getLatCoordName(*VarNames.begin()),
                                Reader->getVertCoordName(*VarNames.begin()),
                                dims, true);
            
            int extent[6] = { 0, dims->GetValue(0) - 1
                            , 0, dims->GetValue(1) - 1
                            , 0, dims->GetValue(2) - 1};
            outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);
         }
         
         if (soutput && !this->VarNames.empty())
         { // Grille structurée
            
            vtkSmartPointer<vtkIntArray> dims = vtkSmartPointer<vtkIntArray>::New();
            this->GetDimensions(Reader->getLonCoordName(*VarNames.begin()),
                                Reader->getVertCoordName(*VarNames.begin()),
                                dims, true);
            
            int extent[6] = { 0, dims->GetValue(0) - 1
                            , 0, dims->GetValue(1) - 1
                            , 0, dims->GetValue(2) - 1};
            outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);
         }
            
         return (1);
      }

      ///---------------------------------------------------------------

#ifndef LSCE_EXPORTS
   } // namespace vtk
} // namespace xmlioserver
#endif //LSCE_EXPORTS