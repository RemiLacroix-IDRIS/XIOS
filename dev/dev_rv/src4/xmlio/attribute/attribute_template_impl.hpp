/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ATTRIBUTE_TEMPLATE_IMPL_HPP__
#define	__ATTRIBUTE_TEMPLATE_IMPL_HPP__

/**
 * \file    attribute_template_impl.hpp
 * \brief   Gestion des attributs typés d'objet (implémentation template).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace tree {
  
   // ------------------------------ Constructeurs -----------------------------
   
   template <class ValueType>
      CAttributeTemplate<ValueType>::CAttributeTemplate(const std::string & _id)
         : CAttribute(_id)
   { /* Ne rien faire de plus */ }

   template <class ValueType>
      CAttributeTemplate<ValueType>::CAttributeTemplate(const std::string & _id, const ValueType & _value)
	 : CAttribute(_id)
   {
      this->setValue(_value);
   }

   template <class ValueType>
      CAttributeTemplate<ValueType>::CAttributeTemplate(const CAttribute & _attribute_ref) throw (CException)
         : CAttribute(_attribute_ref)
   {
      if (!_attribute_ref.isEmpty() && !_attribute_ref.isType<ValueType>())
         XIOS_ERROR("CAttributeTemplate(_attribute_ref)", << "Invalid instantiation !");
   }

   template <class ValueType>
      CAttributeTemplate<ValueType>::CAttributeTemplate
	(const std::string & _id, xios_map<std::string, CAttribute*> & _umap)
           : CAttribute(_id)
   {
      _umap.insert(_umap.end(), std::make_pair(_id, this));
   }

   template <class ValueType>
      CAttributeTemplate<ValueType>::CAttributeTemplate
         (const std::string & _id, const ValueType & _value, xios_map<std::string, CAttribute*> & _umap)
            : CAttribute(_id)
   {
      this->setValue(_value);
      _umap.insert(_umap.end(), std::make_pair(_id, this));
   }
   
   // ------------------------------- Destructeur ------------------------------
   
   template <class ValueType>
      CAttributeTemplate<ValueType>::~CAttributeTemplate(void)
   { 
      this->clear();
   }
  
   // ------------------------------- Accesseur --------------------------------

   template <class ValueType>
      ValueType CAttributeTemplate<ValueType>::getValue(void) const  throw (CException)
   {
      if (SuperClass::isEmpty())
         XIOS_ERROR("ValueType CAttributeTemplate<ValueType>::getValue(void) const",
                     << "[ id = " << this->getId() << "]"
                     << " L'attribut est requis mais n'est pas défini !");
      return (SuperClass::getValue<ValueType>());
   }
      
   // ------------------------------- Mutateur ---------------------------------
   
   template <class ValueType>
      void CAttributeTemplate<ValueType>::setValue(const ValueType & _value)
   {
      SuperClass::setValue<ValueType>(_value);
   }

   // ------------------------------- Opérateur --------------------------------

   template <class ValueType>
      ValueType CAttributeTemplate<ValueType>::operator=(const ValueType & _value)
   {
      this->setValue(_value);  return (this->getValue());
   }

   // -------------------------- Conversion en chaîne --------------------------

   template <class ValueType>
      std::string CAttributeTemplate<ValueType>::toString(void) const
   {
      std::ostringstream oss;
      if (!this->isEmpty() && this->hasId())
         oss << this->getName() << "=\"" << this->getValue() << "\"";
      return (oss.str());
   }

   template <class ValueType>
      void CAttributeTemplate<ValueType>::fromString(const std::string & _str)
   {
      XIOS_ERROR("CAttributeTemplate<ValueType>::fromString(const std::string & str)",
                  << "[ str = " << _str << " ] Not implemented yet !");
   }

   // -------------------------- Conversion binaire ----------------------------

   template <class ValueType>
     void CAttributeTemplate<ValueType>::toBinary (std::ostream & _os) const
   {
      this->getValue()->toBinary(_os);
   }

   template <class ValueType>
      void CAttributeTemplate<ValueType>::fromBinary(std::istream & _is)
   {
      ValueType value;
      FromBinary(_is, value);
      this->setValue(value);
   }  
      
} // namespace tree
} // namespace xmlioserver

#endif	// __ATTRIBUTE_TEMPLATE_IMPL_HPP__