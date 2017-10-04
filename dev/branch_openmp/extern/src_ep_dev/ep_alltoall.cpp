#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_mpi.hpp"


namespace ep_lib
{

  int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Alltoall(sendbuf, sendcount, to_mpi_type(sendtype), recvbuf, recvcount, to_mpi_type(recvtype), to_mpi_comm(comm.mpi_comm));
    }


    assert(valid_type(sendtype) && valid_type(recvtype));
    assert(sendcount == recvcount);

    ::MPI_Aint datasize, llb;
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(sendtype), &llb, &datasize);

    int count = sendcount;
    
    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

    void* tmp_recvbuf;
    if(ep_rank == 0) tmp_recvbuf = new void*[count * ep_size * ep_size * datasize];

    MPI_Gather(sendbuf, count*ep_size, sendtype, tmp_recvbuf, count*ep_size, recvtype, 0, comm);

    
    
    // reorder tmp_buf
    void* tmp_sendbuf;
    if(ep_rank == 0) tmp_sendbuf = new void*[count * ep_size * ep_size * datasize];

    if(ep_rank == 0)
    for(int i=0; i<ep_size; i++)
    {
      for(int j=0; j<ep_size; j++)
      {
        //printf("tmp_recv[%d] = tmp_send[%d]\n", i*ep_size*count + j*count, j*ep_size*count + i*count);

        memcpy(tmp_sendbuf + j*ep_size*count*datasize + i*count*datasize, tmp_recvbuf + i*ep_size*count*datasize + j*count*datasize, count*datasize);
      }
    }

    MPI_Scatter(tmp_sendbuf, ep_size*count, sendtype, recvbuf, ep_size*recvcount, recvtype, 0, comm);

    if(ep_rank == 0)
    {
      delete[] tmp_recvbuf;
      delete[] tmp_sendbuf;
    }

    return 0;
  }

}


