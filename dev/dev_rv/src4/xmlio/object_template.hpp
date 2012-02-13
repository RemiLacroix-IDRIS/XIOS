/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __OBJECT_TEMPLATE_HPP__
#define __OBJECT_TEMPLATE_HPP__

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
#include "xml_node.hpp"
#include "attribute_map.hpp"
#include "node_enum.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   template <class T>
      class CObjectTemplate
         : public CObject
         , public virtual tree::CAttributeMap
   {

      public : // Classe amie

         friend class CObjectFactory;

      public : // Définition de types

         typedef tree::CAttributeMap SuperClassMap;
         typedef CObject SuperClass;
         typedef T DerivedType;

      public : // Autres méthodes

         virtual std::string toString(void) const;
         virtual void fromString(const std::string & _str);

         virtual void toBinary  (std::ostream & _os) const;
         virtual void fromBinary(std::istream & _is);

         virtual void parse(xml::CXMLNode & _node);
         
      public : // Accesseur

         tree::ENodeType getType(void) const;

      public : // Test

         virtual bool hasChild(void) const;

      public : // Traitement

         virtual void solveDescInheritance(const tree::CAttributeMap * const _parent = 0);

      public : // Traitement statique

         static void ClearAllAttributes(void);

      public : // Accesseur statique

         static std::vector<boost::shared_ptr<DerivedType> > &
            GetAllVectobject(const std::string & _contextId);

      public : // Destructeur

         virtual ~CObjectTemplate(void);

      protected : // Constructeurs

         CObjectTemplate(void);
         explicit CObjectTemplate(const std::string & _id);
         CObjectTemplate(const CObjectTemplate<T> & _object_ref,
                         bool _withAttrList = true, bool _withId = true);
         CObjectTemplate(const CObjectTemplate<T> * const _object_ptr); // Not implemented.

      private : // Propriétés statiques

         static xios_map<std::string,
                xios_map<std::string,
                boost::shared_ptr<DerivedType> > > AllMapObj; 
         static xios_map<std::string,
                std::vector<boost::shared_ptr<DerivedType> > > AllVectObj;

   }; // class CObjectTemplate
} // namespace xmlioserver

#endif	// __OBJECT_TEMPLATE_HPP__