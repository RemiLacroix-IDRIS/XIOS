/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
/**
 * \file    main_server.cpp
 * \brief   Programme principal du projet.
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 *
 * Ce fichier contient le point d'entrée du programme permettant
 * de lancer le serveur d'Entrée/Sortie parallèle.
 */

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

#include "mpi_interface.hpp"
//~ #include "onetcdf4.hpp"
//~ #include "onetcdf4_impl.hpp"
#include "array_util.hpp"
#include "array_util_impl.hpp"

namespace xios = xmlioserver;
//~ using namespace xios::data;
//~ using namespace xios::io;
//~ using namespace xios::tree;
//~ using namespace xios::xml;
using namespace xios::comm;
//~ using namespace xios::func;

#include <boost/xpressive/xpressive.hpp>

// Point d'entrée du programme serveur principal.
int main (int argc, char ** argv, char ** XIOS_UNUSED (env))
{
   try
   {
      CMPIManager::Initialise(&argc, &argv);
      {
          std::string hello( "1d 12mo 13mi" );

          boost::xpressive::sregex rex = boost::xpressive::sregex::compile( "(\\d+)([ymodhis]+)" );
          
          boost::xpressive::sregex_iterator cur( hello.begin(), hello.end(), rex );
          boost::xpressive::sregex_iterator end;

          for( ; cur != end; ++cur )
          {
             boost::xpressive::smatch const &what = *cur;
              std::cout << what[0] << '\n';
          }
      }
      CMPIManager::Finalize();
   }
   catch (xios::CException & _exception)
   {
      std::cerr << _exception.getMessage() << std::endl;
      return (-1);
   }
   return (0);
}


