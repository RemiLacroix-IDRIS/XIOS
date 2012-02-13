#ifndef __XMLIO_CGroupFactory__
#define __XMLIO_CGroupFactory__

/// boost headers ///
#include <boost/shared_ptr.hpp>

/// xmlioserver headers ///
#include "xmlioserver_spl.hpp"
#include "exception.hpp"
#include "object_factory.hpp"
#include "group_template.hpp"
#include "xml_parser.hpp"

namespace xmlioserver
{
   /// ////////////////////// Déclarations ////////////////////// ///
   class CGroupFactory
   {
      public :

         /// Mutateurs ///
         static void SetCurrentContextId(const StdString & context);

         template <typename U>
            static inline void AddGroup(boost::shared_ptr<U> pgroup,
                                        boost::shared_ptr<U> cgroup);

         template <typename U>
            static inline void AddChild(boost::shared_ptr<U> group,
                                        boost::shared_ptr<typename U::RelChild> child);

         /// Accesseurs ///
         static StdString & GetCurrentContextId(void);

         template <typename U>
            static inline boost::shared_ptr<U>
               GetGroup(boost::shared_ptr<U> group, const StdString & id);

         template <typename U>
            static inline boost::shared_ptr<typename U::RelChild>
               GetChild(boost::shared_ptr<U> group, const StdString & id);

         template <typename U>
            static inline int GetGroupNum(boost::shared_ptr<U> group);
         template <typename U>
            static inline int GetGroupIdNum(boost::shared_ptr<U> group);
         template <typename U>
            static inline int GetChildNum(boost::shared_ptr<U> group);
         template <typename U>
            static inline int GetChildIdNum(boost::shared_ptr<U> group);

         /// Tests ///
         template <typename U>
            static inline bool HasGroup(boost::shared_ptr<U> group, const StdString & id);

         template <typename U>
            static inline bool HasChild(boost::shared_ptr<U> group, const StdString & id);

         /// Instanciateur ///
         template <typename U>
            static inline boost::shared_ptr<U>
               CreateGroup(boost::shared_ptr<U> group, const StdString & id = StdString(""));

         template <typename U>
            static inline boost::shared_ptr<typename U::RelChild>
               CreateChild(boost::shared_ptr<U> group, const StdString & id = StdString(""));

      private :

         /// Propriétés statiques ///
         static StdString CurrContext;

   }; // class CGroupFactory
} // namespace xmlioserver

#include "group_factory_impl.hpp"
#include "group_parser.hpp"

#endif // __XMLIO_CGroupFactory__
