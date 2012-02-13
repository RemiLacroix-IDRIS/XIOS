/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#include "icutil.hpp"

extern "C"
{
// /////////////////////////////// Définitions ////////////////////////////// //

   // ----------------------- Redéfinition de types ----------------------------
   
   typedef void * XFieldPtr, * XFieldGroupPtr;

   // ------------------------- Attributs des champs ---------------------------
   
   void xios_set_field_name(XFieldPtr field_hdl, const char * name, int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }
   
   void xios_set_field_standard_name(XFieldPtr field_hdl, const char * standard_name, int standard_name_size)
   {
      std::string standard_name_str; 
      if (!cstr2string(standard_name, standard_name_size, standard_name_str)) return; 
   }
   
   void xios_set_field_long_name(XFieldPtr field_hdl, const char * long_name, int long_name_size)
   {
      std::string long_name_str; 
      if (!cstr2string(long_name, long_name_size, long_name_str)) return;  
   }
   
   void xios_set_field_unit(XFieldPtr field_hdl, const char * unit, int unit_size)
   {
      std::string unit_str; 
      if (!cstr2string(unit, unit_size, unit_str)) return; 
   }
   
   void xios_set_field_operation(XFieldPtr field_hdl, const char * operation, int operation_size) 
   {
      std::string operation_str; 
      if (!cstr2string(operation, operation_size, operation_str)) return; 
   }
   
   void xios_set_field_freq_op(XFieldPtr field_hdl, const char * freq_op, int freq_op_size)
   {
      std::string freq_op_str; 
      if (!cstr2string(freq_op, freq_op_size, freq_op_str)) return; 
   }
   
   void xios_set_field_level(XFieldPtr field_hdl, int level)
   {
      
   }
   
   void xios_set_field_prec(XFieldPtr field_hdl, int prec)
   {
      
   }
   
   void xios_set_field_enabled(XFieldPtr field_hdl, bool enabled)
   {
      
   }
   
   void xios_set_field_domain_ref(XFieldPtr field_hdl,  const char * domain_ref, int domain_ref_size)
   {
      std::string domain_ref_str; 
      if (!cstr2string(domain_ref, domain_ref_size, domain_ref_str)) return; 
   }
   
   void xios_set_field_axis_ref(XFieldPtr field_hdl,  const char * axis_ref,  int axis_ref_size)
   {
      std::string axis_ref_str; 
      if (!cstr2string(axis_ref, axis_ref_size, axis_ref_str)) return; 
   }
   
   void xios_set_field_grid_ref(XFieldPtr field_hdl,  const char * grid_ref, int grid_ref_size)
   {
      std::string grid_ref_str; 
      if (!cstr2string(grid_ref, grid_ref_size, grid_ref_str)) return; 
   }
   
   void xios_set_field_field_ref(XFieldPtr field_hdl,  const char * field_ref,  int field_ref_size)
   {
      std::string field_ref_str; 
      if (!cstr2string(field_ref, field_ref_size, field_ref_str)) return; 
   }
   
   void xios_set_field_default_value(XFieldPtr field_hdl, double default_value)
   {
      
   }  
   
   // -------------------- Attributs des groupes de champs ---------------------
  
   void xios_set_fieldgroup_name(XFieldGroupPtr fieldgroup_hdl, const char * name, int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return;  
   }
   
   void xios_set_fieldgroup_standard_name(XFieldGroupPtr fieldgroup_hdl, const char * standard_name, int standard_name_size)
   {
      std::string standard_name_str; 
      if (!cstr2string(standard_name, standard_name_size, standard_name_str)) return;
   }
   
   void xios_set_fieldgroup_long_name(XFieldGroupPtr fieldgroup_hdl, const char * long_name, int long_name_size)
   {
      std::string long_name_str; 
      if (!cstr2string(long_name, long_name_size, long_name_str)) return;
   }
   
   void xios_set_fieldgroup_unit(XFieldGroupPtr fieldgroup_hdl, const char * unit, int unit_size)
   {
      std::string unit_str; 
      if (!cstr2string(unit, unit_size, unit_str)) return;
   }
   
   void xios_set_fieldgroup_operation(XFieldGroupPtr fieldgroup_hdl, const char * operation, int operation_size) 
   {
      std::string operation_str; 
      if (!cstr2string(operation, operation_size, operation_str)) return; 
   }
   
   void xios_set_fieldgroup_freq_op(XFieldGroupPtr fieldgroup_hdl, const char * freq_op, int freq_op_size)
   {
      std::string freq_op_str; 
      if (!cstr2string(freq_op, freq_op_size, freq_op_str)) return;
   }
   
   void xios_set_fieldgroup_level(XFieldGroupPtr fieldgroup_hdl, int level)
   {
      
   }
   
   void xios_set_fieldgroup_prec(XFieldGroupPtr fieldgroup_hdl, int prec)
   {
      
   }
   
   void xios_set_fieldgroup_enabled(XFieldGroupPtr fieldgroup_hdl, bool enabled)
   {
      
   }
   
   void xios_set_fieldgroup_domain_ref(XFieldGroupPtr fieldgroup_hdl,  const char * domain_ref, int domain_ref_size)
   {
      std::string domain_ref_str; 
      if (!cstr2string(domain_ref, domain_ref_size, domain_ref_str)) return; 
   }
   
   void xios_set_fieldgroup_axis_ref(XFieldGroupPtr fieldgroup_hdl,  const char * axis_ref,  int axis_ref_size)
   {
      std::string axis_ref_str; 
      if (!cstr2string(axis_ref, axis_ref_size, axis_ref_str)) return; 
   }
   
   void xios_set_fieldgroup_grid_ref(XFieldGroupPtr fieldgroup_hdl,  const char * grid_ref, int grid_ref_size)
   {
      std::string grid_ref_str; 
      if (!cstr2string(grid_ref, grid_ref_size, grid_ref_str)) return;
   }
   
   void xios_set_fieldgroup_field_ref(XFieldGroupPtr fieldgroup_hdl,  const char * field_ref,  int field_ref_size)
   {
      std::string field_ref_str; 
      if (!cstr2string(field_ref, field_ref_size, field_ref_str)) return; 
   }
   
   void xios_set_fieldgroup_default_value(XFieldGroupPtr fieldgroup_hdl, double default_value)
   {
      
   }  
   
   // ------------------------ Création des handle -----------------------------
   
   void xios_field_handle_create (XFieldPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
   void xios_fieldgroup_handle_create (XFieldGroupPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
} // extern "C"
