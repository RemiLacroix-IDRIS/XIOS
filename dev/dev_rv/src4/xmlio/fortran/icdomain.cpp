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
   
   typedef void * XDomainPtr, * XDomainGroupPtr;

   // ------------------------- Attributs des axes -----------------------------
   
   void xios_set_domain_name(XDomainPtr domain_hdl, const char * name, int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }
   
   void xios_set_domain_standard_name(XDomainPtr domain_hdl, const char * standard_name, int standard_name_size)
   {
      std::string standard_name_str; 
      if (!cstr2string(standard_name, standard_name_size, standard_name_str)) return; 
   }
   
   void xios_set_domain_long_name(XDomainPtr domain_hdl, const char * long_name, int long_name_size)
   {
      std::string long_name_str; 
      if (!cstr2string(long_name, long_name_size, long_name_str)) return; 
   }
   
   void xios_set_domain_domain_group_ref(XDomainPtr domain_hdl, const char * domain_group_ref, int domain_group_ref_size)
   {
      std::string domain_group_ref_str; 
      if (!cstr2string(domain_group_ref, domain_group_ref_size, domain_group_ref_str)) return; 
   }
   
   void xios_set_domain_ni_glo(XDomainPtr domain_hdl, int ni_glo)
   {
   
   }
   
   void xios_set_domain_nj_glo(XDomainPtr domain_hdl, int nj_glo)
   {
   
   }
   
   void xios_set_domain_ibegin(XDomainPtr domain_hdl, int ibegin)
   {
   
   }
   
   void xios_set_domain_iend(XDomainPtr domain_hdl, int iend)
   {
   
   }
   
   void xios_set_domain_ni(XDomainPtr domain_hdl, int ni)
   {
   
   }
   
   void xios_set_domain_jbegin(XDomainPtr domain_hdl, int jbegin)
   {
   
   }
   
   void xios_set_domain_jend(XDomainPtr domain_hdl, int jend)
   {
   
   }
   
   void xios_set_domain_nj(XDomainPtr domain_hdl, int nj)
   {
   
   }
   
   void xios_set_domain_mask(XDomainPtr domain_hdl, bool * mask, int mask_extent1, int mask_extent2)
   {
      boost::shared_ptr<boost::multi_array<bool, 2> > mask_val
         (new boost::multi_array<bool, 2>(boost::extents [mask_extent1][mask_extent2],
                                  	  boost::fortran_storage_order()));
      std::copy(mask, &(mask[mask_val->num_elements()]), mask_val->data()); 
   }
   
   void xios_set_domain_data_dim(XDomainPtr domain_hdl, int data_dim)
   {
   
   }
   
   void xios_set_domain_data_ni(XDomainPtr domain_hdl, int data_ni)
   {
   
   }
   
   void xios_set_domain_data_nj(XDomainPtr domain_hdl, int data_nj)
   {
   
   }
   
   void xios_set_domain_data_ibegin(XDomainPtr domain_hdl, int data_ibegin)
   {
   
   }
   
   void xios_set_domain_data_jbegin(XDomainPtr domain_hdl, int data_jbegin)
   {
   
   }
   
   void xios_set_domain_zoom_ni(XDomainPtr domain_hdl, int zoom_ni)
   {
   
   }
   
   void xios_set_domain_zoom_nj(XDomainPtr domain_hdl, int zoom_nj)
   {
   
   }
   
   void xios_set_domain_zoom_ibegin(XDomainPtr domain_hdl, int zoom_ibegin)
   {
   
   }
    
   void xios_set_domain_zoom_jbegin(XDomainPtr domain_hdl, int zoom_jbegin)
   {
   
   }
   
   void xios_set_domain_zoom_ni_loc(XDomainPtr domain_hdl, int zoom_ni_loc)
   {
   
   }
   
   void xios_set_domain_zoom_nj_loc(XDomainPtr domain_hdl, int zoom_nj_loc)
   {
   
   }
   
   void xios_set_domain_zoom_ibegin_loc(XDomainPtr domain_hdl, int zoom_ibegin_loc)
   {
   
   }
   
   void xios_set_domain_zoom_jbegin_loc(XDomainPtr domain_hdl, int zoom_jbegin_loc)
   {
   
   }
   
   void xios_set_domain_data_n_index(XDomainPtr domain_hdl, int data_n_index)
   {
   
   }
   
   void xios_set_domain_data_i_index(XDomainPtr domain_hdl, int * data_i_index, int data_i_index_extent1)
   {
      boost::shared_ptr<boost::multi_array<int, 1> > data_i_index_val
         (new boost::multi_array<int, 1>(boost::extents [data_i_index_extent1],
                                  	 boost::fortran_storage_order()));
      std::copy(data_i_index, &(data_i_index[data_i_index_val->num_elements()]), data_i_index_val->data()); 
   }
   
   void xios_set_domain_data_j_index(XDomainPtr domain_hdl, int * data_j_index, int data_j_index_extent1)
   {
      boost::shared_ptr<boost::multi_array<int, 1> > data_j_index_val
         (new boost::multi_array<int, 1>(boost::extents [data_j_index_extent1],
                                  	 boost::fortran_storage_order()));
      std::copy(data_j_index, &(data_j_index[data_j_index_val->num_elements()]), data_j_index_val->data()); 
   }
    
   void xios_set_domain_lonvalue(XDomainPtr domain_hdl, double * lonvalue, int lonvalue_extent1)
   {
      boost::shared_ptr<boost::multi_array<double, 1> > lonvalue_val
         (new boost::multi_array<double, 1>(boost::extents [lonvalue_extent1],
                                  	    boost::fortran_storage_order()));
      std::copy(lonvalue, &(lonvalue[lonvalue_val->num_elements()]), lonvalue_val->data());
   }
   
   void xios_set_domain_latvalue(XDomainPtr domain_hdl, double * latvalue, int latvalue_extent1)
   {
      boost::shared_ptr<boost::multi_array<double, 1> > latvalue_val
         (new boost::multi_array<double, 1>(boost::extents [latvalue_extent1],
                                  	    boost::fortran_storage_order()));
      std::copy(latvalue, &(latvalue[latvalue_val->num_elements()]), latvalue_val->data());
   }   
   
   // -------------------- Attributs des groupes d'axes ------------------------
   
   
   void xios_set_domaingroup_name(XDomainGroupPtr domaingroup_hdl, const char * name, int name_size)
   {
      std::string name_str; 
      if (!cstr2string(name, name_size, name_str)) return; 
   }
   
   void xios_set_domaingroup_standard_name(XDomainGroupPtr domaingroup_hdl, const char * standard_name, int standard_name_size)
   {
      std::string standard_name_str; 
      if (!cstr2string(standard_name, standard_name_size, standard_name_str)) return;
   }
   
   void xios_set_domaingroup_long_name(XDomainGroupPtr domaingroup_hdl, const char * long_name, int long_name_size)
   {
      std::string long_name_str; 
      if (!cstr2string(long_name, long_name_size, long_name_str)) return; 
   }
   
   void xios_set_domaingroup_domain_group_ref(XDomainGroupPtr domaingroup_hdl, const char * domain_group_ref, int domain_group_ref_size)
   {
      std::string domain_group_ref_str; 
      if (!cstr2string(domain_group_ref, domain_group_ref_size, domain_group_ref_str)) return;
   }
   
   void xios_set_domaingroup_ni_glo(XDomainGroupPtr domaingroup_hdl, int ni_glo)
   {
   
   }
   
   void xios_set_domaingroup_nj_glo(XDomainGroupPtr domaingroup_hdl, int nj_glo)
   {
   
   }
   
   void xios_set_domaingroup_ibegin(XDomainGroupPtr domaingroup_hdl, int ibegin)
   {
   
   }
   
   void xios_set_domaingroup_iend(XDomainGroupPtr domaingroup_hdl, int iend)
   {
   
   }
   
   void xios_set_domaingroup_ni(XDomainGroupPtr domaingroup_hdl, int ni)
   {
   
   }
   
   void xios_set_domaingroup_jbegin(XDomainGroupPtr domaingroup_hdl, int jbegin)
   {
   
   }
   
   void xios_set_domaingroup_jend(XDomainGroupPtr domaingroup_hdl, int jend)
   {
   
   }
   
   void xios_set_domaingroup_nj(XDomainGroupPtr domaingroup_hdl, int nj)
   {
   
   }
   
   void xios_set_domaingroup_mask(XDomainGroupPtr domaingroup_hdl, bool * mask, int mask_extent1, int mask_extent2)
   {
      boost::shared_ptr<boost::multi_array<bool, 2> > mask_val
         (new boost::multi_array<bool, 2>(boost::extents [mask_extent1][mask_extent2],
                                  	  boost::fortran_storage_order()));
      std::copy(mask, &(mask[mask_val->num_elements()]), mask_val->data()); 
   }
   
   void xios_set_domaingroup_data_dim(XDomainGroupPtr domaingroup_hdl, int data_dim)
   {
   
   }
   
   void xios_set_domaingroup_data_ni(XDomainGroupPtr domaingroup_hdl, int data_ni)
   {
   
   }
   
   void xios_set_domaingroup_data_nj(XDomainGroupPtr domaingroup_hdl, int data_nj)
   {
   
   }
   
   void xios_set_domaingroup_data_ibegin(XDomainGroupPtr domaingroup_hdl, int data_ibegin)
   {
   
   }
   
   void xios_set_domaingroup_data_jbegin(XDomainGroupPtr domaingroup_hdl, int data_jbegin)
   {
   
   }
   
   void xios_set_domaingroup_zoom_ni(XDomainGroupPtr domaingroup_hdl, int zoom_ni)
   {
   
   }
   
   void xios_set_domaingroup_zoom_nj(XDomainGroupPtr domaingroup_hdl, int zoom_nj)
   {
   
   }
   
   void xios_set_domaingroup_zoom_ibegin(XDomainGroupPtr domaingroup_hdl, int zoom_ibegin)
   {
   
   }
    
   void xios_set_domaingroup_zoom_jbegin(XDomainGroupPtr domaingroup_hdl, int zoom_jbegin)
   {
   
   }
   
   void xios_set_domaingroup_zoom_ni_loc(XDomainGroupPtr domaingroup_hdl, int zoom_ni_loc)
   {
   
   }
   
   void xios_set_domaingroup_zoom_nj_loc(XDomainGroupPtr domaingroup_hdl, int zoom_nj_loc)
   {
   
   }
   
   void xios_set_domaingroup_zoom_ibegin_loc(XDomainGroupPtr domaingroup_hdl, int zoom_ibegin_loc)
   {
   
   }
   
   void xios_set_domaingroup_zoom_jbegin_loc(XDomainGroupPtr domaingroup_hdl, int zoom_jbegin_loc)
   {
   
   }
   
   void xios_set_domaingroup_data_n_index(XDomainGroupPtr domaingroup_hdl, int data_n_index)
   {
   
   }
   
   void xios_set_domaingroup_data_i_index(XDomainGroupPtr domaingroup_hdl, int * data_i_index, int data_i_index_extent1)
   {
      boost::shared_ptr<boost::multi_array<int, 1> > data_i_index_val
         (new boost::multi_array<int, 1>(boost::extents [data_i_index_extent1],
                                  	  boost::fortran_storage_order()));
      std::copy(data_i_index, &(data_i_index[data_i_index_val->num_elements()]), data_i_index_val->data()); 
   }
   
   void xios_set_domaingroup_data_j_index(XDomainGroupPtr domaingroup_hdl, int * data_j_index, int data_j_index_extent1)
   {
      boost::shared_ptr<boost::multi_array<int, 1> > data_j_index_val
         (new boost::multi_array<int, 1>(boost::extents [data_j_index_extent1],
                                  	  boost::fortran_storage_order()));
      std::copy(data_j_index, &(data_j_index[data_j_index_val->num_elements()]), data_j_index_val->data()); 
   }
    
   void xios_set_domaingroup_lonvalue(XDomainGroupPtr domaingroup_hdl, double * lonvalue, int lonvalue_extent1)
   {
      boost::shared_ptr<boost::multi_array<double, 1> > lonvalue_val
         (new boost::multi_array<double, 1>(boost::extents [lonvalue_extent1],
                                  	    boost::fortran_storage_order()));
      std::copy(lonvalue, &(lonvalue[lonvalue_val->num_elements()]), lonvalue_val->data());
   }
   
   void xios_set_domaingroup_latvalue(XDomainGroupPtr domaingroup_hdl, double * latvalue, int latvalue_extent1)
   {
      boost::shared_ptr<boost::multi_array<double, 1> > latvalue_val
         (new boost::multi_array<double, 1>(boost::extents [latvalue_extent1],
                                  	    boost::fortran_storage_order()));
      std::copy(latvalue, &(latvalue[latvalue_val->num_elements()]), latvalue_val->data());
   }  
   
   // ------------------------ Création des handle -----------------------------
   
   void xios_domain_handle_create (XDomainPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
   void xios_domaingroup_handle_create (XDomainGroupPtr * _ret, const char * _id, int _id_len)
   {
      std::string id; 
      if (!cstr2string(_id, _id_len, id)) return;
   }
   
} // extern "C"
