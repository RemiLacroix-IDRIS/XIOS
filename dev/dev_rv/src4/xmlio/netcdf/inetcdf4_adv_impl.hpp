/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __INETCDF4_ADV_IMPL_H__
#define	__INETCDF4_ADV_IMPL_H__

/**
 * \file    inetcdf4_adv.hpp
 * \brief   Lecture des fichiers de données au format netCDF-4, fonctionnalités avancées (implémentation template).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    15 Juin 2011
 */

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace io {
    
   template <class DataType>
      void CINetCDF4Adv::getMissingValue(DataType & _value, const std::string & _varname)
   {
      std::vector<DataType> value;
      if (this->hasAttribute("missing_value", &_varname))
      {
         this->readAttribute("missing_value", value, &_varname);
         _value = value[0];
      }
      if (this->hasAttribute("_FillValue", &_varname))
      {
         this->readAttribute("_FillValue", value, &_varname);
         _value = value[0];
      }
   }
   
   template <class DataType, std::size_t ndim>
      bool CINetCDF4Adv::replaceMissingValue(const std::string & _varname,
                                             boost::multi_array<DataType, ndim> & _data,
                                             const DataType & newValue)
   {
       if (this->hasMissingValue(_varname))
       {
           DataType mvalue;
           this->CINetCDF4Adv::getMissingValue(mvalue, _varname);
           std::replace(_data.begin(), _data.end(), mvalue, newValue);
           return (true);
       }
       return (false);
   }
    
} // namespace io
} // namespace xmlioserver

#endif	// __INETCDF4_ADV_IMPL_H__

