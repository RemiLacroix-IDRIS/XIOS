/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __ONETCDF4_HPP__
#define __ONETCDF4_HPP__

#ifndef __XIOS_NO_EXTERN

// M(essage) P(assing) I(nterface) headers
#include <mpi.h>
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
// NetCDF headers
#define MPI_INCLUDED
#include <netcdf.h>
extern "C" 
{
#include <netcdf_par.h>
}

// C++ standard headers
#include <vector>

// boost Headers
#include <boost/multi_array.hpp>
#include <boost/optional.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

#ifndef UNLIMITED_DIM
   #define UNLIMITED_DIM (size_t)(-1)
#endif  //UNLIMITED_DIM

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   namespace io
   {
      /**
       * \class CONetCDF4
       * \brief Classe permettant d'écrire des fichiers de données au format netCDF-4.
       */
      class CONetCDF4
      {
         public : // Définition de type
            
            //! \brief Redéfinition de std::vector<std::string> pour simplification d'écriture.
            typedef std::vector<std::string> CNetCDF4Path;
            
         public : // Constructeurs
         
            //! \brief Constructeur simple.
            CONetCDF4(void); // NIY
            
            /*!
             * \brief Constructeur à partir d'un nom de fichier au format netCDF4.
             * \param _filename : un nom de fichier au format netCDF4.
             */
            explicit CONetCDF4(const std::string & _filename, bool exist = false,
                               const MPI_Comm * _comm_server = NULL);
            
            /*!
             * \brief Constructeur par copie .
             * \param _onetcdf4 : référence de l'objet à copier.
             */
            CONetCDF4(const CONetCDF4 & _onetcdf4);// NIY
            
            /*!
             * \brief Constructeur par copie.
             * \param _onetcdf4_ptr : le pointeur sur l'objet à copier.
             */
            CONetCDF4(const CONetCDF4 * const _onetcdf4_ptr); // NIY
            
         public : // Destructeur
         
            //! \brief Destructeur de l'objet (le fichier est clos lors de son appel).
            virtual ~CONetCDF4(void);
            
         public : // Mutateur
            
            void setCurrentPath(const CNetCDF4Path & _path);

            int addDimension(const std::string & _name, std::size_t _size = UNLIMITED_DIM);
            int addVariable (const std::string & _name, nc_type _type,
                             const std::vector<std::string> & _dims);            
            int addGroup    (const std::string & _name);
            
         public : // Accesseurs
            
            const CNetCDF4Path & getCurrentPath(void) const;
            
            std::string getUnlimitedDimensionName(void);          
            std::vector<std::string> getDimensionsIdList (const std::string * _varname);
            
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
            
         public : // Ecriture
         
            template <class DataType>
               void writeAttribute
                  (const std::string &           _attname,
                   const std::vector<DataType> & _value,
                   const std::string           * _varname = NULL);
            
           void writeAttribute
                  (const std::string & _attname,
                   const std::string & _value,
                   const std::string * _varname = NULL);

            template <class DataType, std::size_t ndim>
               void writeData( const boost::multi_array<DataType, ndim> & _data,
                               const std::string                        & _varname,
                               std::size_t                                _record     = 0,
                               bool                                       _collective = true,
                               const std::vector<std::size_t>           * _start      = NULL,
                               const std::vector<std::size_t>           * _count      = NULL);
                               
         private : // Ecriture privée
             
            template <class DataType>
               void writeData_(int                               _grpid,
                                int                              _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                const DataType                 * _data);
             
            template <class DataType>
                void writeAttribute_
                        (const std::string & _attname,
                         const DataType    * _value,
                         std::size_t         _size,
                         int                 _grpid,
                         int                 _varid = NC_GLOBAL);
                         
         private : // Obtention des informations d'écriture
            
            void getWriteDataInfos(const std::string              & _varname,
                                   std::size_t                      _record, 
                                   std::size_t                    & _array_size,
                                   std::vector<std::size_t>       & _sstart,
                                   std::vector<std::size_t>       & _scount,
                                   const std::vector<std::size_t> * _start,
                                   const std::vector<std::size_t> * _count);
            
         public : // Début/Fin de définition

            void definition_start(void);
            void definition_end(void);
            
         private : // Vérification des erreurs NetCDF

            static void CheckError(int _status) throw (CException);
            
         public : // Propriétés privées
         
            boost::optional<MPI_Comm> comm_server;
            std::vector<std::string> path;
            int ncidp;                            
         
      }; // class CONetCDF4

   } // namespace io

} // namespace xmlioserver

#endif// __ONETCDF4_HPP__
