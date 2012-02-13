/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#include "icutil.hpp"

extern "C"
{
// /////////////////////////////// Définitions ////////////////////////////// //

   // ----------------------- Redéfinition de types ----------------------------
   
   typedef void * XAxisPtr  , * XAxisGroupPtr;
   typedef void * XDomainPtr, * XDomainGroupPtr;
   typedef void * XGridPtr  , * XGridGroupPtr;
   typedef void * XFieldPtr , * XFieldGroupPtr;
   typedef void * XFilePtr  , * XFileGroupPtr;
   
   // ----------------------- Ajout d'enfant à un parent -----------------------
   
   void xios_xml_tree_add_field(XFieldGroupPtr  parent_, XFieldPtr * child_, const char * child_id, int child_id_size)
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   void xios_xml_tree_add_grid(XGridGroupPtr   parent_, XGridPtr * child_, const char * child_id, int child_id_size)
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   void xios_xml_tree_add_file(XFileGroupPtr   parent_, XFilePtr * child_, const char * child_id, int child_id_size)
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   void xios_xml_tree_add_axis(XAxisGroupPtr   parent_, XAxisPtr * child_, const char * child_id, int child_id_size)
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   void xios_xml_tree_add_domain(XDomainGroupPtr parent_, XDomainPtr * child_, const char * child_id, int child_id_size) 
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   
   void xios_xml_tree_add_fieldtofile(XFilePtr parent_, XFieldPtr * child_, const char * child_id, int child_id_size)
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   void xios_xml_tree_add_fieldgrouptofile(XFilePtr parent_, XFieldGroupPtr * child_, const char * child_id, int child_id_size)
   {
      std::string child_id_str; 
      if (!cstr2string(child_id, child_id_size, child_id_str)) return; 
   }
   
   
   // ----------------------- Affichage de l'arborescence ----------------------
   
   void xios_xml_tree_show   (const char * filename, int filename_size) 
   {
      std::string filename_str; 
      if (!cstr2string(filename, filename_size, filename_str)) return; 
   }
     
   
   // ----------------------- Parsing de document xml --------------------------
   
   void xios_xml_parse_file  (const char * filename  , int filename_size)
   {
      std::string filename_str; 
      if (!cstr2string(filename, filename_size, filename_str)) return; 
   }
   
   void xios_xml_parse_string(const char * xmlcontent, int xmlcontent_size)
   {
      std::string xmlcontent_str; 
      if (!cstr2string(xmlcontent, xmlcontent_size, xmlcontent_str)) return; 
   }
   


} // extern "C"
