/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __EXCEPTION_MAC_HPP__
#define	__EXCEPTION_MAC_HPP__

/**
 * \file    exception_mac.hpp
 * \brief   Gestion des erreurs levées lors de l'exécution de programme (macros).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    9 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// Boost headers
#include <boost/format.hpp>

#endif // __XIOS_NO_EXTERN

// ///////////////////////////////// Macros //////////////////////////////// //

/*! \def   XIOS_INFO(message)
 *  \brief Ecriture générique d'informations relatives au positionnement d'une portion de code.
 */
/*! \def   XIOS_ERROR(id, message)
 *  \brief Génération d'une exception détaillée durant l'exécution du programme.
 */
/*! \def   XIOS_ERROR_FORMAT(id, , format_str, data)
 *  \brief Génération d'une exception formatée durant l'exécution du programme.
 */
/*! \def   XIOS_DEBUG(message)
 *  \brief Envoi d'un message de débogage sur la sortie de logging durant l'exécution du programme.
 */

#define XIOS_INFO(message) \
   "In file \'" __FILE__ "\', line " << __LINE__ << " -> " message << std::endl

#define XIOS_ERROR(id, message) \
	xmlioserver::CException(id).getStream() << XIOS_INFO(message)

#define XIOS_ERROR_FORMAT(id, format_str, data) \
   xmlioserver::CException(id).getStream() << XIOS_INFO(<<boost::format(format_str)data)

#ifdef  __XIOS_DEBUG
#  define XIOS_DEBUG(message) std::clog << "> Debug " << XIOS_INFO(message)
#else
#  define XIOS_DEBUG(message)
#endif

#endif	// __EXCEPTION_MAC_HPP__

