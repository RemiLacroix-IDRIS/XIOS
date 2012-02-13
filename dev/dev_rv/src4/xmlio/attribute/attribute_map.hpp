/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ATTRIBUTE_MAP_HPP__
#define	__ATTRIBUTE_MAP_HPP__

/**
 * \file    attribute_map.hpp
 * \brief   Gestion des map d'attributs d'objets (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "attribute.hpp"
#include "attribute_template.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver {
   namespace tree {
     
      class CAttributeMap
         : public xios_map<std::string, CAttribute*>
      {
            typedef xios_map<std::string, CAttribute*> SuperClassMap;

         public : // Test

            inline bool hasAttribute(const std::string & _key) const;

         public : // Accesseur

            CAttribute * operator[](const std::string & _key) throw (CException);

         public : // Mutateur

            void setAttribute(const std::string & key, CAttribute * const _attribute) throw (CException);

            void setAttributes(const std::map <std::string, std::string> & _attributes);
            void setAttributes(const CAttributeMap * const _parent);

            void clearAllAttributes(void);

         public : // Destructeur

            virtual ~CAttributeMap(void);

         public : // Flux

            // Debug only //
            // friend StdOStream & operator << (StdOStream & os, const CAttributeMap & attributmap);

         public : // Autres

            virtual std::string toString(void) const;
            virtual void fromString(const  std::string & _str) throw (CException);
            
            virtual void toBinary  (std::ostream & _os) const;
            virtual void fromBinary(std::istream & _is) throw (CException);

         protected : // Constructeurs

            CAttributeMap(void);
            CAttributeMap(const xios_map<std::string, CAttribute*> & _umap);       // Never implemented.
            CAttributeMap(const xios_map<std::string, CAttribute*> * const _umap); // Not implemented.

         protected : //  Propriété statique

            static CAttributeMap * Current;

      };  // class CAttributeMap
   } // namespace tree
} // namespace xmlioserver

#endif	// __ATTRIBUTE_MAP_HPP__