/*!
   \file ep_gather.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Scatterv
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;

namespace ep_lib
{

  int MPI_Scatterv_local(const void *sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount,
                   MPI_Datatype recvtype, int local_root, MPI_Comm comm)
  {

    assert(valid_type(sendtype) && valid_type(recvtype));

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(sendtype), &lb, &datasize);

    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;

    assert(recvcount == sendcounts[ep_rank_loc]);

    if(ep_rank_loc == local_root)
      comm.my_buffer->void_buffer[local_root] = const_cast<void*>(sendbuf);

    MPI_Barrier_local(comm);

    #pragma omp critical (_scatterv)      
    memcpy(recvbuf, comm.my_buffer->void_buffer[local_root]+datasize*displs[ep_rank_loc], datasize * recvcount);
    

    MPI_Barrier_local(comm);
  }

  int MPI_Scatterv(const void *sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount,
                   MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Scatterv(sendbuf, sendcounts, displs, to_mpi_type(sendtype), recvbuf, recvcount, to_mpi_type(recvtype), root, to_mpi_comm(comm.mpi_comm));
    }
   
    assert(sendtype == recvtype);

    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_loc = comm.rank_map->at(root).first;

    bool is_master = (ep_rank_loc==0 && mpi_rank != root_mpi_rank ) || ep_rank == root;
    bool is_root = ep_rank == root;

    MPI_Datatype datatype = sendtype;
    int count = recvcount;

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);
    
    void *tmp_sendbuf;
    if(is_root) tmp_sendbuf = new void*[ep_size * count * datasize];

    // reorder tmp_sendbuf
    std::vector<int>local_ranks(num_ep);
    std::vector<int>ranks(ep_size);

    if(mpi_rank == root_mpi_rank) MPI_Gather_local(&ep_rank, 1, MPI_INT, local_ranks.data(), root_ep_loc, comm);
    else                          MPI_Gather_local(&ep_rank, 1, MPI_INT, local_ranks.data(), 0, comm);


    std::vector<int> recvcounts(mpi_size, 0);
    std::vector<int> my_displs(mpi_size, 0);


    if(is_master)
    {
      for(int i=0; i<ep_size; i++)
      {
        recvcounts[comm.rank_map->at(i).second]++;
      }

      for(int i=1; i<mpi_size; i++)
        my_displs[i] = my_displs[i-1] + recvcounts[i-1];

      ::MPI_Gatherv(local_ranks.data(), num_ep, MPI_INT, ranks.data(), recvcounts.data(), my_displs.data(), MPI_INT, root_mpi_rank, to_mpi_comm(comm.mpi_comm));
    }


    if(is_root)
    {
      int local_displs = 0;
      for(int i=0; i<ep_size; i++)
      {
        //printf("i=%d : start from %d, src displs = %d, count = %d\n ", i, local_displs/datasize, displs[ranks[i]], sendcounts[ranks[i]]);
        memcpy(tmp_sendbuf+local_displs, sendbuf + displs[ranks[i]]*datasize, sendcounts[ranks[i]]*datasize);
        local_displs += sendcounts[ranks[i]]*datasize;
      }
      
      //for(int i=0; i<ep_size*2; i++) printf("%d\t", static_cast<int*>(const_cast<void*>(tmp_sendbuf))[i]);
    }

    // MPI_Scatterv from root to masters
  
    void* local_sendbuf;
    int local_sendcount;
    if(mpi_rank == root_mpi_rank) MPI_Reduce_local(&recvcount, &local_sendcount, 1, MPI_INT, MPI_SUM, root_ep_loc, comm);
    else                          MPI_Reduce_local(&recvcount, &local_sendcount, 1, MPI_INT, MPI_SUM, 0, comm);

    if(is_master) 
    {
      local_sendbuf = new void*[datasize * local_sendcount];
 
      ::MPI_Gather(&local_sendcount, 1, to_mpi_type(MPI_INT), recvcounts.data(), 1, to_mpi_type(MPI_INT), root_mpi_rank, to_mpi_comm(comm.mpi_comm));

      if(is_root) for(int i=1; i<mpi_size; i++) my_displs[i] = my_displs[i-1] + recvcounts[i-1];

      ::MPI_Scatterv(tmp_sendbuf, recvcounts.data(), my_displs.data(), to_mpi_type(sendtype), local_sendbuf, num_ep*count, to_mpi_type(recvtype), root_mpi_rank, to_mpi_comm(comm.mpi_comm));

      // printf("my_displs = %d %d %d %d\n", my_displs[0], my_displs[1], my_displs[2], my_displs[3] );

      // printf("%d %d %d %d %d %d %d %d\n", static_cast<int*>(local_sendbuf)[0], static_cast<int*>(local_sendbuf)[1], static_cast<int*>(local_sendbuf)[2], static_cast<int*>(local_sendbuf)[3],
      //                                     static_cast<int*>(local_sendbuf)[4], static_cast<int*>(local_sendbuf)[5], static_cast<int*>(local_sendbuf)[6], static_cast<int*>(local_sendbuf)[7]);
    }

    std::vector<int>local_sendcounts(num_ep, 0);
    std::vector<int>local_displs(num_ep, 0);

    MPI_Gather_local(&recvcount, 1, MPI_INT, local_sendcounts.data(), 0, comm);
    MPI_Bcast_local(local_sendcounts.data(), num_ep, MPI_INT, 0, comm);
    for(int i=1; i<num_ep; i++)
      local_displs[i] = local_displs[i-1] + local_sendcounts[i-1];
    

    if(mpi_rank == root_mpi_rank) MPI_Scatterv_local(local_sendbuf, local_sendcounts.data(), local_displs.data(), sendtype, recvbuf, recvcount, recvtype, root_ep_loc, comm);
    else                          MPI_Scatterv_local(local_sendbuf, local_sendcounts.data(), local_displs.data(), sendtype, recvbuf, recvcount, recvtype, 0, comm);


    if(is_root) delete[] tmp_sendbuf;
    if(is_master) delete[] local_sendbuf;
  }


}
