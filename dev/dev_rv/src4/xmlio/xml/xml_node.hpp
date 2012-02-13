/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __XML_NODE_HPP__
#define	__XML_NODE_HPP__

/**
 * \file    xml_node.hpp
 * \brief   Manipulation des noeuds dans l'arborescence des documents xml (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    12 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <map>

// rapidXML headers
#include <rapidxml.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   namespace xml
   {
      /**
       * \class CXMLNode
       * \brief Classe de gestion des noeuds dans les documents XML.
       */
      class CXMLNode
      {
         public : // Constructeur publique

            /*!
             * \brief Constructeur d'un noeud xml.
             * \param _root : un noeud xml dans la bibliothèque rapidxml
             * \see node
             *
             * <i> Exemple d'utilisation: </i>
             * \code
             * //> Création d'un noeud xml à partir de rapidxml.
             * char * c_str =  "<? xml version=1.0 ?><simulation></simulation>";
             * rapidxml::xml_document<char> xmldoc;
             * xmldoc.parse<0>(c_str);
             * CXMLNode node(doc.first_node());
             * \endcode
             */
            CXMLNode(rapidxml::xml_node<char> * const _root);

         public :// Destructeur

            //! \brief Destructeur de l'objet.
            ~CXMLNode(void);

         public : // Accesseurs

            /*!
             * \brief Retourne le nom de l'élément courant (pointé dans le document xml).
             * \return l'identifiant de l'élément.
             *
             * <i> Appliquée au document xml ci_dessous, la méthode retourne pour le noeud racine
             * la chaîne <b>element_name</b> </i>
             * \code
             * <? xml version=1.0 ?>
             * <element_name attrib_cle1 = "attrib_value1" attrib_cle2 = "attrib_value2">
             * Ceci est un contenu.
             * </element_name>
             * \endcode
             */
            std::string getElementName(void) const;

            /*!
             * \brief Retourne les valeurs d'attributs de l'élément courant (pointé dans le document xml).
             * \return une map d'attributs identifiés par des clés dans le conteneur.
             *
             * <i> Appliquée  au document xml ci_dessous, la méthode retourne pour le noeud racine la map : </i>
             * <UL> 
             *     <LI>attrib_cle1 -> attrib_value1
             *     <LI>attrib_cle2 -> attrib_value2
             * </UL>
             * \code
             * <? xml version=1.0 ?>
             * <element_name attrib_cle1 = "attrib_value1" attrib_cle2 = "attrib_value2">
             * Ceci est un contenu.
             * </element_name>
             * \endcode
             */
            std::map <std::string, std::string> getAttributes(void) const;

         public : // Mutateurs

            /*!
             * \brief Déplace le pointeur du noeud actuel vers le suivant si possible.
             * \return <b>true</b> si le noeud pointe désormais sur l'élément suivant,
             *  <b>false</b> s'il n'y a pas d'élément suivant.
             * \see goToChildElement goToParentElement
             */
            bool goToNextElement(void);

            /*!
             * \brief Déplace le pointeur du noeud actuel vers son premier enfant si possible.
             * \return <b>true</b> si le noeud pointe désormais sur l'élément enfant,
             *  <b>false</b> s'il n'y a pas d'élément enfant.
             *  \see goToNextElement goToParentElement
             */
            bool goToChildElement(void);

            /*!
             * \brief Déplace le pointeur du noeud actuel vers son parent si possible.
             * \return <b>true</b> si le noeud pointe désormais sur l'élément parent,
             *  <b>false</b> s'il n'y a pas d'élément parent.
             * \see goToChildElement goToNextElement
             */
            bool goToParentElement(void);

         public : // Accesseurs statiques

            /*!
             * \brief Retourne le nom de l'élément racine du document xml.
             * \return le nom de l'élément racine du document xml
             * \see RootName
             */
            static const std::string & GetRootName(void);

         protected : // Constructeurs protégés

            //! \brief Constructeur simple.
            CXMLNode(void);                            // NIY

            /*!
             * \brief Constructeur par copie .
             * \param _node : référence du noeud à copier.
             */
            CXMLNode(const CXMLNode & _node);          // NIY

            /*!
             * \brief Constructeur par copie.
             * \param _node_ptr : le pointeur du noeud à copier.
             */
            CXMLNode(const CXMLNode * const _node_ptr); // NIY

         private : // Propriété privée

            //! \brief Pointeur sur un objet de type noeud dans la bibliothèque rapidxml.
            rapidxml::xml_node<char> * node;

         private : // Propriété statique privée

            //! \brief Nom de l'élément racine du document xml.
            static std::string RootName;

      }; //class CXMLParser

   }// namespace xml
} // namespace xmlioserver

#endif	// __XML_NODE_HPP__

