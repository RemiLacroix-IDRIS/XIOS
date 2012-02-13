/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#  include <sstream>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "object.hpp"

/**
 * \file    exception.hpp
 * \brief   Gestion des erreurs levées lors de l'exécution de programme (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    9 Juin 2011
 */

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
  /**
   * \class CException
   * \brief Classe de gestion des événnements inhabituels qui surviennent lors
   * de l'exécution du programme en production.
   * \see XIOS_ERROR(id, message)
   * 
   * <i> Exemple de levée d'exception: </i>
   * \code
   * //> Lève une exception nommée 'mon_exception' contenant un message détaillé.
   * CException("mon_exception").getStream()
   * << "Ceci un message décrivant le problème rencontré durant l'exécution";
   * \endcode
   * <i> Exemple de traitement d'exception: </i>
   * \code
   * //> Traitement une exception levée lors de l'exécution d'un code.
   * try
   * {
   *    // Code générant une exception  ...
   * }
   * catch (xios::CException & _exception)
   * {
   *    // Affichage du message d'erreur et arrêt du programme.
   *    std::cerr << _exception.getMessage() << std::endl;
   *    exit(-1);
   * }
   * \endcode
   */
   class CException
      : private CObject, public std::ostringstream
   {
         typedef CObject SuperClass;                  /*!< Redéfinition de CObject. */
         typedef std::ostringstream SuperClassStream; /*!< Redéfinition de std::ostringstream. */

      public : // Constructeurs

         //! \brief Constructeur simple d'une exception anonyme.
         CException(void);

         /*!
          * \brief Constructeur simple d'une exception identifiée.
          * \param _id : une chaîne de caractère permettant d'identifier l'exception.
          */
         explicit CException(const std::string & _id);

         /*!
          * \brief Constructeur par copie .
          * \param _exception : référence de l'exception à copier.
          */
         CException(const CException & _exception);

         /*!
          * \brief Constructeur par copie.
          * \param _exception_ptr : le pointeur sur l'exception à copier.
          */
         CException(const CException * const _exception_ptr); // NIY

      public : // Accesseurs

         /*!
          * \brief Retourne un flux d'écriture du message d'erreur.
          * \return un flux d'entrée permettant de décrire l'événement survenu.
          */
         SuperClassStream & getStream(void);

         /*!
          * \brief Retourne un message d'erreur.
          * \return une chaîne de caractère décrivant l'événement survenu.
          */
         const std::string getMessage(void) const;
         
      public : // Destructeur
        
         //! \brief Destructeur de l'objet.
         virtual ~CException(void);

      public : // Autre

         virtual std::string toString(void) const;
         virtual void fromString(const std::string & _str);

      private : // Propriétés

         /*!
          * \brief Indique si l'exception doit être de nouveau levée
          * à la destruction de l'objet courant.
          */
         bool desc_rethrow;

   }; // CException

} // namespace xmlioserver

#endif	// __EXCEPTION_HPP__

