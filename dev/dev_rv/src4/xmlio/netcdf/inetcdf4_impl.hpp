/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __INETCDF4_IMPL_HPP__
#define __INETCDF4_IMPL_HPP__

/**
 * \file    inetcdf4_impl.hpp
 * \brief   Lecture des fichiers de données au format netCDF-4 (implémentation template).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    15 Juin 2011
 */

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver{
namespace io {
   
   template <class DataType, std::size_t ndim>
      void CINetCDF4::readData(boost::multi_array<DataType, ndim> & _data,
                               const std::string & _varname,
                               std::size_t _record,
                               const std::vector<std::size_t> * _start,
                               const std::vector<std::size_t> * _count)
   {
      int grpid = this->getCurrentGroup();
      int varid = this->getVariable(_varname);
      std::size_t _array_size = 1;
      std::vector<std::size_t> _sstart, _scount;
      this->getReadDataInfos
         (_varname, _record, _array_size,  _sstart, _scount, _start, _count);
      
      _data.resize(boost::extents[_array_size]);
      this->readData_(grpid, varid, _sstart, _scount, _data.data());
   }

   template <class DataType>
      void CINetCDF4::readAttribute
          (const std::string & _attname, std::vector<DataType> & _value, const std::string * _varname)
   {
       std::pair<nc_type, std::size_t> attinfos;
       int grpid = this->getCurrentGroup();                                 
       int varid = (_varname != NULL) ? this->getVariable(*_varname) : NC_GLOBAL;
       CheckError(nc_inq_att(grpid, varid, _attname.c_str(), &attinfos.first, &attinfos.second));
       _value.resize(attinfos.second); 
       this->readAttribute_(_attname, &_value[0], grpid, varid);
       
   }
   
} // namespace io
} // namespace xmlioserver

#endif	// __INETCDF4_IMPL_HPP__