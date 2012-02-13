/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

#include "icutil.hpp"

extern "C"
{
// /////////////////////////////// Définitions ////////////////////////////// //

   // ----------------------- Redéfinition de types ----------------------------
   
   typedef enum { NETCDF4 = 0 } XFileType;
   
   typedef void * XContextPtr;

   // -------------------- Traitement des données ------------------------------
   
   void xios_dtreatment_start(XContextPtr * context_hdl, XFileType filetype, int comm_client_server, int comm_server)
   {
   
   }
   
   void xios_dtreatment_end(void)
   {
   
   }
   
   // ---------------------- Ecriture des données ------------------------------
   
   void xios_write_data_k81(const char * fieldid, int fieldid_size, double * data_k8, int data_Xsize)
   {
      std::string fieldid_str; 
      if (!cstr2string(fieldid, fieldid_size, fieldid_str)) return; 
      
      boost::const_multi_array_ref<double, 1> array_(data_k8, 
          boost::extents [data_Xsize],
          boost::fortran_storage_order());      
   }
   
   void xios_write_data_k82(const char * fieldid, int fieldid_size, double * data_k8, int data_Xsize, int data_Ysize)
   {
      std::string fieldid_str; 
      if (!cstr2string(fieldid, fieldid_size, fieldid_str)) return;
      
      boost::const_multi_array_ref<double, 2> array_(data_k8, 
          boost::extents [data_Xsize][data_Ysize],
          boost::fortran_storage_order());
   }
   
   void xios_write_data_k83(const char * fieldid, int fieldid_size, double * data_k8, int data_Xsize, int data_Ysize, int data_Zsize)
   {
      std::string fieldid_str; 
      if (!cstr2string(fieldid, fieldid_size, fieldid_str)) return; 
      
      boost::const_multi_array_ref<double, 3> array_(data_k8, 
          boost::extents [data_Xsize][data_Ysize][data_Zsize],
          boost::fortran_storage_order());
   }
   
   void xios_write_data_k41(const char * fieldid, int fieldid_size, float * data_k4, int data_Xsize)
   {
      std::string fieldid_str; 
      if (!cstr2string(fieldid, fieldid_size, fieldid_str)) return; 
      
      boost::const_multi_array_ref<float, 1> array_(data_k4, 
          boost::extents [data_Xsize],
          boost::fortran_storage_order());
   }
   
   void xios_write_data_k42(const char * fieldid, int fieldid_size, float * data_k4, int data_Xsize, int data_Ysize)
   {
      std::string fieldid_str; 
      if (!cstr2string(fieldid, fieldid_size, fieldid_str)) return; 
      
      boost::const_multi_array_ref<float, 2> array_(data_k4, 
          boost::extents [data_Xsize][data_Ysize],
          boost::fortran_storage_order());
   }
   
   void xios_write_data_k43(const char * fieldid, int fieldid_size, float * data_k4, int data_Xsize, int data_Ysize, int data_Zsize)
   {
      std::string fieldid_str; 
      if (!cstr2string(fieldid, fieldid_size, fieldid_str)) return; 
      
      boost::const_multi_array_ref<float, 3> array_(data_k4, 
          boost::extents [data_Xsize][data_Ysize][data_Zsize],
          boost::fortran_storage_order());
   } 

} // extern "C"
