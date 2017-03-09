#include "ep_lib.hpp"
#include "ep_lib_fortran.hpp"
#include <mpi.h>
#include <map>
#include <utility>
#include "ep_declaration.hpp"


namespace ep_lib
{

  int EP_Comm_c2f(MPI_Comm comm)
  {
    Debug("MPI_Comm_c2f");
    int fint;
    #ifdef _intelmpi
    fint = (::MPI_Fint)(comm.mpi_comm);
    #elif _openmpi
    fint = ::MPI_Comm_c2f(static_cast< ::MPI_Comm>(comm.mpi_comm));
    #endif
    std::map<std::pair<int, int>, MPI_Comm > ::iterator it;
    
    it = fc_comm_map.find(std::make_pair(fint, omp_get_thread_num()));
    if(it == fc_comm_map.end())
    {
      fc_comm_map.insert(std::make_pair( std::make_pair( fint, omp_get_thread_num()) , comm));
      // printf("EP_Comm_c2f : MAP insert: %d, %d, %p\n", fint, omp_get_thread_num(), &comm);
    }
    
    
    return fint;
    
  }

  MPI_Comm EP_Comm_f2c(int comm)
  {
    Debug("MPI_Comm_f2c");
    
    
    std::map<std::pair<int, int>, MPI_Comm > ::iterator it;
    
    it = fc_comm_map.find(std::make_pair(comm, omp_get_thread_num()));
    if(it != fc_comm_map.end())
    {
      MPI_Comm comm_ptr;
      comm_ptr = it->second;
      // printf("EP_Comm_f2c : MAP find: %d, %d, %p\n", it->first.first, it->first.second, it->second);
      return  comm_ptr;
    }
    else
    {      
      MPI_Comm return_comm;
      if(omp_get_thread_num() == 0)
      {
        #ifdef _openmpi
        ::MPI_Comm base_comm = ::MPI_Comm_f2c(comm);
        #elif _intelmpi
        ::MPI_Comm base_comm = (::MPI_Comm)(comm);
        #endif

        if(base_comm != MPI_COMM_NULL_STD)
        {
          int num_ep = omp_get_num_threads();
          MPI_Comm *new_comm;
          MPI_Info info;
          MPI_Comm_create_endpoints(base_comm, num_ep, info, new_comm);
          return_comm = new_comm[omp_get_thread_num()];
        }
        return MPI_COMM_NULL;
      }
      
      
      return return_comm;
    }
  }

  // #ifdef _intelmpi

  // MPI_Fint MPI_Comm_c2f(MPI_Comm comm)
  // {
  //   Debug("MPI_Comm_c2f");
  //   int fint;
  //   fint = (::MPI_Fint)(comm.mpi_comm);
    
  //   std::map<std::pair<int, int>, MPI_Comm  > ::iterator it;
    
  //   it = fc_comm_map.find(std::make_pair(fint, omp_get_thread_num()));
  //   if(it == fc_comm_map.end())
  //   {
  //     fc_comm_map.insert(std::make_pair( std::make_pair( fint, omp_get_thread_num()) , comm));
  //     printf("MAP insert: %d, %d, %p\n", fint, omp_get_thread_num(), &comm);
  //   }
    
  //   MPI_Fint Fint;
  //   Fint.mpi_fint = fint;
  //   return Fint;
    
  // }

  


  // MPI_Comm MPI_Comm_f2c(MPI_Fint comm)
  // {
  //   Debug("MPI_Comm_f2c");
    
    
  //   std::map<std::pair<int, int>, MPI_Comm  > ::iterator it;
    
  //   it = fc_comm_map.find(std::make_pair(comm.mpi_fint, omp_get_thread_num()));
  //   if(it != fc_comm_map.end())
  //   {
  //     MPI_Comm comm_ptr;
  //     comm_ptr =  it->second;
  //     printf("MAP find: %d, %d, %p\n", it->first.first, it->first.second, &comm_ptr);
  //     return  comm_ptr;
  //   }
  //   else
  //   {      
  //     MPI_Comm return_comm;
  //     return_comm.mpi_comm = (::MPI_Comm)(comm.mpi_fint);
  //     return return_comm;
  //   }
  // }

  

  // #elif _openmpi
  
  // int MPI_Comm_c2f(MPI_Comm comm)
  // {
  //   Debug("MPI_Comm_c2f");
  //   int fint;
  //   fint = ::MPI_Comm_c2f(static_cast< ::MPI_Comm>(comm.mpi_comm));
    
  //   std::map<std::pair<int, int>, MPI_Comm > ::iterator it;
    
  //   it = fc_comm_map.find(std::make_pair(fint, omp_get_thread_num()));
  //   if(it == fc_comm_map.end())
  //   {
  //     fc_comm_map.insert(std::make_pair( std::make_pair( fint, omp_get_thread_num()) , comm));
  //     printf("MAP insert: %d, %d, %p\n", fint, omp_get_thread_num(), &comm);
  //   }
    
  //   return fint;
    
  // }

  // ep_lib::MPI_Comm MPI_Comm_f2c(MPI_Fint comm)
  // {
  //   Debug("MPI_Comm_f2c");
    
    
  //   std::map<std::pair<int, int>, MPI_Comm > ::iterator it;
    
  //   it = fc_comm_map.find(std::make_pair(comm, omp_get_thread_num()));
  //   if(it != fc_comm_map.end())
  //   {
  //     MPI_Comm comm_ptr;
  //     comm_ptr =  it->second;
  //     printf("MAP find: %d, %d, %p\n", it->first.first, it->first.second, &comm_ptr);
  //     return  comm_ptr;
  //   }
  //   else
  //   {      
  //     MPI_Comm return_comm;
  //     return_comm.mpi_comm = (::MPI_Comm)(comm);
  //     return return_comm;
  //   }
  // }
  // #endif

}


