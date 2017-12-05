#include "ep_lib.hpp"
#include "ep_lib_fortran.hpp"
#include <mpi.h>
#include <map>
#include <utility>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

// #ifdef _openmpi
// //#undef MPI_Fint
// #endif

namespace ep_lib
{

  int EP_Comm_c2f(MPI_Comm comm)
  {
    Debug("MPI_Comm_c2f");
    int fint;
    //#ifdef _intelmpi
    fint = (::MPI_Fint)(to_mpi_comm(comm.mpi_comm));
    //#elif _openmpi
    //fint = ::MPI_Comm_c2f(to_mpi_comm(comm.mpi_comm));
    //#endif
    
    std::map<std::pair<int, int>, MPI_Comm > ::iterator it;
    
    #pragma omp critical (fc_comm_map)
    {
      it = fc_comm_map.find(std::make_pair(fint, omp_get_thread_num()));
      if(it == fc_comm_map.end())
      {
        fc_comm_map.insert(std::make_pair( std::make_pair( fint, omp_get_thread_num()) , comm));
        printf("EP_Comm_c2f : MAP %p insert: %d, %d, %p\n", &fc_comm_map, fint, omp_get_thread_num(), comm.ep_comm_ptr);
      }
    }
    
    
    return fint;
    
  }

  MPI_Comm EP_Comm_f2c(int comm)
  {
    Debug("MPI_Comm_f2c");
    
    
    std::map<std::pair<int, int>, MPI_Comm > ::iterator it;
    
    #pragma omp critical (fc_comm_map)
    it = fc_comm_map.find(std::make_pair(comm, omp_get_thread_num()));
    
    if(it != fc_comm_map.end())
    {
      MPI_Comm comm_ptr;
      comm_ptr = it->second;
      printf("EP_Comm_f2c : MAP %p find: %d, %d, %p\n", &fc_comm_map, it->first.first, it->first.second, comm_ptr.ep_comm_ptr);
      return  comm_ptr;
    }
       
      
    //#ifdef _openmpi
    //::MPI_Comm base_comm = ::MPI_Comm_f2c(comm);
    //#elif _intelmpi
    ::MPI_Comm *base_comm = new ::MPI_Comm;
    *base_comm = (::MPI_Comm)(comm);
    //#endif

    if(*base_comm != to_mpi_comm(MPI_COMM_NULL.mpi_comm))
    {
      if(omp_get_thread_num() == 0)
      {
        int num_ep = omp_get_num_threads();
        MPI_Comm *new_comm;
        MPI_Info info;
        MPI_Comm_create_endpoints(base_comm, num_ep, info, new_comm);
        passage = new_comm;
      }
      #pragma omp barrier

      MPI_Comm return_comm = passage[omp_get_thread_num()];
      return return_comm;
        
    }
    return MPI_COMM_NULL;

  }

}


