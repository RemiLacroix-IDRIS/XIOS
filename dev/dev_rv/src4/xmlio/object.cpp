/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    object.cpp
 * \brief   Base de tous les objets du projet XMLIOSERVER (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// Boost headers
#include <boost/none.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "object.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {

   // ------------------------------ Constructeurs -----------------------------

   //- Constructeur simple d'un objet anonyme (ie sans identifiant).
   CObject::CObject(void) : id()
   { /* Ne rien faire de plus */ }

   //- Constructeur simple d'un objet identifié.
   CObject::CObject(const std::string & id) : id(id)
   { /* Ne rien faire de plus */ }

   //- Constructeur par copie.
   CObject::CObject(const CObject & object)
      : id(object.id)
   { /* Ne rien faire de plus */ }

   // ------------------------------- Destructeur ------------------------------

   //- Destructeur de l'objet.
   CObject::~CObject(void)
   { /* Ne rien faire de plus */ }

   // ------------------------------- Accesseurs -------------------------------

   //- Retourne l'identifiant de l'objet.
   const std::string & CObject::getId(void) const
   {
      return (this->id.get());
   }

   // --------------------------- Tests sur l'objet ----------------------------

   //- Indique si l'objet est identifié.
   bool CObject::hasId(void) const
   {
      return (this->id);
   }

   // ------------------------------- Mutateurs --------------------------------

   //- Supprime l'identifiant de l'objet, rendant ce dernier anonyme.
   void CObject::resetId(void)
   {
      this->id = boost::none;
   }

   //- Assigne un identifiant à l'objet courant.
   void CObject::setId(const std::string & id)
   {
      this->id = id ;
   }

   // ----------------------- Opérateurs de comparaison ------------------------

   //- Indique si deux objets sont identiques.
   bool CObject::operator==(const CObject & other) const
   {
      if(!this->hasId() || !other.hasId()) return (false);
      return (this->id.get().compare(other.id.get()) == 0);
   }

   //- Indique si deux objets sont différents.
   bool CObject::operator!=(const CObject & other) const
   {
      return (!(*this == other));
   }

   // --------------------------- Flux de sortie -------------------------------

   //- Opérateur de flux de sortie ascii. 
   std::ostream & operator << (std::ostream & _os, const CObject & _object)
   {
      _os << _object.toString();
      return (_os);
   }

} // namespace xmlioserver
