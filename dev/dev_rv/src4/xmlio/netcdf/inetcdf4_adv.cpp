/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    inetcdf4_adv.hpp
 * \brief   Lecture des fichiers de données au format netCDF-4, fonctionnalités avancées (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    15 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// Boost headers
#include <boost/algorithm/string.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "inetcdf4_adv.hpp"
#include "inetcdf4_adv_impl.hpp"


// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace io {
    
   CINetCDF4Adv::CINetCDF4Adv(const std::string & filename)
        : CINetCDF4(filename)
   { /* Ne rien faire de plus */ }

   CINetCDF4Adv::~CINetCDF4Adv(void)
   { /* Ne rien faire de plus */ }
   
   bool CINetCDF4Adv::hasAttribute(const std::string & name,
                                   const std::string * const _varname)
   {
      std::vector<std::string > atts = this->getAttributes(_varname);
      std::vector<std::string >::const_iterator it = atts.begin(), end = atts.end();
      for (; it != end; it++)
      {
         const std::string  & attname = *it;
         if (attname.compare(name) == 0) return (true);
      }
      return (false);
   }
   
   bool CINetCDF4Adv::hasVariable    (const std::string & _varname)
   {
       return (SuperClass::varExist(_varname));
   }
   
   bool CINetCDF4Adv::hasMissingValue(const std::string & _varname)
   {
      return (this->hasAttribute("missing_value", &_varname) ||
              this->hasAttribute("_FillValue", &_varname));
   }
   
   bool CINetCDF4Adv::hasCoordinates (const std::string & _varname)
   {
      return (this->hasAttribute("coordinates", &_varname));
   }
   
   bool CINetCDF4Adv::hasBounds      (const std::string & _varname)
   {
      return (this->hasAttribute("bounds", &_varname));
   }

   bool CINetCDF4Adv::hasTemporalDim(void)
   {
      return (this->getUnlimitedDimension() != -1);
   }

   bool CINetCDF4Adv::isBound(const std::string & _varname)
   {
      std::set<std::string> bounds = this->getBoundVariables();
      return (bounds.find(_varname) != bounds.end());
   }
   
   bool CINetCDF4Adv::isCoordinate(const std::string & _varname)
   {
      std::set<std::string> coords = this->getCoordVariables();
      return (coords.find(_varname) != coords.end());
   }
   
   bool CINetCDF4Adv::isRectilinear(const std::string & _varname)
   {
      std::vector<std::string> coords = this->getCoordinatesIdList(_varname);
      std::vector<std::string>::const_iterator it = coords.begin(), end = coords.end();
      for (; it != end; it++)
      {
         const std::string & coord = *it;
         if (this->hasVariable(coord) && !this->isTemporal(coord))
         {
            std::map<std::string, std::size_t> dimvar = this->getDimensions(&coord);
            if ((dimvar.size() == 1) && (dimvar.find(coord) != dimvar.end()))
               continue;
            else
               return (false);
         }
      }
      return (true);
   }
   
   bool CINetCDF4Adv::isCurvilinear(const std::string & _varname)
   {
      if (this->isRectilinear(_varname) ||
         !this->hasCoordinates(_varname))
         return (false);

      std::vector<std::string> coords = this->getCoordinatesIdList(_varname);
      std::vector<std::string>::const_iterator it = coords.begin(), end = coords.end();
      for (; it != end; it++)
      {
         const std::string & coord = *it;
         if (this->hasVariable(coord))
         {
            std::map<std::string, std::size_t> dimvar = this->getDimensions(&coord);
            if (dimvar.size() != 2) return (false);
         }
         else return (false);
      }
      return (true);
   }
   
   bool CINetCDF4Adv::isUnknown(const std::string & _varname)
   {
      return !(this->isRectilinear(_varname) ||
               this->isCurvilinear(_varname) ||
               this->isUnstructured(_varname));
   }
   
   bool CINetCDF4Adv::isUnstructured(const std::string & _varname)
   {
      if (this->isRectilinear(_varname)    ||
          this->isCurvilinear(_varname)    ||
         !this->hasCoordinates(_varname))
          return (false);

      std::string dimname = this->getDimensionsIdList(&_varname).back();

      std::vector<std::string> coords = this->getCoordinatesIdList(_varname);
      std::vector<std::string>::const_iterator it = coords.begin(), end = coords.end();
      for (; it != end; it++)
      {
         const std::string & coord = *it;
         if (this->hasVariable(coord))
         {
            std::map<std::string, std::size_t> dimvar = this->getDimensions(&coord);
            if ((dimvar.size() == 1) &&
                (dimvar.find(dimname) != dimvar.end()))
               continue;
            else
               return (false);
         }
            else return (false);
      }

      return (true);
   }
   
   bool CINetCDF4Adv::isTemporal(const std::string & _varname)
   {
      if (!this->hasTemporalDim()) return (false);
      std::map<std::string, std::size_t> dims = this->getDimensions(&_varname);
      if (dims.find(this->getUnlimitedDimensionName()) != dims.end())
         return (true);
      return (false);
   }
   
   bool CINetCDF4Adv::is3Dim(const std::string & _varname)
   {
      int i = 0;
      std::vector<std::string> coords = this->getCoordinatesIdList(_varname);
      std::vector<std::string>::const_iterator it = coords.begin(), end = coords.end();
      for (; it != end; it++)
      {
         const std::string & coord = *it;
         if (this->hasVariable(coord))
         {
            if (this->isTemporal(coord))
               continue;
            i++;
         }
         else
         {
            if (coord.compare(this->getUnlimitedDimensionName()) == 0)
               continue;
            i++;
         }
      }
      return (i == 3);
   }
   
   bool CINetCDF4Adv::isCellGrid(const std::string & _varname)
   {
      if (this->isCoordinate(_varname))
      {
         return (this->hasBounds(_varname));
      }
      else
      {
         std::vector<std::string> coords = this->getCoordinatesIdList(_varname);
         std::vector<std::string>::const_iterator it = coords.begin(), end = coords.end();
         for (; it != end; it++)
         {
            const std::string & coord = *it;
            if (this->hasVariable(coord))
            {
               if (this->isTemporal(coord))
                  continue;
               if (this->isCellGrid(coord))
                  continue;
               return (false);
            }
            else
            {
               if (coord.compare(this->getUnlimitedDimensionName()) == 0)
                  continue;
               return (false);
            }
         }
      }

      return (true);
   }
   
   std::size_t CINetCDF4Adv::getNbVertex(const std::string & _varname)
   {
      if (this->isRectilinear(_varname) ||
          this->isCurvilinear(_varname))
      {
         if (this->is3Dim(_varname)) return (8);
         else return (4);
      }
      if (this->isUnstructured(_varname))
      {
         std::string bound = this->getBoundsId
                  (this->getCoordinatesIdList(_varname).back());
         std::string dim = this->getDimensionsIdList(&bound).back();
         return (this->getDimensions(&bound)[dim]);
      }
      return ((size_t)(-1));
   }
             
   std::set<std::string> CINetCDF4Adv::getCoordVariables(void)
   {
      std::set<std::string> retvalue;
      std::vector<std::string> variables = this->getVariables();
      std::vector<std::string>::const_iterator it = variables.begin(), end = variables.end();
      for (; it != end; it++)
      {
         const std::string & var = *it;
         std::vector<std::string> coords = this->getCoordinatesIdList(var);
         std::vector<std::string>::const_iterator it = coords.begin(), end = coords.end();
         for (; it != end; it++)
         {
            const std::string & coord = *it;
            if (this->hasVariable(coord))
               retvalue.insert(retvalue.end(), coord);
         }
      }
      return (retvalue);
   }
   
   std::set<std::string> CINetCDF4Adv::getBoundVariables(void)
   {
      std::set<std::string> retvalue;         
      std::vector<std::string> variables = this->getVariables();
      std::vector<std::string>::const_iterator it = variables.begin(), end = variables.end();
      for (; it != end; it++)
      {
         const std::string & var = *it;
         if (this->hasBounds(var))
            retvalue.insert(retvalue.end(), this->getBoundsId(var));
      }
      return (retvalue);
   }
            
   std::string CINetCDF4Adv::getCoordinatesId(const std::string & _varname)
   {
         std::string retvalue;
         if (this->hasAttribute("coordinates", &_varname))
         {
            this->readAttribute("coordinates", retvalue, &_varname);
            return (retvalue);
         }
         else
         {
            std::vector<std::string> dims = this->getDimensionsIdList(&_varname);
            std::vector<std::string>::const_iterator it = dims.begin(), end = dims.end();
            for (; it != end; it++)
            {
               const std::string & value = *it;               
               retvalue.append(value).push_back(' ');
            }
            retvalue.erase (retvalue.end()-1) ;
         }

         return (retvalue);
   }
   
   std::string CINetCDF4Adv::getBoundsId (const std::string & _varname)
   {
      std::string retvalue;
      if (this->hasAttribute("bounds", &_varname))
            this->readAttribute("bounds", retvalue, &_varname);

      return (retvalue);
   }
           
            
   std::vector<std::string> CINetCDF4Adv::getCoordinatesIdList(const std::string & _varname)
   {
      std::vector<std::string> retvalue;
      std::string value = this->getCoordinatesId(_varname);
         
      boost::split(retvalue, value, boost::is_any_of(" "));
         
      std::vector<std::string>::iterator it = retvalue.begin(), end = retvalue.end();
      for (; it != end; it++)
      {
         std::string & coord = *it;
         coord.assign(coord.data());
      }
      return (retvalue);
   }
   
   std::vector<std::string> CINetCDF4Adv::getDataVariables
        (bool _is3D, bool _isRecti, bool _isCurvi, bool _isUnstr, bool _isCellData, bool _isTemporal)
   {
      std::vector<std::string> retvalue;
      std::vector<std::string> allvars  = this->getVariables();

      std::vector<std::string>::const_iterator it = allvars.begin(), end = allvars.end();
      for (; it != end; it++)
      {
         const std::string & var = *it;
         if (this->isCoordinate(var)) continue;

         if (!_isRecti && this->isRectilinear(var) ) continue;
         if (!_isCurvi && this->isCurvilinear(var) ) continue;
         if (!_isUnstr && this->isUnstructured(var)) continue;

         if (!_isTemporal && this->isTemporal(var)) continue;
         if (!_is3D       && this->is3Dim(var)    ) continue;
         if (!_isCellData && this->isCellGrid(var)) continue;

         if (this->isUnknown(var)) continue;

         retvalue.push_back(var);
      }
      return (retvalue);
    }
   
    std::string CINetCDF4Adv::getLonCoordName (const std::string & _varname)
    {
       std::vector<std::string> clist = this->getCoordinatesIdList(_varname);
       std::vector<std::string>::const_iterator it = clist.begin(), end = clist.end();
       if (this->hasCoordinates(_varname))
       {
          for (; it != end; it++)
          {
              const std::string & var = *it;
              if (this->hasVariable(var) && this->hasAttribute("axis", &var))
              {
                  std::string axis_name;
                  this->readAttribute("axis", axis_name, &var);
                  if (axis_name.compare("X") == 0)
                      return (var);
              }
          }
          return (*clist.begin());
       }
       else
          return (*clist.rbegin());
    }
            
    std::string CINetCDF4Adv::getLatCoordName (const std::string & _varname)
    {
       std::vector<std::string> clist = this->getCoordinatesIdList(_varname);
       std::vector<std::string>::const_iterator it = clist.begin(), end = clist.end();
       if (this->hasCoordinates(_varname))
       {
          for (; it != end; it++)
          {
              const std::string & var = *it;
              if (this->hasVariable(var) && this->hasAttribute("axis", &var))
              {
                  std::string axis_name;
                  this->readAttribute("axis", axis_name, &var);
                  if (axis_name.compare("Y") == 0)
                      return (var);
              }
          }
          return (*(++clist.begin()));
       }
       else
          return (*(++clist.rbegin()));
    }
            
    std::string CINetCDF4Adv::getVertCoordName(const std::string & _varname)
    {
       if (!this->is3Dim(_varname)) return (std::string());
       std::vector<std::string> clist = this->getCoordinatesIdList(_varname);
       std::vector<std::string>::const_iterator it = clist.begin(), end = clist.end();
       if (this->hasCoordinates(_varname))
       {
          for (; it != end; it++)
          {
              const std::string & var = *it;
              if (this->hasVariable(var) && this->hasAttribute("axis", &var))
              {
                  std::string axis_name;
                  this->readAttribute("axis", axis_name, &var);
                  if (axis_name.compare("Z") == 0)
                      return (var);
              }
          }
          return (*(++(++clist.begin())));
       }
       else
          return (*(++(++clist.rbegin())));
    }
    
   std::string CINetCDF4Adv::getTimeCoordName(const std::string & _varname)
   {
      if (!this->isTemporal(_varname)) return (std::string());
       std::vector<std::string> clist = this->getCoordinatesIdList(_varname);
       std::vector<std::string>::const_iterator it = clist.begin(), end = clist.end();
       if (this->hasCoordinates(_varname))
       {
          for (; it != end; it++)
          {
              const std::string & var = *it;
              if (this->hasVariable(var) && this->hasAttribute("standard_name", &var))
              {
                  std::string time_name;
                  this->readAttribute("standard_name", time_name, &var);
                  if (time_name.compare("time") == 0)
                      return (var);
              }
          }
          return (this->getUnlimitedDimensionName());
       }
       else
          return (*(clist.begin()));
   }
   
   
} // namespace io
} // namespace xmlioserver