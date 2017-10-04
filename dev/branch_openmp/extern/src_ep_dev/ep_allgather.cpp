/*!
   \file ep_gather.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Gather, MPI_Allgather
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include <string.h>
#include "ep_mpi.hpp"

using namespace std;

namespace ep_lib
{

  int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
  {

    if(!comm.is_ep && comm.mpi_comm)
    {
      ::MPI_Allgather(const_cast<void*>(sendbuf), sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, recvcount, static_cast< ::MPI_Datatype>(recvtype),
                      static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
    }

    if(!comm.mpi_comm) return 0;

    assert(sendcount == recvcount);

    assert(valid_type(sendtype) && valid_type(recvtype));

    MPI_Datatype datatype = sendtype;
    int count = sendcount;

    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);


    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

    bool is_master = ep_rank_loc==0;

    void* local_recvbuf;
    void* tmp_recvbuf;


    if(is_master)
    {
      local_recvbuf = new void*[datasize * num_ep * count];
      tmp_recvbuf = new void*[datasize * count * ep_size];
    }

    MPI_Gather_local(sendbuf, count, datatype, local_recvbuf, 0, comm);


    int* mpi_recvcounts;
    int *mpi_displs;
    
    if(is_master)
    {
      
      mpi_recvcounts = new int[mpi_size];
      mpi_displs = new int[mpi_size];

      int local_sendcount = num_ep * count;

      ::MPI_Allgather(&local_sendcount, 1, to_mpi_type(MPI_INT), mpi_recvcounts, 1, to_mpi_type(MPI_INT), to_mpi_comm(comm.mpi_comm));

      mpi_displs[0] = 0;
      for(int i=1; i<mpi_size; i++)
      {
        mpi_displs[i] = mpi_displs[i-1] + mpi_recvcounts[i-1];
      }

    
      ::MPI_Allgatherv(local_recvbuf, num_ep * count, to_mpi_type(datatype), tmp_recvbuf, mpi_recvcounts, mpi_displs, to_mpi_type(datatype), to_mpi_comm(comm.mpi_comm));


      // reorder 
      int offset;
      for(int i=0; i<ep_size; i++)
      {
        offset = mpi_displs[comm.rank_map->at(i).second] + comm.rank_map->at(i).first * sendcount; 
        memcpy(recvbuf + i*sendcount*datasize, tmp_recvbuf+offset*datasize, sendcount*datasize);
      }

      delete[] mpi_recvcounts;
      delete[] mpi_displs;
    }

    MPI_Bcast_local(recvbuf, count*ep_size, datatype, 0, comm);

    MPI_Barrier(comm);


    if(is_master)
    {
      delete[] local_recvbuf;
      delete[] tmp_recvbuf;

    }

  }



}
