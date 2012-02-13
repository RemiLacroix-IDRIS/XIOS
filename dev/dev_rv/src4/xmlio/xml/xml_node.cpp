/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    xml_node.cpp
 * \brief   Manipulation des noeuds dans l'arborescence des documents xml (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    12 Juin 2011
 */

// XMLIOServer headers
#include "xml_parser.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace xml {

   // ------------------------ Propriété statique privée -----------------------
   
   // Nom de l'élément racine du document xml.
   std::string CXMLNode::RootName("simulation");

   // ------------------------------ Constructeurs -----------------------------

   // Constructeur d'un noeud xml.
   CXMLNode::CXMLNode(rapidxml::xml_node<char> * const _root)
      : node(_root)
   { /* Ne rien faire de plus */ }
      
   // ------------------------------- Destructeur ------------------------------

   // Destructeur de l'objet.
   CXMLNode::~CXMLNode(void)
   { /* Ne rien faire de plus */ }

   // ------------------------------- Accesseurs -------------------------------

   // Retourne les valeurs d'attributs de l'élément courant (pointé dans le document xml).
   std::map <std::string, std::string> CXMLNode::getAttributes(void) const
   {
      std::map <std::string, std::string> attributes;
      rapidxml::xml_attribute<char> * currentAttr = NULL;

      if ((currentAttr = this->node->first_attribute()) != NULL)
      {
         do 
         {
            attributes.insert(std::pair<std::string, std::string>
                             (std::string(currentAttr->name()),
                              std::string(currentAttr->value())));
         } while ((currentAttr = currentAttr->next_attribute()) != NULL);
      }

      return (attributes) ;
   }
   
   // Retourne le nom de l'élément courant (pointé dans le document xml).
   std::string CXMLNode::getElementName(void) const
   { 
         return (this->node->name()); 
   }
   
   // ------------------------------- Mutateurs --------------------------------

   // Déplace le pointeur du noeud actuel vers le suivant si possible.
   bool CXMLNode::goToNextElement(void)
   {
      bool retvalue = false;
      for(rapidxml::xml_node<char> * nextElement = this->node->next_sibling();
                                   ; nextElement = this->node->next_sibling())
      {
         if (nextElement == NULL) break;
         else if (nextElement->type() == rapidxml::node_element)
         { 
            node = nextElement;
            return (!retvalue);
         }
      }
      return (retvalue);
   }

   // Déplace le pointeur du noeud actuel vers son premier enfant si possible.
   bool CXMLNode::goToChildElement(void)
   {
      bool retvalue = false;
      rapidxml::xml_node<char> * nextElement = this->node->first_node();
      if (nextElement != NULL)
      {
         for(;;nextElement = this->node->next_sibling())
         {
            if (nextElement == NULL) break;
            else if (nextElement->type() == rapidxml::node_element)
            { 
               node = nextElement; 
               return (!retvalue); 
            }
         }
      }
      return (retvalue);
   }

   // Déplace le pointeur du noeud actuel vers son parent si possible.
   bool CXMLNode::goToParentElement(void)
   {
      bool retvalue = false;
      if (!(this->getElementName().compare(CXMLNode::RootName)))
         return (retvalue);
      node = node->parent();
      return (!retvalue);
   }
   
   // -------------------------- Accesseurs statiques --------------------------

   // Retourne le nom de l'élément racine du document xml.
   const std::string & CXMLNode::GetRootName(void)
   { 
      return (CXMLNode::RootName); 
   }

} // namespace xml
} // namespace xmlioserver
