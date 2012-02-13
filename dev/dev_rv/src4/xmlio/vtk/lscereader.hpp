#ifndef __XMLIO_LSCE_READER__
#define __XMLIO_LSCE_READER__

/// xmlioserver headers ///
#include "xmlioserver_spl.hpp"
#include "inetcdf4.hpp"
#include "inetcdf4_adv.hpp"

/// boost header ///
#include <boost/shared_ptr.hpp>

/// VTK headers ///
#include "vtkDataObjectAlgorithm.h"

#include "vtkStdString.h"

#include "vtkUnstructuredGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkRectilinearGrid.h"

#include "vtkSmartPointer.h"

#include "vtkStringArray.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkDataArraySelection.h"

#include "vtkCellData.h"
#include "vtkPointData.h"

//BTX
#ifndef LSCE_EXPORTS
namespace xmlioserver
{
   namespace vtk
   {
#endif //LSCE_EXPORTS
//ETX
      /// ////////////////////// Déclarations ////////////////////// ///

      class VTK_EXPORT vtkLSCEReader : public vtkDataObjectAlgorithm
      {
         public :
             
            //BTX
            typedef enum _gridtype
            { RECTILINEAR = 0, CURVILINEAR, UNSTRUCTURED } GridType; 
            //ETX

            /// Spécifique VTK ///
            static vtkLSCEReader * New(void);
            vtkTypeMacro(vtkLSCEReader, vtkDataObjectAlgorithm);
            void PrintSelf(ostream& os, vtkIndent indent);

   
            /// Mutateurs ///
            void SetFileName(const vtkStdString & fileName);
            void SetVariable(const vtkStdString & variable);
      
            //BTX
            void AddVariableToSelection(const vtkStdString & varName);
            void RemoveSelectedVariable(const vtkStdString & varName);
            void RemoveAllSelectedVariables(void);

            void SetGridType (GridType type);

            void AcceptTemporalOnly(bool value);
            void Accept3DOnly(bool value);
            void AcceptCellOnly(bool value);  

            //ETX
            /// Accesseurs ///
            const char * GetFileName(void) const;
            
            vtkGetMacro(ReplaceFillValueWithNan, int);
            vtkSetMacro(ReplaceFillValueWithNan, int);
            vtkBooleanMacro(ReplaceFillValueWithNan, int);
             
            vtkGetMacro(SphericalCoordinates, int);
            vtkSetMacro(SphericalCoordinates, int);
            vtkBooleanMacro(SphericalCoordinates, int);
            //BTX
            
            const std::set<vtkStdString> & GetSelectedVariables(void) const;

   
            /// Tests ///
            bool IsUnstructured(void) const;
            bool IsCurvilinear(void) const;
            bool IsRectilinear(void) const;

            bool HasSelectedVariable(void) const;

            /// Visualisation ///
            static void ShowVariable(const vtkStdString & filename,
                                     const vtkStdString & varname);
  

         protected:

            /// Constructeur ///
            vtkLSCEReader(void);

            /// Destructeur ///
            virtual ~vtkLSCEReader(void);
 
            void GetSpacings(const vtkStdString & coordinate,
                             bool bounds, vtkFloatArray * spacing);

 

            void CreateRectilinearGrid(vtkRectilinearGrid *, vtkInformation *,
                                        vtkFloatArray *, vtkFloatArray *, vtkFloatArray *,
                                        vtkIntArray *);

            void CreateStructuredGrid(vtkStructuredGrid *, vtkInformation *,
                                      vtkPoints *, vtkIntArray *);

            void CreateUnstructuredGrid(vtkUnstructuredGrid *, vtkInformation *,
                                        vtkPoints *, vtkCellArray *, int);

 

            void CreateSimpleGrid (int xi, int xf, int yi, int yf, int zi, int zf,
                                   vtkFloatArray *, vtkFloatArray *, vtkFloatArray *,
                                   vtkIntArray *);

            void CreateSimpleGrid (int xi, int xf, int yi, int yf, int zi, int zf,
                                   vtkPoints *, vtkIntArray *);

            void CreateSimpleGrid (int xi, int xf, int yi, int yf, int zi, int zf,
                                   vtkPoints *, vtkCellArray *, vtkIntArray *);

 

            void GetRectilinearConnectivity(int, int, int, vtkCellArray *);

 

            void AddPoint(vtkPoints * points, float *  value, bool proj);

            void GetPoints(const vtkStdString & xcoordinate,
                           const vtkStdString & ycoordinate,
                           const vtkStdString & zcoordinate,
                           bool bounds, bool proj,
                           vtkPoints * points, vtkIntArray * dimensions);

 

            void GetCellsAndPoints(const vtkStdString & xcoordinate,
                                   const vtkStdString & ycoordinate,
                                   const vtkStdString & zcoordinate,
                                   bool bounds, bool proj, bool clean, std::size_t nbvertex,
                                   vtkCellArray * cells, vtkPoints * points,
                                   vtkIntArray * dimensions);

 

            void AddScalarData(vtkDataSet * output, const vtkStdString & varname,
                               std::size_t record, bool bounds);
            
            void GetDimensions(const vtkStdString & xcoordinate,
                               const vtkStdString & ycoordinate,
                               const vtkStdString & zcoordinate,
                              vtkIntArray * dimensions, bool bounds);
            
            void GetDimensions(const vtkStdString & xcoordinate,
                               const vtkStdString & zcoordinate,
                               vtkIntArray * dimensions, bool bounds);
            
            void GetTimeInformation
               (const vtkStdString & variable, vtkDoubleArray * values, double * timeRange);
            
            std::size_t GetRecord(const vtkStdString & variable, const double & value);
             
            //ETX
            
            /// Traitements ///
            virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

            virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

            //virtual int RequestDataObject
            //   (vtkInformation *, vtkInformationVector **, vtkInformationVector *);

            virtual int ProcessRequest(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
            

            
         private:

            /// Constructeurs ///
            vtkLSCEReader(const vtkLSCEReader &);        // Not implemented 
            //vtkLSCEReader(const vtkLSCEReader * const);  // Not implemented  
            
            /// Opérateur ///
            void operator=(const vtkLSCEReader &);       // Not implemented    
            
            //BTX
            /// Propriétés privées ///
            vtkStdString FileName;

            GridType CurGridType;
            std::set<vtkStdString> VarNames;
            bool A3D, ATemporal, ACell, AGridDef;
            
            int ReplaceFillValueWithNan;
            int SphericalCoordinates;

            boost::shared_ptr<xmlioserver::io::CINetCDF4Adv> Reader;
            //ETX

      }; // class vtkLSCEReader
      
//BTX
#ifndef LSCE_EXPORTS
   } // namespace vtk
} // namespace xmlioserver
#endif //LSCE_EXPORTS
//ETX

#endif //__XMLIO_LSCE_READER__
