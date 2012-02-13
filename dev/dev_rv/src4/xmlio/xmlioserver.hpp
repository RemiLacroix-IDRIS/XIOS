/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __XMLIO__
#define __XMLIO__

/**
 * \dir xmlio
 * \brief Contient toutes les sources du projet.
 */

/**
 * \file    xmlioserver.hpp
 * \brief   Entête unique du projet pour l'interface et le point d'entrée du serveur.
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

/*!
 * \brief Point d'entrée du programme serveur principal..
 * \param argc : nombre d'arguments passés en ligne de commande.
 * \param argv : liste des arguments passés en ligne de commande.
 * \param env  : variables d'environnement.
 * \return code d'erreur du programme.
 */
int main(int argc, char ** argv, char ** env);

#endif //__XMLIO__
