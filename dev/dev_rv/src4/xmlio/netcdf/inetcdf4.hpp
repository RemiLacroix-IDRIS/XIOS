/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __INETCDF4_HPP__
#define __INETCDF4_HPP__

/**
 * \file    inetcdf4.hpp
 * \brief   Lecture des fichiers de données au format netCDF-4 (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    15 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// M(essage) P(assing) I(nterface) headers
#include <mpi.h>
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
// NetCDF headers
#define MPI_INCLUDED
#include <netcdf.h>

// C++ standard headers
#include <vector>

// boost Headers
#include <boost/multi_array.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

#ifndef UNLIMITED_DIM
   #define UNLIMITED_DIM (size_t)(-1)
#endif  //UNLIMITED_DIM

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   /// \brief Espace de nommage permettant de traiter les Entrées/Sorties sous divers formats.
   namespace io
   {
      /**
       * \class CINetCDF4
       * \brief Classe permettant de lire les fichiers de données au format netCDF-4.
       */
      class CINetCDF4
      {
         public : // Définition de type
            
            //! \brief Redéfinition de std::vector<std::string> pour simplification d'écriture.
            typedef std::vector<std::string> CNetCDF4Path;
            
         public : // Constructeurs
         
            //! \brief Constructeur simple.
            CINetCDF4(void); // NIY
            
            /*!
             * \brief Constructeur à partir d'un nom de fichier au format netCDF4.
             * \param _filename : un nom de fichier au format netCDF4.
             */
            explicit CINetCDF4(const std::string & _filename);
            
            /*!
             * \brief Constructeur par copie .
             * \param _inetcdf4 : référence de l'objet à copier.
             */
            CINetCDF4(const CINetCDF4 & _inetcdf4);// NIY
            
            /*!
             * \brief Constructeur par copie.
             * \param _inetcdf4_ptr : le pointeur sur l'objet à copier.
             */
            CINetCDF4(const CINetCDF4 * const _inetcdf4_ptr); // NIY
            
         public : // Destructeur
         
            //! \brief Destructeur de l'objet (le fichier est clos lors de son appel).
            virtual ~CINetCDF4(void);
            
            
                
         public : // Accesseur de parcours (d'exploration)
             
            std::size_t getNbOfTimestep(void);
            
            std::string getUnlimitedDimensionName(void);
             
            std::map<std::string, std::size_t> 
               getDimensions(const std::string * const _varname  = NULL); 
            
            std::vector<std::string> getDimensionsIdList (const std::string * _varname);
            
            std::vector<std::string> getAttributes(const std::string * const _varname  = NULL);            
            std::vector<std::string> getGroups   (void);
            std::vector<std::string> getVariables(void);
            
         public : // Mutateur
            
            void setCurrentPath(const CNetCDF4Path & _path);
            
         public : // Accesseurs
            
            const CNetCDF4Path & getCurrentPath(void) const;
            
         public : // Lecture
            
            template <class DataType>
               void readAttribute
                  (const std::string &     _attname,
                   std::vector<DataType> & _value,
                   const std::string *     _varname = NULL);
            
           void readAttribute
                  (const std::string & _attname,
                   std::string       & _value,
                   const std::string * _varname = NULL);

            template <class DataType, std::size_t ndim>
               void readData( boost::multi_array<DataType, ndim> & _data,
                              const std::string                  & _varname,
                              std::size_t                          _record = 0,
                              const std::vector<std::size_t>     * _start  = NULL,
                              const std::vector<std::size_t>     * _count  = NULL);

         protected : // Accesseurs protégés
         
            /*!
             * \brief Retourne l'identifiant du groupe courant 
             * (le chemin est gardée en mémoire dans la variable path).
             * \return  l'identifiant du groupe courant
             * \see path
             */
            int getCurrentGroup(void);
            
            /*!
             * \brief Retourne l'identifiant du groupe  dont le chemin est passé en argument.
             * \param _path : Un chemin dans la structure de donnée netCDF.
             * \return l'identifiant du groupe.
             *
             * <i> Exemple d'utilisation: </i>
             * \code
             * //> Retourne l'identifiant d'un group.
             * CINetCDF4 ncinput(std::string("mon_fichier.nc"));
             * CNetCDF4Path path;
             * path.push_back("mes_donnees"); path.push_back("mes_tableaux");
             * int groupid = ncinput.getGroup(path);
             * \endcode
             */
            int getGroup(const CNetCDF4Path & _path);
            
            /*!
             * \brief Retourne l'identifiant d'une variable dans le groupe courant.
             * \param _varname : Le nom d'une variable dans le fichier de données.
             * \return l'identifiant de la variable.
             */
            int getVariable(const std::string & _varname);
            
            /*!
             * \brief Retourne l'identifiant d'une dimension dans le groupe courant.
             * \param _dimname : Le nom d'une dimension dans le  fichier de données.
             * \return l'identifiant de la dimension.
             */
            int getDimension(const std::string & _dimname);
            
            /*!
             * \brief Retourne les dimensions d'une variable.
             * \param _varname : Le nom d'une variable dans le  fichier de données.
             * \return Un vecteur des dimensions associées à la variables
             */
            std::vector<std::size_t> getDimensions(const std::string & _varname);
            
            /*!
             * \brief Retourne l'identifiant de la dimension non limité dans le fichier.
             * \return l'identifiant de la dimension non limité ou <b>-1</b> s'il n'y en a pas.
             */
            int getUnlimitedDimension(void);
            
            /*!
             * \brief Indique si une variable existe dans le groupe courant.
             * \param _varname : Le nom d'une variable dans le  fichier de données.
             * \return  <b>true</b> si la variable existe, <b>false</b> sinon.
             */
            bool varExist(const std::string & _varname);

         private : // Obtention des informations de lecture
            
            void getReadDataInfos(const std::string              & _varname,
                                  std::size_t                      _record, 
                                  std::size_t                    & _array_size,
                                  std::vector<std::size_t>       & _sstart,
                                  std::vector<std::size_t>       & _scount,
                                  const std::vector<std::size_t> * _start,
                                  const std::vector<std::size_t> * _count);
            
         private : // Lecture privée
             
             template <class DataType>
                void readData_(int                              _grpid,
                               int                              _varid,
                               const std::vector<std::size_t> & _sstart,
                               const std::vector<std::size_t> & _scount,
                               DataType                       * _data);
             
             template <class DataType>
                void readAttribute_
                        (const std::string & _attname,
                         DataType          * _value,
                         int                 _grpid,
                         int                 _varid = NC_GLOBAL);
             
         private : // Vérification des erreurs NetCDF

            /*!
             * \brief Vérifie le code d'erreur retourné lors de l'appel d'une fonction
             * dans la bibliothèque netCDF afin de détecter les erreurs.
             * \param _status : le code d'erreur retourné par l'appel d'une fonction netCDF.
             * \remarks Tous les appels de fonction dans la bibliothèque netCDF doivent être
             * pris en charge par cette méthode statique.
             * 
             * <i> Exemple d'utilisation: </i>
             * \code
             * //> Retourne l'identifiant d'une variable netCDF dont le nom est 'varname' dans le groupe grpid 
             * CheckError(nc_inq_varid (grpid, varname, varid));
             * \endcode
             */
            static void CheckError(int _status) throw (CException);

         public : // Propriétés privées
         
            std::vector<std::string> path; /*!< \brief Chemin vers le groupe de données en cours de lecture. */
            int ncidp;                     /*!< \brief Identifiant de fichiers netCDF. */
         
      }; // class CINetCDF4
   } // namespace io

} // namespace xmlioserver

#endif	// __INETCDF4_HPP__
