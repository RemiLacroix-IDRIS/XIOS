#ifndef __XIOS_CObjectFactory_impl__
#define __XIOS_CObjectFactory_impl__

#include "object_factory.hpp"

namespace xios
{
   /// ////////////////////// Définitions ////////////////////// ///
   template <typename U>
       int CObjectFactory::GetObjectNum(void)
   {
      if (CurrContext_ptr->size() == 0)
         ERROR("CObjectFactory::GetObjectNum(void)",
               << "please define current context id !");

      if(U::AllVectObj == NULL) return 0;
      
      
      return (*U::AllVectObj)[*CObjectFactory::CurrContext_ptr].size();
   }

   template <typename U>
      int CObjectFactory::GetObjectIdNum(void)
   {
      if (CurrContext_ptr->size() == 0)
         ERROR("CObjectFactory::GetObjectIdNum(void)",
               << "please define current context id !");
      if(U::AllMapObj  == NULL) return 0;

      

      return (* U::AllMapObj) [*CObjectFactory::CurrContext_ptr].size();
   }

   template <typename U>
      bool CObjectFactory::HasObject(const StdString & id)
   {
      if (CurrContext_ptr->size() == 0)
         ERROR("CObjectFactory::HasObject(const StdString & id)",
               << "[ id = " << id << " ] please define current context id !");
      
      if(U::AllMapObj  == NULL)  return false;

      

      return ((*U::AllMapObj)[*CObjectFactory::CurrContext_ptr].find(id) !=
              (*U::AllMapObj)[*CObjectFactory::CurrContext_ptr].end());
   }

   template <typename U>
      bool CObjectFactory::HasObject(const StdString & context, const StdString & id)
   {
      if(U::AllMapObj  == NULL) return false;

      if (U::AllMapObj->find(context) == U::AllMapObj->end()) return false ;

      else
      {
         return ((*U::AllMapObj)[context].find(id) !=  (*U::AllMapObj)[context].end());
      } 
         
   }

   template <typename U>
      boost::shared_ptr<U> CObjectFactory::GetObject(const U * const object)
   {
      if(U::AllVectObj == NULL) return (boost::shared_ptr<U>());
   
      if (CurrContext_ptr->size() == 0)
         ERROR("CObjectFactory::GetObject(const U * const object)",
               << "please define current context id !");
      std::vector<boost::shared_ptr<U> > & vect =
                     (*U::AllVectObj)[*CObjectFactory::CurrContext_ptr];

      typename std::vector<boost::shared_ptr<U> >::const_iterator
         it = vect.begin(), end = vect.end();

      for (; it != end; it++)
      {
         boost::shared_ptr<U> ptr = *it;
         if (ptr.get() == object)
            return (ptr);
      }

      ERROR("CObjectFactory::GetObject(const U * const object)",
               << "[type = " << U::GetName() << ", adress = " << object << "] "
               << "object was not found.");
      return (boost::shared_ptr<U>()); // jamais atteint
   }

   template <typename U>
      boost::shared_ptr<U> CObjectFactory::GetObject(const StdString & id)
   {
      if(U::AllMapObj  == NULL) return (boost::shared_ptr<U>());

      if (CurrContext_ptr->size() == 0)
         ERROR("CObjectFactory::GetObject(const StdString & id)",
               << "[ id = " << id << " ] please define current context id !");
      if (!CObjectFactory::HasObject<U>(id))
         ERROR("CObjectFactory::GetObject(const StdString & id)",
               << "[ id = " << id << ", U = " << U::GetName() << " ] "
               << "object was not found.");
      return (*U::AllMapObj)[*CObjectFactory::CurrContext_ptr][id];
   }

   template <typename U>
      boost::shared_ptr<U> CObjectFactory::GetObject(const StdString & context, const StdString & id)
   {
      if(U::AllMapObj  == NULL) return (boost::shared_ptr<U>());

      if (!CObjectFactory::HasObject<U>(context,id))
         ERROR("CObjectFactory::GetObject(const StdString & id)",
               << "[ id = " << id << ", U = " << U::GetName() <<", context = "<<context<< " ] "
               << "object was not found.");

      return (*U::AllMapObj)[context][id];
   }

   template <typename U>
   boost::shared_ptr<U> CObjectFactory::CreateObject(const StdString& id)
   {
      if(U::AllVectObj == NULL) U::AllVectObj = new xios_map<StdString, std::vector<boost::shared_ptr<U> > >;
      if(U::AllMapObj  == NULL) U::AllMapObj  = new xios_map<StdString, xios_map<StdString, boost::shared_ptr<U> > >;

      printf("so far so good : %d %d %d\n", CurrContext_ptr->empty(), CObjectFactory::HasObject<U>(id), id.empty());
      
      if (CurrContext_ptr->empty())
         ERROR("CObjectFactory::CreateObject(const StdString& id)",
               << "[ id = " << id << " ] please define current context id !");

      if (CObjectFactory::HasObject<U>(id))
      {
         return CObjectFactory::GetObject<U>(id);
      }
      else
      {
         boost::shared_ptr<U> value(new U(id.empty() ? CObjectFactory::GenUId<U>() : id));
         printf("in CreateObject, value OK\n");  
         (* U::AllVectObj)[*CObjectFactory::CurrContext_ptr].insert((*U::AllVectObj)[*CObjectFactory::CurrContext_ptr].end(), value);
         printf("AllVectObj insert OK\n");
         (* U::AllMapObj) [*CObjectFactory::CurrContext_ptr].insert(std::make_pair(value->getId(), value));
         printf("AllMapObj insert OK\n");

         return value;
      }
   }

   template <typename U>
      const std::vector<boost::shared_ptr<U> > &
         CObjectFactory::GetObjectVector(const StdString & context)
   {
      if(U::AllVectObj != NULL) 
      

      return (*U::AllVectObj)[context];
   }

   template <typename U>
   const StdString& CObjectFactory::GetUIdBase(void)
   {
      static const StdString base = "__" + U::GetName() + "_undef_id_";
      return base;
   }

   template <typename U>
   StdString CObjectFactory::GenUId(void)
   {
      StdOStringStream oss;
      if(U::GenId == NULL) U::GenId = new xios_map< StdString, long int >;
      oss << GetUIdBase<U>() << (*U::GenId)[*CObjectFactory::CurrContext_ptr]++;
      return oss.str();
   }

   template <typename U>
   bool CObjectFactory::IsGenUId(const StdString& id)
   {
      const StdString& base = GetUIdBase<U>();
      return (id.size() > base.size() && id.compare(0, base.size(), base) == 0);
   }

} // namespace xios

#endif // __XIOS_CObjectFactory_impl__
