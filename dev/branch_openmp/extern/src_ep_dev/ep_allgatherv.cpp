/*!
   \file ep_gather.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Gatherv, MPI_Allgatherv
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"


using namespace std;

namespace ep_lib
{

  int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm)
  {

    if(!comm.is_ep && comm.mpi_comm)
    {
      ::MPI_Allgatherv(sendbuf, sendcount, to_mpi_type(sendtype), recvbuf, recvcounts, displs, to_mpi_type(recvtype), to_mpi_comm(comm.mpi_comm));
      return 0;
    }

    if(!comm.mpi_comm) return 0;


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

    assert(sendcount == recvcounts[ep_rank]);

    bool is_master = ep_rank_loc==0;

    void* local_recvbuf;
    void* tmp_recvbuf;

    int recvbuf_size = 0;
    for(int i=0; i<ep_size; i++)
      recvbuf_size = max(recvbuf_size, displs[i]+recvcounts[i]);


    vector<int>local_recvcounts(num_ep, 0);
    vector<int>local_displs(num_ep, 0);

    MPI_Gather_local(&sendcount, 1, MPI_INT, local_recvcounts.data(), 0, comm);
    for(int i=1; i<num_ep; i++) local_displs[i] = local_displs[i-1] + local_recvcounts[i]; 


    if(is_master)
    {
      local_recvbuf = new void*[datasize * num_ep * count];
      tmp_recvbuf = new void*[datasize * std::accumulate(recvcounts, recvcounts+ep_size, 0)];
    }

    MPI_Gatherv_local(sendbuf, count, datatype, local_recvbuf, local_recvcounts.data(), local_displs.data(), 0, comm);

    


    if(is_master)
    {
      std::vector<int>mpi_recvcounts(mpi_size, 0);
      std::vector<int>mpi_displs(mpi_size, 0);

      int local_sendcount = num_ep * count;
      MPI_Allgather(&local_sendcount, 1, MPI_INT, mpi_recvcounts.data(), 1, MPI_INT, to_mpi_comm(comm.mpi_comm));

      for(int i=1; i<mpi_size; i++)
        mpi_displs[i] = mpi_displs[i-1] + mpi_recvcounts[i-1];

      // if(ep_rank_loc == 0)
      // { 
      //   printf("local_recvbuf =\n");
      //   for(int i=0; i<num_ep*sendcount; i++) printf("%d\t", static_cast<int*>(local_recvbuf)[i]);
      //   printf("\n");
      // }

      // printf("mpi_recvcounts = %d %d %d\n", mpi_recvcounts[0], mpi_recvcounts[1], mpi_recvcounts[2]);
      // printf("mpi_displs = %d %d %d\n", mpi_displs[0], mpi_displs[1], mpi_displs[2]);
      


      ::MPI_Allgatherv(local_recvbuf, num_ep * count, to_mpi_type(datatype), tmp_recvbuf, mpi_recvcounts.data(), mpi_displs.data(), to_mpi_type(datatype), to_mpi_comm(comm.mpi_comm));

      // if(ep_rank == 0)
      // { 
      //   printf("tmp_recvbuf =\n");
      //   for(int i=0; i<ep_size*sendcount; i++) printf("%d\t", static_cast<int*>(tmp_recvbuf)[i]);
      //   printf("\n");
      // }


      // reorder 
      int offset;
      for(int i=0; i<ep_size; i++)
      {
        int extra = 0;
        for(int j=0, k=0; j<ep_size, k<comm.rank_map->at(i).first; j++)
          if(comm.rank_map->at(i).second == comm.rank_map->at(j).second)
          {
            extra += recvcounts[j];
            k++;
          }  

        offset = mpi_displs[comm.rank_map->at(i).second] +  extra;

        memcpy(recvbuf+displs[i]*datasize, tmp_recvbuf+offset*datasize, recvcounts[i]*datasize);
        
      }
      
      // if(ep_rank == 0)
      // {
      //       printf("recvbuf[%d] =\n", recvbuf_size);
      //       for(int i=0; i<ep_size*sendcount; i++) printf("%d\t", static_cast<int*>(recvbuf)[i]);
      //       printf("\n");
      // }
    }

    MPI_Bcast_local(recvbuf, recvbuf_size, datatype, 0, comm);

    if(is_master)
    {
      delete[] local_recvbuf;
      delete[] tmp_recvbuf;
    }


  }



}
