/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __ONETCDF4_IMPL_HPP__
#define __ONETCDF4_IMPL_HPP__

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver{
namespace io {
   
   template <class DataType, std::size_t ndim>
      void CONetCDF4::writeData(const boost::multi_array<DataType, ndim> & _data,
                                const std::string                        & _varname,
                                std::size_t                                _record,
                                bool                                       _collective,
                                const std::vector<std::size_t>           * _start,
                                const std::vector<std::size_t>           * _count)
   {
      int grpid = this->getCurrentGroup();
      int varid = this->getVariable(_varname);
      std::size_t _array_size = 1;
      std::vector<std::size_t> _sstart, _scount;
      
      if (this->comm_server && _collective)
         CheckError(nc_var_par_access(grpid, varid, NC_COLLECTIVE));
      if (this->comm_server && !_collective)
         CheckError(nc_var_par_access(grpid, varid, NC_INDEPENDENT));
      
      this->getWriteDataInfos
         (_varname, _record, _array_size,  _sstart, _scount, _start, _count);
      this->writeData_(grpid, varid, _sstart, _scount, _data.data());
   }

   template <class DataType>
      void CONetCDF4::writeAttribute
          (const std::string           & _attname,
           const std::vector<DataType> & _value,
           const std::string           * _varname)
   {
       std::pair<nc_type, std::size_t> attinfos;
       int grpid = this->getCurrentGroup();                                 
       int varid = (_varname != NULL) ? this->getVariable(*_varname) : NC_GLOBAL;
       this->writeAttribute_(_attname, &_value[0], _value.size(), grpid, varid);       
   }
   
} // namespace io
} // namespace xmlioserver

#endif	// __ONETCDF4_IMPL_HPP__
