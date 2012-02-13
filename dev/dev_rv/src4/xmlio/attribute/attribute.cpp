/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    attribute.cpp
 * \brief   Gestion des attributs des objets dans la hierarchie de la bibliothèque (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "attribute.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace tree {
  
   // ------------------------------ Constructeurs -----------------------------
   CAttribute::CAttribute(const std::string & _id)
      : CObject(_id), value()
   { /* Ne rien faire de plus */ }

   CAttribute::CAttribute(const CAttribute & _attribut_ref)
      : CObject(_attribut_ref.getId())
   { 
      this->value = _attribut_ref.getAnyValue(); 
   }
   
   // ------------------------------- Destructeur ------------------------------
   
   CAttribute::~CAttribute(void)
   { /* Ne rien faire de plus */ }
   
   // ------------------------------- Accesseurs -------------------------------

   const boost::any & CAttribute::getAnyValue(void) const
   { 
      return (this->value); 
   }

   const std::string & CAttribute::getName(void) const
   { 
      return (this->getId()); 
   }
   
   // --------------------------- Tests sur l'objet ----------------------------

   bool CAttribute::isEmpty(void) const
   { 
      return (this->value.empty()); 
   }
   
   // ------------------------------- Mutateurs --------------------------------
   
   void CAttribute::setAnyValue(const boost::any & _value)
   { 
      this->value = _value; 
   }
      
   void CAttribute::clear(void)
   {
      this->value = boost::any(); 
   }
   
} // namespace tree
} // namespace xmlioserver