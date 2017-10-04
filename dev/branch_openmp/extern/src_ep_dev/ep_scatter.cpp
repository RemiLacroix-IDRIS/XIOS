/*!
   \file ep_gather.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Scatter
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;

namespace ep_lib
{

  int MPI_Scatter_local(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int local_root, MPI_Comm comm)
  {
    assert(valid_type(sendtype) && valid_type(recvtype));
    assert(recvcount == sendcount);

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(sendtype), &lb, &datasize);

    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;


    if(ep_rank_loc == local_root)
      comm.my_buffer->void_buffer[local_root] = const_cast<void*>(sendbuf);

    MPI_Barrier_local(comm);

    #pragma omp critical (_scatter)      
    memcpy(recvbuf, comm.my_buffer->void_buffer[local_root]+datasize*ep_rank_loc*sendcount, datasize * recvcount);
    

    MPI_Barrier_local(comm);
  }

  int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Scatter(sendbuf, sendcount, to_mpi_type(sendtype), recvbuf, recvcount, to_mpi_type(recvtype), root, to_mpi_comm(comm.mpi_comm));
    }
   
    assert(sendcount == recvcount);

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
    int count = sendcount;

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
    std::vector<int> displs(mpi_size, 0);


    if(is_master)
    {
      for(int i=0; i<ep_size; i++)
      {
        recvcounts[comm.rank_map->at(i).second]++;
      }

      for(int i=1; i<mpi_size; i++)
        displs[i] = displs[i-1] + recvcounts[i-1];

      ::MPI_Gatherv(local_ranks.data(), num_ep, MPI_INT, ranks.data(), recvcounts.data(), displs.data(), MPI_INT, root_mpi_rank, to_mpi_comm(comm.mpi_comm));
    }



    // if(is_root) printf("\nranks = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", ranks[0], ranks[1], ranks[2], ranks[3], ranks[4], ranks[5], ranks[6], ranks[7], 
    //                                                                                   ranks[8], ranks[9], ranks[10], ranks[11], ranks[12], ranks[13], ranks[14], ranks[15]);

    if(is_root)
    for(int i=0; i<ep_size; i++)
    {
      memcpy(tmp_sendbuf + i*datasize*count, sendbuf + ranks[i]*datasize*count, count*datasize);
    }

    // if(is_root) printf("\ntmp_sendbuf = %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", static_cast<int*>(tmp_sendbuf)[0], static_cast<int*>(tmp_sendbuf)[2], static_cast<int*>(tmp_sendbuf)[4], static_cast<int*>(tmp_sendbuf)[6], 
    //                                                                           static_cast<int*>(tmp_sendbuf)[8], static_cast<int*>(tmp_sendbuf)[10], static_cast<int*>(tmp_sendbuf)[12], static_cast<int*>(tmp_sendbuf)[14], 
    //                                                                           static_cast<int*>(tmp_sendbuf)[16], static_cast<int*>(tmp_sendbuf)[18], static_cast<int*>(tmp_sendbuf)[20], static_cast<int*>(tmp_sendbuf)[22], 
    //                                                                           static_cast<int*>(tmp_sendbuf)[24], static_cast<int*>(tmp_sendbuf)[26], static_cast<int*>(tmp_sendbuf)[28], static_cast<int*>(tmp_sendbuf)[30] );


    // MPI_Scatterv from root to masters
    void* local_recvbuf;
    if(is_master) local_recvbuf = new void*[datasize * num_ep * count];


    if(is_master)
    {
      int local_sendcount = num_ep * count;
      ::MPI_Gather(&local_sendcount, 1, to_mpi_type(MPI_INT), recvcounts.data(), 1, to_mpi_type(MPI_INT), root_mpi_rank, to_mpi_comm(comm.mpi_comm));
      
      if(is_root) for(int i=1; i<mpi_size; i++) displs[i] = displs[i-1] + recvcounts[i-1];

      ::MPI_Scatterv(tmp_sendbuf, recvcounts.data(), displs.data(), to_mpi_type(sendtype), local_recvbuf, num_ep*count, to_mpi_type(recvtype), root_mpi_rank, to_mpi_comm(comm.mpi_comm));

      // printf("local_recvbuf = %d %d %d %d\n", static_cast<int*>(local_recvbuf)[0], static_cast<int*>(local_recvbuf)[1], static_cast<int*>(local_recvbuf)[2], static_cast<int*>(local_recvbuf)[3]);
                                                          // static_cast<int*>(local_recvbuf)[4], static_cast<int*>(local_recvbuf)[5], static_cast<int*>(local_recvbuf)[6], static_cast<int*>(local_recvbuf)[7]);
    }

    if(mpi_rank == root_mpi_rank) MPI_Scatter_local(local_recvbuf, count, sendtype, recvbuf, recvcount, recvtype, root_ep_loc, comm);
    else                          MPI_Scatter_local(local_recvbuf, count, sendtype, recvbuf, recvcount, recvtype, 0, comm);

    if(is_root)   delete[] tmp_sendbuf;
    if(is_master) delete[] local_recvbuf;
  }

}
