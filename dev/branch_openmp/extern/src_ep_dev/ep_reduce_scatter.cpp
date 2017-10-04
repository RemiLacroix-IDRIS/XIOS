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


namespace ep_lib {


  int MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, to_mpi_type(datatype), to_mpi_op(op), to_mpi_comm(comm.mpi_comm));
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

    int count = accumulate(recvcounts, recvcounts+ep_size, 0);
    if(is_master)
    {
      local_recvbuf = new void*[datasize * count]; 
    }

    MPI_Reduce_local(sendbuf, local_recvbuf, count, datatype, op, 0, comm);

    
    if(is_master)
    {  
      ::MPI_Allreduce(MPI_IN_PLACE, local_recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), to_mpi_comm(comm.mpi_comm));
    }


    // master have reduced data
    // local scatterv

    std::vector<int> local_recvcounts(num_ep, 0);
    std::vector<int>local_displs(num_ep, 0);

    int my_recvcount = recvcounts[ep_rank];
    MPI_Gather_local(&my_recvcount, 1, MPI_INT, local_recvcounts.data(), 0, comm);
    MPI_Bcast_local(local_recvcounts.data(), num_ep, MPI_INT, 0, comm);

    int my_displs = std::accumulate(recvcounts, recvcounts+ep_rank, 0);
    MPI_Gather_local(&my_displs, 1, MPI_INT, local_displs.data(), 0, comm);
    MPI_Bcast_local(local_displs.data(), num_ep, MPI_INT, 0, comm);

    

    MPI_Scatterv_local(local_recvbuf, local_recvcounts.data(), local_displs.data(), datatype, recvbuf, recvcounts[ep_rank], datatype, 0, comm);

    if(is_master)
    {
      delete[] local_recvbuf;
    }

  }

  
}

