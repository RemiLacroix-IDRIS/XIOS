#include "object_factory.hpp"

namespace xios
{
   /// ////////////////////// Définitions ////////////////////// ///

   //StdString CObjectFactory::CurrContext("");
	 StdString *CObjectFactory::CurrContext_ptr = new StdString;

   void CObjectFactory::SetCurrentContextId(const StdString & context)
   { 
   	 //CObjectFactory::CurrContext = context; 
   	 *CObjectFactory::CurrContext_ptr = context; 
   }

   StdString & CObjectFactory::GetCurrentContextId(void)
   { 
   	 //return (CObjectFactory::CurrContext); 
   	 return (*CObjectFactory::CurrContext_ptr); 
   }

} // namespace xios
