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

  int MPI_Gatherv_local(const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf, const int recvcounts[], const int displs[], int local_root, MPI_Comm comm)
  {
    assert(valid_type(datatype));

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);

    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;

    //if(ep_rank_loc == local_root) printf("local_gatherv : recvcounts = %d %d\n\n", recvcounts[0], recvcounts[1]);
    //if(ep_rank_loc == local_root) printf("local_gatherv : displs = %d %d\n\n", displs[0], displs[1]);

    #pragma omp critical (_gatherv)
    comm.my_buffer->void_buffer[ep_rank_loc] = const_cast< void* >(sendbuf);

    MPI_Barrier_local(comm);

    if(ep_rank_loc == local_root)
    {
      for(int i=0; i<num_ep; i++)
        memcpy(recvbuf + datasize*displs[i], comm.my_buffer->void_buffer[i], datasize*recvcounts[i]);

    }

    MPI_Barrier_local(comm);
  }

  int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int input_recvcounts[], const int input_displs[],
                  MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
  
    if(!comm.is_ep)
    {
      ::MPI_Gatherv(const_cast<void*>(sendbuf), sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, const_cast<int*>(input_recvcounts), const_cast<int*>(input_displs),
                    static_cast< ::MPI_Datatype>(recvtype), root, static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
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

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(sendtype), &lb, &datasize);

    int *recvcounts;
    int* displs;

    recvcounts = new int[ep_size];
    displs = new int[ep_size];


    bool is_master = (ep_rank_loc==0 && mpi_rank != root_mpi_rank ) || ep_rank == root;
    bool is_root = ep_rank == root;

    void* local_recvbuf;
    std::vector<int>local_recvcounts(num_ep, 0);
    std::vector<int>local_displs(num_ep, 0);


    if(is_root)
    { 
      copy(input_recvcounts, input_recvcounts+ep_size, recvcounts);
      copy(input_displs, input_displs+ep_size, displs);
    }

    MPI_Bcast(recvcounts, ep_size, MPI_INT, root, comm);
    MPI_Bcast(displs, ep_size, MPI_INT, root, comm);

    if(mpi_rank == root_mpi_rank) MPI_Gather_local(&sendcount, 1, MPI_INT, local_recvcounts.data(), root_ep_loc, comm);
    else                          MPI_Gather_local(&sendcount, 1, MPI_INT, local_recvcounts.data(), 0, comm);



    if(is_master)
    {
      int local_recvbuf_size = std::accumulate(local_recvcounts.begin(), local_recvcounts.end(), 0);
      
      for(int i=1; i<num_ep; i++)
        local_displs[i] = local_displs[i-1] + local_recvcounts[i-1];

      local_recvbuf = new void*[datasize * local_recvbuf_size];
    }

    if(mpi_rank == root_mpi_rank) MPI_Gatherv_local(sendbuf, sendcount, sendtype, local_recvbuf, local_recvcounts.data(), local_displs.data(), root_ep_loc, comm);
    else                          MPI_Gatherv_local(sendbuf, sendcount, sendtype, local_recvbuf, local_recvcounts.data(), local_displs.data(), 0, comm);

    //if(is_master) printf("local_recvbuf = %d %d %d %d\n", static_cast<int*>(local_recvbuf)[0], static_cast<int*>(local_recvbuf)[1], static_cast<int*>(local_recvbuf)[2], static_cast<int*>(local_recvbuf)[3]);

    void* tmp_recvbuf;
    int tmp_recvbuf_size = std::accumulate(recvcounts, recvcounts+ep_size, 0);

    if(is_root) tmp_recvbuf = new void*[datasize * tmp_recvbuf_size];


    std::vector<int> mpi_recvcounts(mpi_size, 0);
    std::vector<int> mpi_displs(mpi_size, 0);


    if(is_master)
    {
      for(int i=0; i<ep_size; i++)
      {
        mpi_recvcounts[comm.rank_map->at(i).second]+=recvcounts[i];
      }



      for(int i=1; i<mpi_size; i++)
        mpi_displs[i] = mpi_displs[i-1] + mpi_recvcounts[i-1];



      ::MPI_Gatherv(local_recvbuf, sendcount*num_ep, sendtype, tmp_recvbuf, mpi_recvcounts.data(), mpi_displs.data(), recvtype, root_mpi_rank, to_mpi_comm(comm.mpi_comm));
    }   


    // reorder data
    if(is_root)
    {
      // printf("tmp_recvbuf =\n");
      // for(int i=0; i<ep_size*sendcount; i++) printf("%d\t", static_cast<int*>(tmp_recvbuf)[i]);
      // printf("\n");

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

        //printf("recvbuf[%d] = tmp_recvbuf[%d] \n", i, offset);
        
      }

      // printf("recvbuf =\n");
      // for(int i=0; i<ep_size*sendcount; i++) printf("%d\t", static_cast<int*>(recvbuf)[i]);
      // printf("\n");

    }

    delete[] recvcounts;
    delete[] displs;

    if(is_master)
    {
      delete[] local_recvbuf;
    }
    if(is_root) delete[] tmp_recvbuf;
  }

}
