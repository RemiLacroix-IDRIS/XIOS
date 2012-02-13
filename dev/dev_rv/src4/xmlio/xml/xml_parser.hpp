/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __XML_PARSER_HPP__
#define	__XML_PARSER_HPP__

/**
 * \file    xml_parser.hpp
 * \brief   Définition des méthodes d'initialisation du parsing xml (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    12 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <string>
#include <iostream>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xml_node.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   /// \brief Espace de nommage incluant les classes de manipulation du format xml.
   namespace xml
   {
      /**
       * \class CXMLParser
       * \brief Classe de traitement de document xml depuis différentes sources de données.
       */
      class CXMLParser
      {
         public : // Méthodes statiques

            /*!
             * \brief Parse un document xml à partir d'un nom de fichier.
             * \param _filename : le nom d'un fichier contenant l'arborescence xml.
             * \see ParseString ParseStream
             *
             * Cette méthode utilise ParseStream de la manière suivante :
             * \code
             * //> Parsing d'un flux d'entrée de fichier.
             * std::ifstream ifs ( _filename.c_str() , std::ifstream::in );
             * CXMLParser::ParseStream(ifs);
             * \endcode
             */
            static void ParseFile  (const std::string & _filename);

            /*!
             * \brief Parse un document xml à partir d'une chaîne de caractères.
             * \param _xmlContent : une chaîne de caractères contenant l'arborescence xml.
             * \see ParseFile ParseStream
             * 
             * Cette méthode utilise ParseStream de la manière suivante :
             * \code
             * //> Parsing d'un flux d'entrée de chaîne de caractères.
             * StdIStringStream iss(_xmlContent);
             * CXMLParser::ParseStream(iss);
             * \endcode
             */
            static void ParseString(const std::string & _xmlContent);

            /*!
             * \brief Parse un document xml à partir d'un flux d'entrée de données.
             * \param _stream : le flux d'entrée contenant l'arborescence xml.
             * \see ParseString ParseFile
             */
            static void ParseStream(std::istream & _stream);

      }; //class CXMLParser

   }// namespace xml
} // namespace xmlioserver

#endif // __XML_PARSER_HPP__

