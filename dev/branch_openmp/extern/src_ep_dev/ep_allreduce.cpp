/*!
   \file ep_reduce.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Reduce, MPI_Allreduce
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;


namespace ep_lib
 {



  int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
  {
    if(!comm.is_ep && comm.mpi_comm)
    {
      return ::MPI_Allreduce(sendbuf, recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), to_mpi_comm(comm.mpi_comm));
    }



    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;


    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    bool is_master = ep_rank_loc==0;

    void* local_recvbuf;

    if(is_master)
    {
      local_recvbuf = new void*[datasize * count];
    }

    MPI_Reduce_local(sendbuf, local_recvbuf, count, datatype, op, 0, comm);
    


    if(is_master)
    {
      ::MPI_Allreduce(local_recvbuf, recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), to_mpi_comm(comm.mpi_comm));
    }

    MPI_Bcast_local(recvbuf, count, datatype, 0, comm);


    if(is_master)
    {
      delete[] local_recvbuf;
    }

    MPI_Barrier_local(comm);
  }

  


}

