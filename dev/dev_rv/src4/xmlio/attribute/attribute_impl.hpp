/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ATTRIBUTE_IMPL_HPP__
#define	__ATTRIBUTE_IMPL_HPP__

/**
 * \file    attribute_impl.hpp
 * \brief   Gestion des attributs des objets dans la hierarchie de la bibliothèque (implémentation template).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace tree {
  
   // ------------------------------- Accesseurs -------------------------------
   
   template <typename T> T CAttribute::getValue(void) const
   { 
      return (boost::any_cast<T>(this->value)); 
   }
   
   // ------------------------------- Mutateurs --------------------------------
   
   template <typename T> void CAttribute::setValue(const T & value)
   { 
      this->value = value; 
   }
   
   // --------------------------- Tests sur l'objet ----------------------------
   
   template<typename T> bool CAttribute::isType(void)
   { 
      return (this->value.type() == typeid(T)); 
   }
  
  
} // namespace tree
} // namespace xmlioserver

#endif // __ATTRIBUTE_IMPL_HPP__