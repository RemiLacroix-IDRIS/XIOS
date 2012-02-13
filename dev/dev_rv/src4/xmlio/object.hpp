/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

/**
 * \file    object.hpp
 * \brief   Base de tous les objets du projet XMLIOSERVER (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <string>
#include <iostream>

// Boost headers
#include <boost/optional.hpp>

#endif // __XIOS_NO_EXTERN

// ////////////////////////////// Déclarations ///////////////////////////// //


/// \brief Espace de nommage regroupant toutes les sources du projet.
namespace xmlioserver
{
   /**
    * \class CObject
    * \brief Classe abstraite de base dans l'arborescence d'objets de la bibliothèque.
    */
   class CObject
   {
      public : // Destructeur
         
         //! \brief Destructeur de l'objet.
         virtual ~CObject(void);

      public : // Accesseurs

         /*!
          * \brief Retourne l'identifiant de l'objet.
          * \return la chaîne de caractères permettant d'identifier l'objet.
          */
         const std::string & getId(void) const;

      public : // Mutateurs

         /// \brief Supprime l'identifiant de l'objet, rendant ce dernier anonyme.
         void resetId(void);

         /*!
          * \brief Assigne un identifiant à l'objet courant.
          * \param _new_id : une chaîne de caractères identifiant l'objet.
          * \see id
          */
         void setId(const std::string & _new_id);

      public : // Tests sur l'objet

         /*!
          * \brief Indique si l'objet est identifié.
          * \return <b>true</b> si l'objet est identifié, <b>false</b> sinon.
          */
         bool hasId(void) const;

      public : // Opérateurs de comparaison

         /*!
          * \brief Indique si deux objets sont identiques.
          * \param _other_obj : objet à comparer avec l'élément courant.
          * \return <b>true</b> si les objets sont identiques, <b>false</b> sinon.
          * \remarks Deux objets sont identiques s'ils sont du même type et ont le même identifiant.
          */
         bool operator == (const CObject & _other_obj) const;

         /*!
          * \brief Indique si deux objets sont différents.
          * \param _other_obj : l'objet à comparer avec l'élément courant.
          * \return <b>true</b> si les objets sont différents, <b>false</b> sinon.
          * \remarks Deux objets sont identiques s'ils sont du même type et ont le même identifiant.
          */
         bool operator != (const CObject & _other_obj) const;

      public :// Flux de sortie

         /*!
          * \brief Opérateur de flux de sortie ascii.
          * \param _os : un objet de flux de sortie standard.
          * \param _object : l'objet à sérialiser sous forme ascii.
          * \return Le paramètre _os éventuellement modifié après appel de la méthode.
          * \see toString
          */
         friend std::ostream & operator << (std::ostream & _os, const CObject & _object);

      public : // virtual - Autres méthodes


         /*!
          * \brief Retourne une représentation ascii de l'objet.
          * \return Une représentation ascii de l'objet
          * \see fromString
          */
         virtual std::string toString(void) const = 0;

         /*!
          * \brief Modifie l'objet en fonction d'une chaîne de caractère.
          * \param _object_str : une représentation ascii de l'objet
          * \see toString
          */
         virtual void fromString(const std::string & _object_str) = 0;

      protected : // Constructeurs

          //! \brief Constructeur simple d'un objet anonyme (ie sans identifiant).
         CObject(void);

         /*!
          * \brief Constructeur simple d'un objet identifié.
          * \param _id : une chaîne de caractère permettant d'identifier l'objet.
          * \see id
          */
         explicit CObject(const std::string & _id);

         /*!
          * \brief Constructeur par copie .
          * \remarks L'objet d'origine (<i>référencé</i>) n'est jamais modifié.
          * \param _object : référence de l'objet à copier.
          */
         CObject(const CObject & _object);

         /*!
          * \brief Constructeur par copie.
          * \remarks L'objet d'origine (<i>pointé</i>) n'est jamais modifié.
          * \param _object_ptr : le pointeur sur l'objet à copier.
          */
         CObject(const CObject * const _object_ptr); // NIY

      private : // Propriétés

         boost::optional<std::string> id ;  /*!< \brief (optionnel) Identifiant de l'objet. */

   }; // class CObject

} // namespace xmlioserver

#endif	// __OBJECT_HPP__

