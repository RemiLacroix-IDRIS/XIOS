#include "group_factory.hpp"

namespace xios
{
   /// ////////////////////// Définitions ////////////////////// ///
   StdString *CGroupFactory::CurrContext_ptr = new StdString;

   void CGroupFactory::SetCurrentContextId(const StdString & context)
   {  
      if(CGroupFactory::CurrContext_ptr == NULL ) CGroupFactory::CurrContext_ptr = new StdString; 
      CGroupFactory::CurrContext_ptr->assign(context);
   }

   StdString & CGroupFactory::GetCurrentContextId(void)
   { 
      return (*CGroupFactory::CurrContext_ptr);
   }

} // namespace xios
