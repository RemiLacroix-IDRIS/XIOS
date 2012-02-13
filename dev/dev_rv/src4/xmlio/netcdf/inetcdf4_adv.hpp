/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __INETCDF4_ADV_HPP__
#define	__INETCDF4_ADV_HPP__

/**
 * \file    inetcdf4_adv.hpp
 * \brief   Lecture des fichiers de données au format netCDF-4, fonctionnalités avancées (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    15 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#  include <set>
#  include <algorithm>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "inetcdf4.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   namespace io
   {
      class CINetCDF4Adv : public CINetCDF4
      {
            typedef CINetCDF4 SuperClass; /*!< Redéfinition de CINetCDF4. */
            
         public : // Constructeurs
         
            //! \brief Constructeur simple.
            CINetCDF4Adv(void); // NIY
            
            /*!
             * \brief Constructeur à partir d'un nom de fichier au format netCDF4.
             * \param _filename : un nom de fichier au format netCDF4.
             */
            explicit CINetCDF4Adv(const std::string & _filename);
            
            /*!
             * \brief Constructeur par copie .
             * \param _inetcdf4 : référence de l'objet à copier.
             */
            CINetCDF4Adv(const CINetCDF4Adv & _inetcdfadv);// NIY
            
            /*!
             * \brief Constructeur par copie.
             * \param _inetcdf4_ptr : le pointeur sur l'objet à copier.
             */
            CINetCDF4Adv(const CINetCDF4Adv * const _inetcdfadv); // NIY
            
         public : // Destructeur
         
            //! \brief Destructeur de l'objet (le fichier est clos lors de son appel).
            virtual ~CINetCDF4Adv(void);
            
         public : // Divers accesseurs

            std::string getLonCoordName (const std::string & _varname);
            std::string getLatCoordName (const std::string & _varname);
            std::string getVertCoordName(const std::string & _varname);
            std::string getTimeCoordName(const std::string & _varname);
             
            std::set<std::string> getCoordVariables(void);
            std::set<std::string> getBoundVariables(void);
            
            std::string getCoordinatesId(const std::string & _varname);
            std::string getBoundsId     (const std::string & _varname);
            
            template <class DataType>
               void getMissingValue(DataType & _value, const std::string & _varname);
            
            std::vector<std::string> getCoordinatesIdList(const std::string & _varname);
            
            std::vector<std::string> getDataVariables(bool _is3D       = true,
                                                      bool _isRecti    = true,
                                                      bool _isCurvi    = true,
                                                      bool _isUnstr    = true,
                                                      bool _isCellData = true,
                                                      bool _isTemporal = true);
            

            std::size_t getNbVertex(const std::string & _varname);
            
         public : // Divers mutateurs
             
             template <class DataType, std::size_t ndim>
                 bool replaceMissingValue(const std::string & _varname,
                                          boost::multi_array<DataType, ndim> & _data,
                                          const DataType & newValue);
            
         public : // Divers méthodes de tests           

            bool hasAttribute(const std::string & name,
                              const std::string * const _varname  = NULL);

            bool hasVariable    (const std::string & _varname);
            bool hasMissingValue(const std::string & _varname);
            bool hasCoordinates (const std::string & _varname);
            bool hasBounds      (const std::string & _varname);

            bool hasTemporalDim(void);

            bool isBound       (const std::string & _varname);
            bool isCoordinate  (const std::string & _varname);
            bool isRectilinear (const std::string & _varname);
            bool isCurvilinear (const std::string & _varname);
            bool isUnknown     (const std::string & _varname);
            bool isUnstructured(const std::string & _varname);
            bool isTemporal    (const std::string & _varname);
            bool is3Dim        (const std::string & _varname);
            bool isCellGrid    (const std::string & _varname);            
            
      }; // class CINetCDF4Adv
   } // namespace io
} // namespace xmlioserver

#endif	// __INETCDF4_ADV_HPP__ 

