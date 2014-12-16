#include "data_output.hpp"

#include "attribute_template.hpp"
#include "group_template.hpp"
#include "context.hpp"

namespace xios
{
      /// ////////////////////// Définitions ////////////////////// ///

      CDataOutput::~CDataOutput(void)
      { /* Ne rien faire de plus */ }

      //----------------------------------------------------------------

      void CDataOutput::writeGrid(CGrid* grid)
      {
//         if (grid->domain_ref.isEmpty())
//            ERROR("CDataOutput::writeGrid(grid)",
//                   << " domain is not defined !");
//
//         if (grid->axis_ref.isEmpty())
//         {
//            this->writeGrid(CDomain::get(grid->domain_ref.getValue()));
//         }
//         else
//         {
//            this->writeGrid(CDomain::get(grid->domain_ref.getValue()),
//                            CAxis::get(grid->axis_ref.getValue()));
//         }
//        this->writeGrid(grid->domain, grid->axis);
        this->writeGrid(grid->getDomains(), grid->getAxis());
      }

      //----------------------------------------------------------------

      void CDataOutput::writeFile(CFile*  file)
      {
         this->writeFile_(file);
      }

      void CDataOutput::writeAttribute(CVariable*  var)
      {
         this->writeAttribute_(var) ;
      }

      void CDataOutput::syncFile(void)
      {
         this->syncFile_();
      }

      void CDataOutput::closeFile(void)
      {
         this->closeFile_();
      }

      //----------------------------------------------------------------

      void CDataOutput::writeGrid(CDomain* domain,CAxis* axis)
      {
         this->writeDomain_(domain);
         this->writeAxis_(axis);
      }

      void CDataOutput::writeGrid(std::vector<CDomain*> domains, std::vector<CAxis*> axis)
      {
        int domSize = domains.size();
        int aSize = axis.size();
        for (int i = 0; i < domSize; ++i) this->writeDomain_(domains[i]);
        for (int i = 0; i < aSize; ++i) this->writeAxis_(axis[i]);
      }

      //----------------------------------------------------------------

      void CDataOutput::writeGrid(CDomain* domain)
      {
         this->writeDomain_(domain);
      }

      void CDataOutput::writeTimeDimension(void)
      {
         this->writeTimeDimension_();
      }

      //----------------------------------------------------------------

      void CDataOutput::writeField(CField* field)
      {
         CContext* context = CContext::getCurrent() ;
         boost::shared_ptr<CCalendar> calendar = context->getCalendar();

         this->writeField_(field);
         this->writeTimeAxis_(field, calendar);
      }

      //----------------------------------------------------------------

      void CDataOutput::writeFieldGrid(CField* field)
      {
         this->writeGrid(field->getRelGrid());
      }

      //----------------------------------------------------------------

      void CDataOutput::writeFieldData(CField* field)
      {
//         CGrid* grid = CGrid::get(field->grid_ref.getValue());
//         CDomain* domain = CDomain::get(grid->domain_ref.getValue());
         this->writeFieldData_(field);
      }

      ///----------------------------------------------------------------

} // namespace xios
