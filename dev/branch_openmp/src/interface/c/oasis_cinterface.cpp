#include "oasis_cinterface.hpp"
#include <string>
using namespace ep_lib;

namespace xios
{ 
 
  void oasis_init(const std::string& server_id)
  {
    fxios_oasis_init(server_id.data(),server_id.size()) ;
  }
  
  void oasis_finalize(void)
  {
    fxios_oasis_finalize() ;
  }
  
  void oasis_enddef(void)
  {
    fxios_oasis_enddef() ;
  }

  void oasis_get_localcomm(MPI_Comm& comm)
  {
    MPI_Fint f_comm ;
    
    fxios_oasis_get_localcomm(&f_comm) ;
    comm=EP_Comm_f2c(f_comm.mpi_fint) ;
  }
 
  void oasis_get_intracomm(MPI_Comm& comm_client_server,const std::string& server_id)
  {
    MPI_Fint f_comm ;
    
    fxios_oasis_get_intracomm(&f_comm,server_id.data(),server_id.size()) ;
    comm_client_server=EP_Comm_f2c(f_comm.mpi_fint) ;
  }
 
  void oasis_get_intercomm(MPI_Comm& comm_client_server,const std::string& server_id)
  {
    MPI_Fint f_comm ;
    
    fxios_oasis_get_intercomm(&f_comm,server_id.data(),server_id.size()) ;
    comm_client_server=EP_Comm_f2c(f_comm.mpi_fint) ;
  }
}
