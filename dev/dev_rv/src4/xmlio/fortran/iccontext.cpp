/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#include "icutil.hpp"

extern "C"
{
// /////////////////////////////// Définitions ////////////////////////////// //

   // ----------------------- Redéfinition de types ----------------------------

   typedef enum { D360 = 0 , ALLLEAP, NOLEAP, JULIAN, GREGORIAN } XCalendarType ;

   typedef void * XContextPtr;

   // ------------------------- Attributs des contextes ------------------------
   
   
   void xios_set_context_calendar_type(XContextPtr context_hdl, const char * calendar_type, int calendar_type_size)
   {
      std::string calendar_type_str; 
      if (!cstr2string(calendar_type, calendar_type_size, calendar_type_str)) return; 
   }
   
   void xios_set_context_start_date(XContextPtr context_hdl, const char * start_date, int start_date_size)
   {
      std::string start_date_str; 
      if (!cstr2string(start_date, start_date_size, start_date_str)) return; 
   }
   
   void xios_set_context_output_dir(XContextPtr context_hdl, const char * output_dir, int output_dir_size)
   {
      std::string output_dir_str; 
      if (!cstr2string(output_dir, output_dir_size, output_dir_str)) return; 
   }
   
   // ------------------------ Création des handle -----------------------------
   
   void xios_context_handle_create (XContextPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
   // ------------------------ Changements de contextes ------------------------
   
   void xios_context_set_current(XContextPtr context, bool withswap)
   {
   
   }
   
   // ------------------------ Création de contexte ----------------------------
   
   void xios_context_create(XContextPtr * context,
                            const char  * context_id,
                            int           context_id_size,
                            XCalendarType calendar_type,
                            int           year,
                            int           month,
                            int           day,
                            int           hour,
                            int           minute,
                            int           second)
   {
      std::string context_id_str; 
      if (!cstr2string(context_id, context_id_size, context_id_str)) return;
   
   }
   
} // extern "C"
