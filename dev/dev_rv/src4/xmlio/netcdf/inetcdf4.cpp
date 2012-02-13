/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    inetcdf4.hpp
 * \brief   Lecture des fichiers de données au format netCDF-4 (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    15 Juin 2011
 */

// XMLIOServer headers
#include "inetcdf4.hpp"
#include "inetcdf4_impl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace io {
    
    
   // ------------------------------ Constructeurs -----------------------------
   
   CINetCDF4::CINetCDF4(const std::string & filename)
   {
      CheckError(nc_open(filename.c_str(), NC_NOWRITE, &this->ncidp));
   }

   // ------------------------------- Destructeur ------------------------------
   
   CINetCDF4::~CINetCDF4(void)
   {
       CheckError(nc_close(this->ncidp));
   }
   
   // --------------------- Vérification des erreurs NetCDF --------------------
   
   void CINetCDF4::CheckError(int _status) throw (CException)
   {
      if (_status != NC_NOERR)
      {
         std::string errormsg (nc_strerror(_status)); // fuite mémoire ici ?
         XIOS_ERROR("CONetCDF4::CheckError(int status)",
              << "[ status = " << _status << " ] " << errormsg);
      }
   }
   
   // --------------------------- Accesseurs protégés --------------------------
   
   int CINetCDF4::getCurrentGroup(void)
   {
      return (this->getGroup(this->getCurrentPath()));
   }
   
   int CINetCDF4::getGroup(const CNetCDF4Path & path)
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
   
   int CINetCDF4::getVariable(const std::string & varname)
   {
      int varid = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_inq_varid (grpid, varname.c_str(), &varid));
      return (varid);
   }
   
   int CINetCDF4::getDimension(const std::string & dimname)
   {
      int dimid = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_inq_dimid (grpid, dimname.c_str(), &dimid));
      return (dimid);
   }
   
   int CINetCDF4::getUnlimitedDimension(void)
   {
      int dimid = 0;
      int grpid = this->getCurrentGroup();
      CheckError(nc_inq_unlimdim (grpid, &dimid));
      return (dimid);
   }
   
   std::vector<std::size_t> CINetCDF4::getDimensions(const std::string & varname)
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
   
   bool CINetCDF4::varExist(const std::string & _varname)
   {
      int varid = 0;
      int grpid = this->getCurrentGroup();
      return (nc_inq_varid (grpid, _varname.c_str(), &varid) == NC_NOERR);
   }
   
   // ------------------------------- Accesseurs -------------------------------
   
   const CINetCDF4::CNetCDF4Path & CINetCDF4::getCurrentPath(void) const
   { 
       return (this->path); 
   }

   // ------------------------------- Mutateurs --------------------------------

   void CINetCDF4::setCurrentPath(const CNetCDF4Path & path)
   { 
       this->path = path; 
   }
   
   // ----------------- Obtention des informations de lecture ------------------
   
   void CINetCDF4::getReadDataInfos(const std::string & _varname,
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
   
   // ------------------------- Lecture des données --------------------------
   
   template <>
      void CINetCDF4::readData_(int _grpid, int _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                float * _data)
   {
      CheckError(nc_get_vara_float(_grpid, _varid, &(_sstart[0]), &(_scount[0]), _data));       
   }
   
   template <>
      void CINetCDF4::readData_(int _grpid, int _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                int * _data)
   {
      CheckError(nc_get_vara_int(_grpid, _varid, &(_sstart[0]), &(_scount[0]), _data));       
   }
   
   template <>
      void CINetCDF4::readData_(int _grpid, int _varid,
                                const std::vector<std::size_t> & _sstart,
                                const std::vector<std::size_t> & _scount,
                                double * _data)
   {
      CheckError(nc_get_vara_double(_grpid, _varid, &(_sstart[0]), &(_scount[0]), _data));       
   }

   // ------------------------- Lecture des attributs --------------------------
   void CINetCDF4::readAttribute
      (const std::string & _attname, std::string & _value, const std::string * _varname)
   {
        std::vector<char> chart;
        this->readAttribute(_attname,chart , _varname);
        _value.assign(&(chart[0]), chart.size());
   }
   
   template <>
      void CINetCDF4::readAttribute_
         (const std::string & _attname, double * _value, int _grpid, int _varid)
   {       
        CheckError(nc_get_att_double(_grpid, _varid, _attname.c_str(), _value));
   }
   
   template <>
      void CINetCDF4::readAttribute_
         (const std::string & _attname, float * _value, int _grpid, int _varid)
   {
       CheckError(nc_get_att_float(_grpid, _varid, _attname.c_str(), _value));
   }
   
   template <>
      void CINetCDF4::readAttribute_
         (const std::string & _attname, int * _value, int _grpid, int _varid)
   {
       CheckError(nc_get_att_int(_grpid, _varid, _attname.c_str(), _value));
   }
   
   template <>
      void CINetCDF4::readAttribute_
         (const std::string & _attname, char * _value, int _grpid, int _varid)
   {
       CheckError(nc_get_att_text(_grpid, _varid, _attname.c_str(), _value));
   }
   
   // ----------------- Accesseur de parcours (d'exploration) ------------------

   std::vector<std::string> CINetCDF4::getAttributes(const std::string * const _varname)
   {
      int nbatt = 0;
      char full_name_in[NC_MAX_NAME +1];
      std::vector<std::string> retvalue;
      int grpid = this->getCurrentGroup();
      int varid = (_varname != NULL) ? this->getVariable(*_varname) : NC_GLOBAL;

      if (_varname != NULL)
         CheckError(nc_inq_varnatts (grpid, varid, &nbatt));
      else
         CheckError(nc_inq_natts(grpid, &nbatt));

      for (int i = 0; i < nbatt; i++)
      {
         CheckError(nc_inq_attname(grpid, varid, i, full_name_in));
         std::string attname(full_name_in);
         retvalue.push_back(attname);
      }
      return (retvalue);
   }   
   
   std::map<std::string, std::size_t> CINetCDF4::getDimensions(const std::string * const _varname)
   {
      std::size_t size = 0;
      char full_name_in[NC_MAX_NAME +1];
      int nbdim = 0, *dimid = NULL;
      int grpid = this->getCurrentGroup();
      int varid = (_varname != NULL) ? this->getVariable(*_varname) : NC_GLOBAL;
      std::map<std::string, std::size_t> retvalue;

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
         CheckError(nc_inq_dimlen (grpid, dimid[i], &size));

         std::string dimname(full_name_in);
         retvalue.insert(retvalue.end(), std::make_pair(dimname, size));
      }
      delete [] dimid;

      return (retvalue);
   }
   
   std::vector<std::string> CINetCDF4::getGroups(void)
   {
      std::size_t strlen = 0;
      char full_name_in[NC_MAX_NAME +1];
      int nbgroup = 0, *groupid = NULL;
      int grpid = this->getCurrentGroup();
      std::vector<std::string> retvalue;

      CheckError(nc_inq_grps(grpid, &nbgroup, NULL));
      groupid = new int[nbgroup]();
      CheckError(nc_inq_grps(grpid, NULL, groupid));

      for (int i = 0; i < nbgroup; i++)
      {
         CheckError(nc_inq_grpname_full(groupid[i], &strlen, full_name_in));
         std::string groupname(full_name_in, strlen);
         retvalue.push_back(groupname);
      }

      delete [] groupid;
      return (retvalue);
   }
   
   std::vector<std::string> CINetCDF4::getVariables(void)
   {
      char full_name_in[NC_MAX_NAME +1];
      int nbvar = 0, *varid = NULL;
      int grpid = this->getCurrentGroup();
      std::vector<std::string> retvalue;

      CheckError(nc_inq_varids(grpid, &nbvar, NULL));
      varid = new int[nbvar]();
      CheckError(nc_inq_varids(grpid, NULL, varid));

      for (int i = 0; i < nbvar; i++)
      {
         CheckError(nc_inq_varname(grpid, varid[i], full_name_in));
         std::string varname(full_name_in);
         retvalue.push_back(varname);
      }

      delete [] varid;
      return (retvalue);
   }
   
   std::size_t CINetCDF4::getNbOfTimestep(void)
   {
      int dimid = this->getUnlimitedDimension();
      if (dimid == -1) return (1);
      return (this->getDimensions(NULL)[this->getUnlimitedDimensionName()]);
   }
            
   std::string CINetCDF4::getUnlimitedDimensionName(void)
   {
      char full_name_in[NC_MAX_NAME +1];
      int grpid = this->getGroup(path);
      int dimid = this->getUnlimitedDimension();
      
      if (dimid == -1) return (std::string());
      CheckError(nc_inq_dimname(grpid, dimid, full_name_in));

      std::string dimname(full_name_in);
      return (dimname);
   }
 
   std::vector<std::string> CINetCDF4::getDimensionsIdList (const std::string * _varname)
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
   
} // namespace io
} // namespace xmlioserver
