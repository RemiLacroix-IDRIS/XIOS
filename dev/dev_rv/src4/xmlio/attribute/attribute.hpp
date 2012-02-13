/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ATTRIBUTE_HPP__
#define	__ATTRIBUTE_HPP__

/**
 * \file    attribute.hpp
 * \brief   Gestion des attributs des objets dans la hierarchie de la bibliothèque (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <string>
#include <iostream>

// boost headers
#include <boost/any.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "object.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver 
{
   namespace tree
   {
      /**
      * \class CAttribute
      * \brief Classe de gestion des attributs non-typés et optionnels
      * pour les différents objets de la bibliothèque.
      */
      class CAttribute : public CObject
      {
            typedef CObject SuperClass;

         public : // Constructeurs publics

            explicit CAttribute(const std::string & _id);
            CAttribute(const CAttribute & _attribut_ref);
            CAttribute(const CAttribute * const _attribut_ptr); // Not implemented.

        public : // Accesseurs

            const std::string & getName(void) const;
            const boost::any & getAnyValue(void) const;
            template <typename T> inline T getValue(void) const;

        public : // Mutateurs

            template <typename T> inline void setValue(const T & _new_value);
            void setAnyValue(const boost::any & _new_value);
            void clear(void);

        public : // Test ///

            bool isEmpty(void) const;
            template <typename T> inline bool isType(void);

        public : // Destructeur ///

            virtual ~CAttribute(void);

        public : // Autres

            virtual std::string toString(void) const          = 0;
            virtual void fromString(const std::string & _str) = 0;

            virtual void toBinary  (std::ostream & _os) const = 0;
            virtual void fromBinary(std::istream & _is)       = 0;

         protected :// Constructeur protégé

            CAttribute(void);  // Not implemented.

         private : // Propriété

            boost::any value;

      }; // class CAttribute
   } // namespace tree
} // namespace xmlioserver

#endif	// __ATTRIBUTE_HPP__