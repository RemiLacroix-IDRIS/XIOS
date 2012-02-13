/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
// XMLIOServer headers
#include "onetcdf4.hpp"
#include "onetcdf4_impl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace io {

   // ------------------------------ Constructeurs -----------------------------
   
   CONetCDF4::CONetCDF4(const std::string & filename, bool exist,
                        const MPI_Comm * _comm_server)
   {
      if (_comm_server != NULL) this->comm_server = *_comm_server;
      if (this->comm_server)
      {  
         if (exist)
            CheckError(nc_open_par(filename.c_str(), NC_NETCDF4|NC_MPIIO,
                        this->comm_server.get(), MPI_INFO_NULL, &this->ncidp));
         else
            CheckError(nc_create_par(filename.c_str(), NC_NETCDF4|NC_MPIIO,
                        this->comm_server.get(), MPI_INFO_NULL, &this->ncidp));
      }
      else
      {  
         if (exist)
            CheckError(nc_open(filename.c_str(), NC_NETCDF4, &this->ncidp));
         else
            CheckError(nc_create(filename.c_str(), NC_NETCDF4, &this->ncidp));
      }
   }

   // ------------------------------- Destructeur ------------------------------
   
   CONetCDF4::~CONetCDF4(void)
   {
       CheckError(nc_close(this->ncidp));
   }
   
   // --------------------- Vérification des erreurs NetCDF --------------------
   
   void CONetCDF4::CheckError(int _status) throw (CException)
   {
      if (_status != NC_NOERR)
      {
         std::string errormsg (nc_strerror(_status));           // fuite mémoire ici ?
         XIOS_ERROR_FORMAT("CONetCDF4::CheckError(int status)", // Identifiant d'erreur
                           "[ status = %1% ] %2%",              // Format du message d'erreur
                           %_status % errormsg);                // Variables écrites
      }
   }
   
   // ------------------------- Début/Fin de définition ------------------------
   
   void CONetCDF4::definition_start(void)
   { 
      CheckError(nc_redef(this->ncidp));
   }
      
   void CONetCDF4::definition_end(void)
   { 
      CheckError(nc_enddef(this->ncidp));
   }
   
   // ------------------------------- Accesseurs -------------------------------
   
   const CONetCDF4::CNetCDF4Path & CONetCDF4::getCurrentPath(void) const
   { 
       return (this->path); 
   }
   
   std::string CONetCDF4::getUnlimitedDimensionName(void)
   {
      char full_name_in[NC_MAX_NAME +1];
      int grpid = this->getGroup(path);
      int dimid = this->getUnlimitedDimension();
      
      if (dimid == -1) return (std::string());
      CheckError(nc_inq_dimname(grpid, dimid, full_name_in));

      std::string dimname(full_name_in);
      return (dimname);
   }
 
   std::vector<std::string> CONetCDF4::getDimensionsIdList (const std::string * _varname)
   {
      char full_name_in[NC_MAX_NAME +1];
      int nbdim = 0, *dimid = NULL;
      int grpid = this->getCurrentGroup();
      int varid = (_varname != NULL) ? this->getVariable(*_varname) : NC_GLOBAL;
      std::vector<std::string> retvalue;

      if (_varname != NULL)
      {
         CheckError(nc_inq_varndims(grpid, varid, &nbdim));
         dimid = new int[nbdim]();
         CheckError(nc_inq_vardimid(grpid, varid, dimid));
      }
      else
      {
         CheckError(nc_inq_dimids(grpid, &nbdim, NULL, 1));
         dimid = new int[nbdim]();
         CheckError(nc_inq_dimids(grpid, NULL, dimid, 1));
      }

      for (int i = 0; i < nbdim; i++)
      {
         CheckError(nc_inq_dimname(grpid, dimid[i], full_name_in));
         std::string dimname(full_name_in);
         retvalue.push_back(dimname);
      }
      delete [] dimid;

      return (retvalue);
   }

   // ------------------------------- Mutateurs --------------------------------

   void CONetCDF4::setCurrentPath(const CONetCDF4::CNetCDF4Path & path)
   { 
       this->path = path; 
   }
   
   int CONetCDF4::addDimension(const std::string & _name, std::size_t _size)
   {
      int retvalue = 0;
      int grpid = this->getCurrentGroup();
      if (_size != UNLIMITED_DIM)
         CheckError(nc_def_dim (grpid, _name.c_str(), _size, &retvalue));
      else
         CheckError(nc_def_dim (grpid, _name.c_str(), NC_UNLIMITED, &retvalue));
      return (retvalue);
   }
   
   int CONetCDF4::addVariable (const std::string & _name, nc_type _type,
                               const std::vector<std::string> & _dims)
   {
      int retvalue = 0;
      std::vector<int> dimids;
      int grpid = this->getCurrentGroup();
         
      std::vector<std::string>::const_iterator
         it  = _dims.begin(), end = _dims.end();

      for (;it != end; it++)
      {
         const std::string & dimid = *it;
         dimids.push_back(this->getDimension(dimid));
      }
      CheckError(nc_def_var(grpid, _name.c_str(), _type, dimids.size(), &(dimids[0]), &retvalue));
      return (retvalue);
   }
   
   int CONetCDF4::addGroup(const std::string & _name)
   {
      int retvalue = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_def_grp(grpid, const_cast<char*>(_name.c_str()), &retvalue));
      return (retvalue);
   }
   
   // --------------------------- Accesseurs protégés --------------------------
   
   int CONetCDF4::getCurrentGroup(void)
   {
      return (this->getGroup(this->getCurrentPath()));
   }
   
   int CONetCDF4::getGroup(const CNetCDF4Path & path)
   {
      int retvalue = this->ncidp;
      
      CNetCDF4Path::const_iterator
         it  = path.begin(), end = path.end();

      for (;it != end; it++)
      {
         const std::string & groupid = *it;
         CheckError(nc_inq_ncid(retvalue, const_cast<char*>(groupid.c_str()), &retvalue));
      }
      return (retvalue);
   }
   
   int CONetCDF4::getVariable(const std::string & varname)
   {
      int varid = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_inq_varid (grpid, varname.c_str(), &varid));
      return (varid);
   }
   
   int CONetCDF4::getDimension(const std::string & dimname)
   {
      int dimid = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_inq_dimid (grpid, dimname.c_str(), &dimid));
      return (dimid);
   }
   
   int CONetCDF4::getUnlimitedDimension(void)
   {
      int dimid = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_inq_unlimdim (grpid, &dimid));
      return (dimid);
   }
   
   std::vector<std::size_t> CONetCDF4::getDimensions(const std::string & varname)
   {
      std::size_t size = 0;
      std::vector<std::size_t> retvalue;
      int grpid = this->getCurrentGroup();
      int varid = this->getVariable(varname);
      int nbdim = 0, *dimid = NULL;

      CheckError(nc_inq_varndims(grpid, varid, &nbdim));
      dimid = new int[nbdim]();
      CheckError(nc_inq_vardimid(grpid, varid, dimid));

      for (int i = 0; i < nbdim; i++)
      {
         CheckError(nc_inq_dimlen (grpid, dimid[i], &size));
         if (size == NC_UNLIMITED)
             size = UNLIMITED_DIM;
         retvalue.push_back(size);
      }

      return (retvalue);
   }
   
   bool CONetCDF4::varExist(const std::string & _varname)
   {
      int varid = 0;
      int grpid = this->getCurrentGroup();
      return (nc_inq_varid (grpid, _varname.c_str(), &varid) == NC_NOERR);
   }
   
// ----------------- Obtention des informations d'écriture ------------------
   
   void CONetCDF4::getWriteDataInfos(const std::string & _varname,
                                     std::size_t   _record, 
                                     std::size_t & _array_size,
                                     std::vector<std::size_t> & _sstart,
                                     std::vector<std::size_t> & _scount,
                                     const std::vector<std::size_t> * _start,
                                     const std::vector<std::size_t> * _count)
   {
      std::vector<std::size_t> sizes  = this->getDimensions(_varname);
      std::vector<std::string> iddims = this->getDimensionsIdList (&_varname);   
      std::vector<std::size_t>::const_iterator
         it  = sizes.begin(), end = sizes.end();
      int i = 0;
      
      if (iddims.begin()->compare(this->getUnlimitedDimensionName()) == 0)
      {
         _sstart.push_back(_record);
         _scount.push_back(1); 
         if ((_start == NULL) &&
             (_count == NULL)) i++;
         it++;
      }

      for (;it != end; it++)
      {      
         if ((_start != NULL) && (_count != NULL))
         {
            _sstart.push_back((*_start)[i]);
            _scount.push_back((*_count)[i]);
            _array_size *= (*_count)[i];
            i++;
         }
         else
         {
            _sstart.push_back(0);
            _scount.push_back(sizes[i]);
            _array_size *= sizes[i];
            i++;
         }
      }
   }
   
   // ------------------------- Ecriture des données --------------------------
     
   template <>
      void CONetCDF4::writeData_(int _grpid, int _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                const float * _data)
   {
      CheckError(nc_put_vara_float(_grpid, _varid, &(_sstart[0]), &(_scount[0]), _data));       
   }
   
   template <>
      void CONetCDF4::writeData_(int _grpid, int _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                const int * _data)
   {
      CheckError(nc_put_vara_int(_grpid, _varid, &(_sstart[0]), &(_scount[0]), _data));       
   }
   
   template <>
      void CONetCDF4::writeData_(int _grpid, int _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                const double * _data)
   {
      CheckError(nc_put_vara_double(_grpid, _varid, &(_sstart[0]), &(_scount[0]), _data));       
   }

   // ------------------------- Ecriture des attributs --------------------------

   template <>
      void CONetCDF4::writeAttribute_
         (const std::string & _attname, const double * _value, std::size_t _size, int _grpid, int _varid)
   {       
        CheckError(nc_put_att_double(_grpid, _varid, _attname.c_str(), NC_DOUBLE, _size, _value));
   }
   
   template <>
      void CONetCDF4::writeAttribute_
         (const std::string & _attname, const float * _value, std::size_t _size, int _grpid, int _varid)
   {
       CheckError(nc_put_att_float(_grpid, _varid, _attname.c_str(), NC_FLOAT, _size, _value));
   }
   
   template <>
      void CONetCDF4::writeAttribute_
         (const std::string & _attname, const int * _value, std::size_t _size, int _grpid, int _varid)
   {
       CheckError(nc_put_att_int(_grpid, _varid, _attname.c_str(), NC_INT, _size, _value));
   }
   
   template <>
      void CONetCDF4::writeAttribute_
         (const std::string & _attname, const char * _value, std::size_t _size, int _grpid, int _varid)
   {
       CheckError(nc_put_att_text(_grpid, _varid, _attname.c_str(), _size, _value));
   }
   
   void CONetCDF4::writeAttribute
      (const std::string & _attname, const std::string & _value, const std::string * _varname)
   {
      std::vector<char> chart;
      chart.assign (_value.begin(), _value.end());
      this->writeAttribute(_attname, chart, _varname);
   }
   
} // namespace io
} // namespace xmlioserver
