#include "group_factory.hpp"

namespace xios
{
   /// ////////////////////// Définitions ////////////////////// ///
   StdString CGroupFactory::CurrContext("");
   StdString *CGroupFactory::CurrContext_ptr = new StdString;

   void CGroupFactory::SetCurrentContextId(const StdString & context)
   { 
      //CGroupFactory::CurrContext = context;
      *CGroupFactory::CurrContext_ptr = context;
   }

   StdString & CGroupFactory::GetCurrentContextId(void)
   { 
      //return (CGroupFactory::CurrContext);
      return (*CGroupFactory::CurrContext_ptr);
   }

} // namespace xios
