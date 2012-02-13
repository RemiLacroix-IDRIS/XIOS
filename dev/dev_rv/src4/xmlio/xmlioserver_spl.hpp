/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
/**
 * \file    xmlioserver_spl.hpp
 * \brief   Entête de base pour chacun des fichiers du projet.
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

#ifndef __XMLIO_SPL__
#define __XMLIO_SPL__

// XMLIOServer headers
#include "exception.hpp"
#include "exception_mac.hpp"

// Macro //
//! Définit le paramètre d'une fonction comme étant inutilisé.
#define XIOS_UNUSED(param)

//! Définit le type des map.
#include <map>
#define xios_map std::map

#endif //__XMLIO_SPL__
