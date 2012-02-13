/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ATTRIBUTE_TEMPLATE_HPP__
#define	__ATTRIBUTE_TEMPLATE_HPP__

/**
 * \file    attribute_template.hpp
 * \brief   Gestion des attributs typés d'objet (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <string>
#include <iostream>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "attribute.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver {
   namespace tree {
  
      template <class ValueType>
         class CAttributeTemplate : public CAttribute
      {
            typedef CAttribute SuperClass;

         public : // Constructeurs

            explicit CAttributeTemplate(const std::string & _id);
            CAttributeTemplate(const std::string & _id,
                               xios_map<std::string, CAttribute*> & _umap);
            CAttributeTemplate(const std::string & _id, const ValueType & _value);
            CAttributeTemplate(const std::string & _id, const ValueType & _value,
                               xios_map<std::string, CAttribute*> & _umap);
            CAttributeTemplate(const CAttribute & _attribut_ref) throw (CException);
            CAttributeTemplate(const CAttribute * const _attribut_ptr); // Not implemented.

          public : // Accesseur

            inline ValueType getValue(void) const  throw (CException);

          public : // Mutateur

            inline void setValue(const ValueType & _value);

          public : // Destructeur

            virtual ~CAttributeTemplate(void);

          public : // Operateur

            ValueType operator=(const ValueType & _value);

          public : // Autres

            virtual std::string toString(void) const;
            virtual void fromString(const std::string & _str);

            virtual void toBinary  (std::ostream & _os) const;
            virtual void fromBinary(std::istream & _is);            

         protected : // Constructeurs

            CAttributeTemplate(void); // Not implemented.

      }; // class CAttribute  
      
      template <class T>  void FromBinary(std::istream & _is, T & _object);
     
   } // namespace tree
} // namespace xmlioserver

#endif	// __ATTRIBUTE_TEMPLATE_HPP__