/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

#include "xmlioserver.hpp"

#include "attribute_template_impl.hpp"
#include "object_template_impl.hpp"
#include "group_template_impl.hpp"

#include "icutil.hpp"

extern "C"
{
// /////////////////////////////// Définitions ////////////////////////////// //

   // ----------------------- Redéfinition de types ----------------------------
   
   typedef void * XAxisPtr, * XAxisGroupPtr;

   // ------------------------- Attributs des axes -----------------------------
   void xios_set_axis_name 
      (XAxisPtr axis_hdl, const char * name , int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }

   void xios_set_axis_standard_name
      (XAxisPtr axis_hdl, const char * standard_name , int standard_name_size)
   {
      std::string standard_name_str; 
      if (!cstr2string(standard_name, standard_name_size, standard_name_str)) return; 
   }
    
   void xios_set_axis_long_name 
      (XAxisPtr axis_hdl, const char * long_name , int long_name_size)
   {
      std::string long_name_str; 
      if (!cstr2string(long_name, long_name_size, long_name_str)) return; 
   }

   void xios_set_axis_unit 
      (XAxisPtr axis_hdl, const char * unit , int unit_size)
   {
      std::string unit_str; 
      if (!cstr2string(unit, unit_size, unit_str)) return;
   }
    
   void xios_set_axis_size(void * axis_hdl, int size)
   {

   }

   void xios_set_axis_zvalue 
      (XAxisPtr axis_hdl, const double * zvalue , int zvalue_extent1)
   {
      boost::shared_ptr<boost::multi_array<double, 1> > zvalue_val
         (new boost::multi_array<double, 1>(boost::extents [zvalue_extent1],
                                  	    boost::fortran_storage_order()));
      std::copy(zvalue, &(zvalue[zvalue_val->num_elements()]), zvalue_val->data()); 

   }
   
   // -------------------- Attributs des groupes d'axes -------------------------
   
   void xios_set_axisgroup_name 
      (XAxisGroupPtr axisgroup_hdl, const char * name , int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }

   void xios_set_axisgroup_standard_name
      (XAxisGroupPtr axisgroup_hdl, const char * standard_name , int standard_name_size)
   {
      std::string standard_name_str; 
      if (!cstr2string(standard_name, standard_name_size, standard_name_str)) return;
   }
    
   void xios_set_axisgroup_long_name 
      (XAxisGroupPtr axisgroup_hdl, const char * long_name , int long_name_size)
   {
      std::string long_name_str; 
      if (!cstr2string(long_name, long_name_size, long_name_str)) return; 
   }

   void xios_set_axisgroup_unit 
      (XAxisGroupPtr axisgroup_hdl, const char * unit , int unit_size)
   {
      std::string unit_str; 
      if (!cstr2string(unit, unit_size, unit_str)) return;
   }
    
   void xios_set_axisgroup_size(XAxisGroupPtr axisgroup_hdl, int size)
   {

   }

   void xios_set_axisgroup_zvalue 
      (XAxisGroupPtr axisgroup_hdl, const double * zvalue , int zvalue_extent1)
   {
      boost::shared_ptr<boost::multi_array<double, 1> > zvalue_val
         (new boost::multi_array<double, 1>(boost::extents [zvalue_extent1],
                                  	    boost::fortran_storage_order()));
      std::copy(zvalue, &(zvalue[zvalue_val->num_elements()]), zvalue_val->data()); 
   }
   
   // ------------------------ Création des handle -----------------------------
   
   void xios_axis_handle_create (XAxisPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
   void xios_axisgroup_handle_create (XAxisGroupPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
} // extern "C"
