/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    attribute_map.cpp
 * \brief   Gestion des map d'attributs d'objets (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "attribute_map.hpp"
#include "attribute_impl.hpp"
#include "attribute_template_impl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace tree {
  
   // -------------------------- Propriété statique ----------------------------
  
   CAttributeMap * CAttributeMap::Current = NULL;
   
   // ------------------------------ Constructeur ------------------------------
   
   CAttributeMap::CAttributeMap(void)
      : xios_map<std::string, CAttribute*>()
   { 
      CAttributeMap::Current = this; 
   }
   
   // ------------------------------- Destructeur -----------------------------
   
   CAttributeMap::~CAttributeMap(void)
   { /* Ne rien faire de plus */ }
   
   // --------------------------- Tests sur l'objet ----------------------------
   
   bool CAttributeMap::hasAttribute(const std::string & _key) const
   { 
      return (this->find(_key) != this->end()); 
   }
   
   // ------------------------------- Mutateurs --------------------------------
   
   void CAttributeMap::clearAllAttributes(void)
   {
      typedef std::pair<std::string, CAttribute*> StdStrAttPair;
      SuperClassMap::const_iterator it = SuperClassMap::begin(), end = SuperClassMap::end();
      for (; it != end; it++)
      {
         const StdStrAttPair & att = *it;
         if (!att.second->isEmpty()) att.second->clear();
      }
   }
       
   void CAttributeMap::setAttribute(const std::string & _key, CAttribute * const _attribute) throw (CException)
   {
      if (!this->hasAttribute(_key))
         XIOS_ERROR("CAttributeMap::setAttribute(key, attr)",
                    << "[ key = " << _key << "] key not found !");
      if (_attribute == NULL)
         XIOS_ERROR("CAttributeMap::setAttribute(key, attr)",
                     << "[ key = " << _key << "] attr is null !");
      this->find(_key)->second->setAnyValue(_attribute->getAnyValue());
   }
   
   void CAttributeMap::setAttributes(const std::map <std::string, std::string> & _attributes)
   {
      for (std::map <std::string, std::string>::const_iterator it  = _attributes.begin();
                                                               it != _attributes.end();
                                                               it ++)
         if ((*it).first.compare(std::string("id")) != 0 &&
             (*it).first.compare(std::string("src"))!= 0)
         {
            //if (CAttributeMap::operator[]((*it).first)->isEmpty())
            CAttributeMap::operator[]((*it).first)->fromString((*it).second);
         }
   }
   
   void CAttributeMap::setAttributes(const CAttributeMap * const _parent)
   {
      typedef std::pair<std::string, CAttribute*> StdStrAttPair;
      
      SuperClassMap::const_iterator it = _parent->begin(), end = _parent->end();
      for (; it != end; it++)
      {
         const StdStrAttPair & el = *it;
         if (this->hasAttribute(el.first))
         {
            CAttribute * currAtt = CAttributeMap::operator[](el.first);
            if (currAtt->isEmpty() && !el.second->isEmpty())
            {
               this->setAttribute(el.first, el.second);
            }
         }
      }
   }   
   
   // ------------------------------- Opérateur --------------------------------
   
   CAttribute * CAttributeMap::operator[](const std::string & _key) throw (CException)
   {
      if (!this->hasAttribute(_key))
         XIOS_ERROR("CAttributeMap::operator[](const StdString & key)",
                    << "[ key = " << _key << "] key not found !");
      return (SuperClassMap::operator[](_key));
   }
   
   // -------------------------- Conversion en chaîne --------------------------   
   
   std::string CAttributeMap::toString(void) const
   {
      typedef std::pair<std::string, CAttribute*> StdStrAttPair;
      std::ostringstream oss;
      
      SuperClassMap::const_iterator it = SuperClassMap::begin(), end = SuperClassMap::end();
      for (; it != end; it++)
      {
         const StdStrAttPair & att = *it;
         if (!att.second->isEmpty())
            oss << *att.second << " ";
      }
      return (oss.str());
   }
   
   void CAttributeMap::fromString(const std::string & _str) throw (CException)
   { 
      XIOS_ERROR("CAttributeMap::fromString(const StdString & str)",
                 << "[ str = " << _str << "] Not implemented yet !"); 
   }

   //StdOStream & operator << (StdOStream & os, const CAttributeMap & attributmap)
   //{ os << attributmap.toString(); return (os); }
   
   // -------------------------- Conversion binaire ----------------------------
   
   void CAttributeMap::toBinary(std::ostream & _os) const
   {
      typedef std::pair<std::string, CAttribute*> StdStrAttPair;
      SuperClassMap::const_iterator it = this->begin(), end = this->end();
      
      const std::size_t nbatt = SuperClassMap::size();
      _os.write (reinterpret_cast<const char*>(&nbatt) , sizeof(std::size_t));
      
      for (; it != end; it++)
      {
         const std::string & key   = it->first;
         const CAttribute  * value = it->second;            
         const std::size_t   size  = key.size();
         
         _os.write (reinterpret_cast<const char*>(&size) , sizeof(std::size_t));
         _os.write (key.data(), size * sizeof(char));
         
         if (!value->isEmpty())
         {
            bool b = true;
            _os.write (reinterpret_cast<const char*>(&b) , sizeof(bool));
            value->toBinary(_os);
         }
         else 
         {
            bool b = false;
            _os.write (reinterpret_cast<const char*>(&b) , sizeof(bool));
         }
      }
   }
   
   void CAttributeMap::fromBinary(std::istream & _is) throw (CException)
   {
      std::size_t nbatt = 0;
      _is.read (reinterpret_cast<char*>(&nbatt), sizeof(std::size_t));
      
      for (std::size_t i = 0; i < nbatt; i++)
      {
         bool hasValue = false;
         std::size_t size  = 0;
         _is.read (reinterpret_cast<char*>(&size), sizeof(std::size_t));
         std::string key(size, ' ');
         _is.read (const_cast<char *>(key.data()), size * sizeof(char));
         
         if (!this->hasAttribute(key))
            XIOS_ERROR("CAttributeMap::fromBinary(StdIStream & is)",
                        << "[ key = " << key << "] key not found !");
                                     
         _is.read (reinterpret_cast<char*>(&hasValue), sizeof(bool));
         if (hasValue) this->operator[](key)->fromBinary(_is);
      }
   }
      
} // namespace tree
} // namespace xmlioserver