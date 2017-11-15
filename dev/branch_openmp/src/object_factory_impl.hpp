#ifndef __XIOS_CObjectFactory_impl__
#define __XIOS_CObjectFactory_impl__

#include "object_factory.hpp"

namespace xios
{
   /// ////////////////////// Dfinitions ////////////////////// ///
   template <typename U>
       int CObjectFactory::GetObjectNum(void)
   {
      if (CurrContext.size() == 0)
         ERROR("CObjectFactory::GetObjectNum(void)",
               << "please define current context id !");
      //return (U::AllVectObj[CObjectFactory::CurrContext].size());
      if(U::AllVectObj_ptr == NULL) return 0;
      return (*U::AllVectObj_ptr)[CObjectFactory::CurrContext].size();
   }

   template <typename U>
      int CObjectFactory::GetObjectIdNum(void)
   {
      if (CurrContext.size() == 0)
         ERROR("CObjectFactory::GetObjectIdNum(void)",
               << "please define current context id !");
      //return (U::AllMapObj[CObjectFactory::CurrContext].size());
      if(U::AllMapObj_ptr == NULL) return 0;
      return (*U::AllMapObj_ptr)[CObjectFactory::CurrContext].size();
   }

   template <typename U>
      bool CObjectFactory::HasObject(const StdString & id)
   {
      if (CurrContext.size() == 0)
         ERROR("CObjectFactory::HasObject(const StdString & id)",
               << "[ id = " << id << " ] please define current context id !");
      //return (U::AllMapObj[CObjectFactory::CurrContext].find(id) !=
      //        U::AllMapObj[CObjectFactory::CurrContext].end());
      if(U::AllMapObj_ptr  == NULL)  return false;
      return ((*U::AllMapObj_ptr)[CObjectFactory::CurrContext].find(id) !=
              (*U::AllMapObj_ptr)[CObjectFactory::CurrContext].end());

   }

   template <typename U>
      bool CObjectFactory::HasObject(const StdString & context, const StdString & id)
   {

      // if (U::AllMapObj.find(context) == U::AllMapObj.end()) return false ;
      // else return (U::AllMapObj[context].find(id) !=  U::AllMapObj[context].end());
      if(U::AllMapObj_ptr  == NULL) return false;

      if (U::AllMapObj_ptr->find(context) == U::AllMapObj_ptr->end()) return false ;
      else return ((*U::AllMapObj_ptr)[context].find(id) !=  (*U::AllMapObj_ptr)[context].end());
   }

   template <typename U>
      boost::shared_ptr<U> CObjectFactory::GetObject(const U * const object)
   {
      if(U::AllVectObj_ptr == NULL) return (boost::shared_ptr<U>());
      if (CurrContext.size() == 0)
         ERROR("CObjectFactory::GetObject(const U * const object)",
               << "please define current context id !");
      //std::vector<boost::shared_ptr<U> > & vect = U::AllVectObj[CObjectFactory::CurrContext];
      std::vector<boost::shared_ptr<U> > & vect = (*U::AllVectObj_ptr)[CObjectFactory::CurrContext];

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
      if(U::AllMapObj_ptr  == NULL) return (boost::shared_ptr<U>());
      if (CurrContext.size() == 0)
         ERROR("CObjectFactory::GetObject(const StdString & id)",
               << "[ id = " << id << " ] please define current context id !");
      if (!CObjectFactory::HasObject<U>(id))
         ERROR("CObjectFactory::GetObject(const StdString & id)",
               << "[ id = " << id << ", U = " << U::GetName() << " ] "
               << "object was not found.");

      //cout<<"CObjectFactory::GetObject(const StdString & id)[ id = " << id << ", U = " << U::GetName() << " ] "<<endl;
      if(id == "src_domain_regular_read")
      {
         //cout<<"match"<<endl;
      }

      //return (U::AllMapObj[CObjectFactory::CurrContext][id]);
      return (*U::AllMapObj_ptr)[CObjectFactory::CurrContext][id];
   }

   template <typename U>
      boost::shared_ptr<U> CObjectFactory::GetObject(const StdString & context, const StdString & id)
   {
      if(U::AllMapObj_ptr  == NULL) return (boost::shared_ptr<U>());

      if (!CObjectFactory::HasObject<U>(context,id))
         ERROR("CObjectFactory::GetObject(const StdString & id)",
               << "[ id = " << id << ", U = " << U::GetName() <<", context = "<<context<< " ] "
               << "object was not found.");

      //cout<<"CObjectFactory::GetObject(const StdString & context, const StdString & id)[ id = " << id << ", U = " << U::GetName() << " ] "<<endl;
      if(id == "src_domain_regular_read")
      {
         //cout<<"match"<<endl;
         boost::shared_ptr<U> value;
      }
      //return (U::AllMapObj[context][id]);
      return (*U::AllMapObj_ptr)[context][id];
   }

   template <typename U>
   boost::shared_ptr<U> CObjectFactory::CreateObject(const StdString& id)
   {
      if(U::AllVectObj_ptr == NULL) U::AllVectObj_ptr = new xios_map<StdString, std::vector<boost::shared_ptr<U> > >;
      if(U::AllMapObj_ptr  == NULL) U::AllMapObj_ptr  = new xios_map<StdString, xios_map<StdString, boost::shared_ptr<U> > >;

      if (CurrContext.empty())
         ERROR("CObjectFactory::CreateObject(const StdString& id)",
               << "[ id = " << id << " ] please define current context id !");

      if (CObjectFactory::HasObject<U>(id))
      {
         return CObjectFactory::GetObject<U>(id);
      }
      else
      {
         boost::shared_ptr<U> value(new U(id.empty() ? CObjectFactory::GenUId<U>() : id));

         //U::AllVectObj[CObjectFactory::CurrContext].insert(U::AllVectObj[CObjectFactory::CurrContext].end(), value);
         //U::AllMapObj[CObjectFactory::CurrContext].insert(std::make_pair(value->getId(), value));

         (*U::AllVectObj_ptr)[CObjectFactory::CurrContext].insert((*U::AllVectObj_ptr)[CObjectFactory::CurrContext].end(), value);
         (*U::AllMapObj_ptr) [CObjectFactory::CurrContext].insert(std::make_pair(value->getId(), value));
         //cout<<"CObjectFactory::CreateObject(const StdString& id) [ id = " << id << " ]" <<endl;

         return value;
      }
   }

   template <typename U>
      const std::vector<boost::shared_ptr<U> > &
         CObjectFactory::GetObjectVector(const StdString & context)
   {
      //return (U::AllVectObj[context]);
      return (*U::AllVectObj_ptr)[context];
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
      if(U::GenId_ptr == NULL) U::GenId_ptr = new xios_map< StdString, long int >;
      oss << GetUIdBase<U>() << (*U::GenId_ptr)[CObjectFactory::CurrContext]++;
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
