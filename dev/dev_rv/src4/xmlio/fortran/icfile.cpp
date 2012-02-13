/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#include "icutil.hpp"

extern "C"
{
// /////////////////////////////// Définitions ////////////////////////////// //

   // ----------------------- Redéfinition de types ----------------------------
   
   typedef void * XFilePtr, * XFileGroupPtr;

   // ------------------------- Attributs des axes -----------------------------
   
   void xios_set_file_name(XFilePtr file_hdl, const char * name, int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }
   
   void xios_set_file_description(XFilePtr file_hdl, const char * description, int description_size)
   {
      std::string description_str; 
      if (!cstr2string(description, description_size, description_str)) return; 
   } 
   
   void xios_set_file_output_freq(XFilePtr file_hdl, const char * output_freq, int output_freq_size)
   {
      std::string output_freq_str; 
      if (!cstr2string(output_freq, output_freq_size, output_freq_str)) return; 
   }
   
   void xios_set_file_output_level(XFilePtr file_hdl, int output_level)
   {
   
   }
   
   void xios_set_file_enabled(XFilePtr file_hdl, bool enabled)
   {
   
   }
   
   // -------------------- Attributs des groupes d'axes ------------------------
   
   void xios_set_filegroup_name(XFileGroupPtr filegroup_hdl, const char * name, int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }
   
   void xios_set_filegroup_description(XFileGroupPtr filegroup_hdl, const char * description, int description_size)
   {
      std::string description_str; 
      if (!cstr2string(description, description_size, description_str)) return; 
   } 
   
   void xios_set_filegroup_output_freq(XFileGroupPtr filegroup_hdl, const char * output_freq, int output_freq_size)
   {
      std::string output_freq_str; 
      if (!cstr2string(output_freq, output_freq_size, output_freq_str)) return; 
   }
   
   void xios_set_filegroup_output_level(XFileGroupPtr filegroup_hdl, int output_level)
   {
   
   }
   
   void xios_set_filegroup_enabled(XFileGroupPtr filegroup_hdl, bool enabled)
   {
   
   }
   
   // ------------------------ Création des handle -----------------------------
   
   void xios_file_handle_create (XFilePtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
   void xios_filegroup_handle_create (XFileGroupPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
} // extern "C"
