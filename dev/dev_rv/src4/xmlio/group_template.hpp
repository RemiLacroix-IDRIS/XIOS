/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __GROUP_TEMPLATE_HPP__
#define __GROUP_TEMPLATE_HPP__

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <string>
#include <iostream>
#include <vector>

// Boost headers
#include <boost/shared_ptr.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "declare_attribute.hpp"
#include "attribute_map.hpp"
#include "xml_node.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
  template <class U, class V, class W>
      class CGroupTemplate
         : public CObjectTemplate<V>, public virtual W
   {
      public : // Classe amie

         friend class CGroupFactory;

      public : // // Définition de types

         typedef U Child;
         typedef V Derived, Group;
         typedef W SuperClassAttribute;
         typedef CObjectTemplate<V> SuperClass;

      public : // Attribut spécifique des groupes

         DECLARE_ATTRIBUTE(std::string, group_ref)

      public : // Accesseurs

         const xios_map<std::string,
                           boost::shared_ptr<Group> > & getGroupMap(void) const;
         const std::vector<boost::shared_ptr<Child> > & getChildList(void) const;

         void getAllChildren(std::vector<boost::shared_ptr<Child> > & _allc) const;
         std::vector<boost::shared_ptr<Child> > getAllChildren(void) const;

      public : // Autres

         virtual std::string toString(void) const;
         virtual void fromString(const std::string & _str);
         
         virtual void toBinary  (std::ostream & _os) const;
         virtual void fromBinary(std::istream & _is);

         virtual void parse(xml::CXMLNode & _node);
         inline  void parse(xml::CXMLNode & _node, bool _withAttr);
         
      public : // Test

         virtual bool hasChild(void) const;

      public : // Accesseurs statiques

         static inline std::string GetName(void);
         static inline std::string GetDefName(void);

      public : // Traitements

         virtual void solveDescInheritance(const tree::CAttributeMap * const _parent = 0);
         void solveRefInheritance(void);

      public : // Destructeur

         virtual ~CGroupTemplate(void);

      protected : // Constructeurs

         CGroupTemplate(void);
         CGroupTemplate(const std::string & _id);
         CGroupTemplate(const CGroupTemplate<U, V, W> & _group_ref,
                        bool _withAttrList = true, bool _withId = true);    // Not implemented yet.
         CGroupTemplate(const CGroupTemplate<U, V, W> * const _group_ptr);  // Not implemented yet.

      private : // Propriétés

         xios_map<std::string,
                     boost::shared_ptr<Child> > childMap;
         std::vector<boost::shared_ptr<Child> > childList;

         xios_map<std::string,
                     boost::shared_ptr<Group> > groupMap;
         std::vector<boost::shared_ptr<Group> > groupList;

   }; // class CGroupTemplate
} // namespace xmlioserver

#endif	// __GROUP_TEMPLATE_HPP__
