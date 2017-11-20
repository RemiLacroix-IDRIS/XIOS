#include "object_factory.hpp"

namespace xios
{
   /// ////////////////////// Définitions ////////////////////// ///

   StdString *CObjectFactory::CurrContext_ptr = 0;

   void CObjectFactory::SetCurrentContextId(const StdString & context)
   {
     if(CObjectFactory::CurrContext_ptr == 0 ) CObjectFactory::CurrContext_ptr = new StdString;
     CObjectFactory::CurrContext_ptr->assign(context); 
   }

   StdString & CObjectFactory::GetCurrentContextId(void)
   { 
     return (*CObjectFactory::CurrContext_ptr); 
   }

} // namespace xios
