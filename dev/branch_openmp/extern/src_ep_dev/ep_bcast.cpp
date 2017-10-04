/*!
   \file ep_bcast.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Bcast
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;

namespace ep_lib
{

  int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
  {

    if(!comm.is_ep)
    {
      #pragma omp single nowait
      ::MPI_Bcast(buffer, count, static_cast< ::MPI_Datatype>(datatype), root, static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
    }


    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;

    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_rank_loc = comm.rank_map->at(root).first;

    // printf("root_mpi_rank = %d\n", root_mpi_rank);    

    if((ep_rank_loc==0 && mpi_rank != root_mpi_rank ) || ep_rank == root)
    {
      ::MPI_Bcast(buffer, count, to_mpi_type(datatype), root_mpi_rank, to_mpi_comm(comm.mpi_comm));
    }

    if(mpi_rank == root_mpi_rank) MPI_Bcast_local(buffer, count, datatype, root_ep_rank_loc, comm);
    else                          MPI_Bcast_local(buffer, count, datatype, 0, comm);

    return 0;
  }



  int MPI_Bcast_local(void *buffer, int count, MPI_Datatype datatype, int local_root, MPI_Comm comm)
  {
    assert(valid_type(datatype));

    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);
    

    if(ep_rank_loc == local_root)
    {
      comm.my_buffer->void_buffer[local_root] = buffer;
    }

//    #pragma omp flush
    MPI_Barrier_local(comm);
//    #pragma omp flush

    if(ep_rank_loc != local_root)
    {
      #pragma omp critical (_bcast)      
      memcpy(buffer, comm.my_buffer->void_buffer[local_root], datasize * count);
    }

    MPI_Barrier_local(comm);
  }

}
