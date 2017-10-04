/*!
   \file ep_gather.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Gather, MPI_Allgather
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;

namespace ep_lib
{

  int MPI_Gather_local(const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf, int local_root, MPI_Comm comm)
  {
    assert(valid_type(datatype));

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);

    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;

    #pragma omp critical (_gather)
    comm.my_buffer->void_buffer[ep_rank_loc] = const_cast< void* >(sendbuf);

    MPI_Barrier_local(comm);

    if(ep_rank_loc == local_root)
    {
      for(int i=0; i<num_ep; i++)
        memcpy(recvbuf + datasize * i * count, comm.my_buffer->void_buffer[i], datasize * count);

      //printf("local_recvbuf = %d %d \n", static_cast<int*>(recvbuf)[0], static_cast<int*>(recvbuf)[1] );
    }

    MPI_Barrier_local(comm);
  }

  int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Gather(const_cast<void*>(sendbuf), sendcount, to_mpi_type(sendtype), recvbuf, recvcount, to_mpi_type(recvtype),
                   root, to_mpi_comm(comm.mpi_comm));
    }

    assert(sendcount == recvcount && sendtype == recvtype);

    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_loc = comm.rank_map->at(root).first;

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(sendtype), &lb, &datasize);

    bool is_master = (ep_rank_loc==0 && mpi_rank != root_mpi_rank ) || ep_rank == root;
    bool is_root = ep_rank == root;

    void* local_recvbuf;

    if(is_master)
    {
      local_recvbuf = new void*[datasize * num_ep * sendcount];
    }

    void* tmp_recvbuf;
    if(is_root) tmp_recvbuf = new void*[datasize * recvcount * ep_size];


    if(mpi_rank == root_mpi_rank) MPI_Gather_local(sendbuf, sendcount, sendtype, local_recvbuf, root_ep_loc, comm);
    else                          MPI_Gather_local(sendbuf, sendcount, sendtype, local_recvbuf, 0, comm);

    std::vector<int> recvcounts(mpi_size, 0);
    std::vector<int> displs(mpi_size, 0);


    if(is_master)
    {
      for(int i=0; i<ep_size; i++)
      {
        recvcounts[comm.rank_map->at(i).second]+=sendcount;
      }

      for(int i=1; i<mpi_size; i++)
        displs[i] = displs[i-1] + recvcounts[i-1];

      ::MPI_Gatherv(local_recvbuf, sendcount*num_ep, sendtype, tmp_recvbuf, recvcounts.data(), displs.data(), recvtype, root_mpi_rank, to_mpi_comm(comm.mpi_comm));
    }   


    // reorder data
    if(is_root)
    {
      // printf("tmp_recvbuf = %d %d %d %d %d %d %d %d\n", static_cast<int*>(tmp_recvbuf)[0], static_cast<int*>(tmp_recvbuf)[1],
      //                                                   static_cast<int*>(tmp_recvbuf)[2], static_cast<int*>(tmp_recvbuf)[3],
      //                                                   static_cast<int*>(tmp_recvbuf)[4], static_cast<int*>(tmp_recvbuf)[5],
      //                                                   static_cast<int*>(tmp_recvbuf)[6], static_cast<int*>(tmp_recvbuf)[7] );

      int offset;
      for(int i=0; i<ep_size; i++)
      {
        offset = displs[comm.rank_map->at(i).second] + comm.rank_map->at(i).first * sendcount; 
        memcpy(recvbuf + i*sendcount*datasize, tmp_recvbuf+offset*datasize, sendcount*datasize);


      }

    }


    if(is_master)
    {
      delete[] local_recvbuf;
    }
    if(is_root) delete[] tmp_recvbuf;
    
  }

}
