/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    exception.cpp
 * \brief   Gestion des erreurs levées lors de l'exécution de programme (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    9 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// Boost headers
#include <boost/cast.hpp>
#include <boost/format.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {

   // ------------------------------ Constructeurs -----------------------------

   // Constructeur simple d'une exception anonyme.
   CException::CException(void)
      : CObject(), desc_rethrow(true)
   { /* Ne rien faire de plus */ }

   // Constructeur simple d'une exception identifiée.
   CException::CException(const std::string & _id)
      : CObject(_id), desc_rethrow(true)
   { /* Ne rien faire de plus */ }

   // Constructeur par copie.
   CException::CException(const CException & _exception)
      : std::basic_ios<char>()
      , CObject(_exception.getId())
      , std::ostringstream()
      , desc_rethrow(false)
   {
      (*this) << _exception.str();
   }

   // ------------------------------- Destructeur -----------------------------

   // Destructeur de l'instance.
   CException::~CException(void)
   {
      if (this->desc_rethrow) throw (*this);
   }

   // ------------------------------- Accesseurs ------------------------------

   // Retourne un message d'erreur.
   const std::string CException::getMessage(void) const
   {
      std::ostringstream oss;
      // oss << "> Error [" << this->getId() << "] : " << this->str();
      oss << boost::format("> Error [ %1% ] : %2%") % this->getId() % this->str(); 
      return (oss.str());
   }

   // Retourne un flux d'écriture du message d'erreur.
   std::ostringstream &  CException::getStream(void)
   {
      return (*boost::polymorphic_cast<std::ostringstream*>(this));
   }

   // --------------------------- Diverses méthodes ---------------------------

   // Retourne une représentation ascii de l'objet.
   std::string CException::toString(void) const
   {
      return (std::string(this->getMessage()));
   }

   // Modifie l'objet en fonction d'une chaîne de caractère.
   void CException::fromString(const std::string & str)
   {
      this->str(str);
   }

} // namespace xmlioserver
