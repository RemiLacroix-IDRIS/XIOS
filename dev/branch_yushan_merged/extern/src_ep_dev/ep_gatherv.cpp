/*!
   \file ep_gather.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Gatherv, MPI_Allgatherv
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"


using namespace std;

namespace ep_lib
{

  int MPI_Gatherv_local(const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    if(datatype == MPI_INT)
    {
      Debug("datatype is INT\n");
      return MPI_Gatherv_local_int(sendbuf, count, recvbuf, recvcounts, displs, comm);
    }
    else if(datatype == MPI_FLOAT)
    {
      Debug("datatype is FLOAT\n");
      return MPI_Gatherv_local_float(sendbuf, count, recvbuf, recvcounts, displs, comm);
    }
    else if(datatype == MPI_DOUBLE)
    {
      Debug("datatype is DOUBLE\n");
      return MPI_Gatherv_local_double(sendbuf, count, recvbuf, recvcounts, displs, comm);
    }
    else if(datatype == MPI_LONG)
    {
      Debug("datatype is LONG\n");
      return MPI_Gatherv_local_long(sendbuf, count, recvbuf, recvcounts, displs, comm);
    }
    else if(datatype == MPI_UNSIGNED_LONG)
    {
      Debug("datatype is uLONG\n");
      return MPI_Gatherv_local_ulong(sendbuf, count, recvbuf, recvcounts, displs, comm);
    }
    else if(datatype == MPI_CHAR)
    {
      Debug("datatype is CHAR\n");
      return MPI_Gatherv_local_char(sendbuf, count, recvbuf, recvcounts, displs, comm);
    }
    else
    {
      printf("MPI_Gatherv Datatype not supported!\n");
      exit(0);
    }
  }

  int MPI_Gatherv_local_int(const void *sendbuf, int count, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    int *buffer = comm.my_buffer->buf_int;
    int *send_buf = static_cast<int*>(const_cast<void*>(sendbuf));
    int *recv_buf = static_cast<int*>(recvbuf);

    if(my_rank == 0)
    {
      assert(count == recvcounts[0]);
      copy(send_buf, send_buf+count, recv_buf + displs[0]);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, recvcounts[k]-j), recv_buf+j+displs[k]);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gatherv_local_float(const void *sendbuf, int count, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    float *buffer = comm.my_buffer->buf_float;
    float *send_buf = static_cast<float*>(const_cast<void*>(sendbuf));
    float *recv_buf = static_cast<float*>(recvbuf);

    if(my_rank == 0)
    {
      assert(count == recvcounts[0]);
      copy(send_buf, send_buf+count, recv_buf + displs[0]);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, recvcounts[k]-j), recv_buf+j+displs[k]);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gatherv_local_double(const void *sendbuf, int count, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    double *buffer = comm.my_buffer->buf_double;
    double *send_buf = static_cast<double*>(const_cast<void*>(sendbuf));
    double *recv_buf = static_cast<double*>(recvbuf);

    if(my_rank == 0)
    {
      assert(count == recvcounts[0]);
      copy(send_buf, send_buf+count, recv_buf + displs[0]);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, recvcounts[k]-j), recv_buf+j+displs[k]);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gatherv_local_long(const void *sendbuf, int count, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    long *buffer = comm.my_buffer->buf_long;
    long *send_buf = static_cast<long*>(const_cast<void*>(sendbuf));
    long *recv_buf = static_cast<long*>(recvbuf);

    if(my_rank == 0)
    {
      assert(count == recvcounts[0]);
      copy(send_buf, send_buf+count, recv_buf + displs[0]);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, recvcounts[k]-j), recv_buf+j+displs[k]);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gatherv_local_ulong(const void *sendbuf, int count, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    unsigned long *buffer = comm.my_buffer->buf_ulong;
    unsigned long *send_buf = static_cast<unsigned long*>(const_cast<void*>(sendbuf));
    unsigned long *recv_buf = static_cast<unsigned long*>(recvbuf);

    if(my_rank == 0)
    {
      assert(count == recvcounts[0]);
      copy(send_buf, send_buf+count, recv_buf + displs[0]);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, recvcounts[k]-j), recv_buf+j+displs[k]);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gatherv_local_char(const void *sendbuf, int count, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    char *buffer = comm.my_buffer->buf_char;
    char *send_buf = static_cast<char*>(const_cast<void*>(sendbuf));
    char *recv_buf = static_cast<char*>(recvbuf);

    if(my_rank == 0)
    {
      assert(count == recvcounts[0]);
      copy(send_buf, send_buf+count, recv_buf + displs[0]);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, recvcounts[k]-j), recv_buf+j+displs[k]);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }


  int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[],
                  MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
  
    if(!comm.is_ep && comm.mpi_comm)
    {
      ::MPI_Gatherv(const_cast<void*>(sendbuf), sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, const_cast<int*>(recvcounts), const_cast<int*>(displs),
                    static_cast< ::MPI_Datatype>(recvtype), root, static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
    }

    if(!comm.mpi_comm) return 0;

    assert(static_cast< ::MPI_Datatype>(sendtype) == static_cast< ::MPI_Datatype>(recvtype));

    MPI_Datatype datatype = sendtype;
    int count = sendcount;

    int ep_rank, ep_rank_loc, mpi_rank;
    int ep_size, num_ep, mpi_size;

    ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;
    
    if(ep_size == mpi_size) 
      return ::MPI_Gatherv(sendbuf, sendcount, static_cast< ::MPI_Datatype>(datatype), recvbuf, recvcounts, displs,
                              static_cast< ::MPI_Datatype>(datatype), root, static_cast< ::MPI_Comm>(comm.mpi_comm));

    int recv_plus_displs[ep_size];
    for(int i=0; i<ep_size; i++) recv_plus_displs[i] = recvcounts[i] + displs[i];
    
    #pragma omp single nowait
    {
      assert(recv_plus_displs[ep_rank-ep_rank_loc] >= displs[ep_rank-ep_rank_loc+1]);
      for(int i=1; i<num_ep-1; i++)
      {
        assert(recv_plus_displs[ep_rank-ep_rank_loc+i] >= displs[ep_rank-ep_rank_loc+i+1]);
        assert(recv_plus_displs[ep_rank-ep_rank_loc+i] >= displs[ep_rank-ep_rank_loc+i-1]);
      }
      assert(recv_plus_displs[ep_rank-ep_rank_loc+num_ep-1] >= displs[ep_rank-ep_rank_loc+num_ep-2]);
    }

    if(ep_rank != root)
    {
      recvcounts = new int[ep_size];
      displs = new int[ep_size];
    }
    
    MPI_Bcast(const_cast< int* >(recvcounts), ep_size, MPI_INT, root, comm);
    MPI_Bcast(const_cast< int* >(displs), ep_size, MPI_INT, root, comm);


    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_loc = comm.rank_map->at(root).first;


    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    void *local_gather_recvbuf;
    int buffer_size;

    if(ep_rank_loc==0)
    {
      buffer_size = *std::max_element(recv_plus_displs+ep_rank, recv_plus_displs+ep_rank+num_ep);

      local_gather_recvbuf = new void*[datasize*buffer_size];
    }

    MPI_Gatherv_local(sendbuf, count, datatype, local_gather_recvbuf, recvcounts+ep_rank-ep_rank_loc, displs+ep_rank-ep_rank_loc, comm);

    //MPI_Gather
    if(ep_rank_loc == 0)
    {
      int *mpi_recvcnt= new int[mpi_size];
      int *mpi_displs= new int[mpi_size];

      int buff_start = *std::min_element(displs+ep_rank, displs+ep_rank+num_ep);;
      int buff_end = buffer_size;

      int mpi_sendcnt = buff_end - buff_start;


      ::MPI_Gather(&mpi_sendcnt, 1, MPI_INT_STD, mpi_recvcnt, 1, MPI_INT_STD, root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      ::MPI_Gather(&buff_start,  1, MPI_INT_STD, mpi_displs,  1, MPI_INT_STD, root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));


      ::MPI_Gatherv(local_gather_recvbuf + datasize*buff_start, mpi_sendcnt, static_cast< ::MPI_Datatype>(datatype), recvbuf, mpi_recvcnt,
                       mpi_displs, static_cast< ::MPI_Datatype>(datatype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));

      delete[] mpi_recvcnt;
      delete[] mpi_displs;
    }

    int global_min_displs = *std::min_element(displs, displs+ep_size);
    int global_recvcnt = *std::max_element(recv_plus_displs, recv_plus_displs+ep_size);


    if(root_ep_loc != 0 && mpi_rank == root_mpi_rank) // root is not master, master send to root and root receive from master
    {
      innode_memcpy(0, recvbuf+datasize*global_min_displs, root_ep_loc, recvbuf+datasize*global_min_displs, global_recvcnt, datatype, comm);
    }



    if(ep_rank_loc==0)
    {
      if(datatype == MPI_INT)
      {
        delete[] static_cast<int*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_FLOAT)
      {
        delete[] static_cast<float*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_DOUBLE)
      {
        delete[] static_cast<double*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_LONG)
      {
        delete[] static_cast<long*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_UNSIGNED_LONG)
      {
        delete[] static_cast<unsigned long*>(local_gather_recvbuf);
      }
      else // if(datatype == MPI_CHAR)
      {
        delete[] static_cast<char*>(local_gather_recvbuf);
      }
    }
    else
    {
      delete[] recvcounts;
      delete[] displs;
    }
    return 0;
  }



  int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[],
                  MPI_Datatype recvtype, MPI_Comm comm)
  {

    if(!comm.is_ep && comm.mpi_comm)
    {
      ::MPI_Allgatherv(sendbuf, sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, recvcounts, displs,
                       static_cast< ::MPI_Datatype>(recvtype), static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
    }

    if(!comm.mpi_comm) return 0;

    assert(static_cast< ::MPI_Datatype>(sendtype) == static_cast< ::MPI_Datatype>(recvtype));


    MPI_Datatype datatype = sendtype;
    int count = sendcount;

    int ep_rank, ep_rank_loc, mpi_rank;
    int ep_size, num_ep, mpi_size;

    ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;
    
    if(ep_size == mpi_size) 
      return ::MPI_Allgatherv(sendbuf, sendcount, static_cast< ::MPI_Datatype>(datatype), recvbuf, recvcounts, displs,
                              static_cast< ::MPI_Datatype>(datatype), static_cast< ::MPI_Comm>(comm.mpi_comm));
   

    int recv_plus_displs[ep_size];
    for(int i=0; i<ep_size; i++) recv_plus_displs[i] = recvcounts[i] + displs[i];

    #pragma omp single nowait
    {
      assert(recv_plus_displs[ep_rank-ep_rank_loc] >= displs[ep_rank-ep_rank_loc+1]);
      for(int i=1; i<num_ep-1; i++)
      {
        assert(recv_plus_displs[ep_rank-ep_rank_loc+i] >= displs[ep_rank-ep_rank_loc+i+1]);
        assert(recv_plus_displs[ep_rank-ep_rank_loc+i] >= displs[ep_rank-ep_rank_loc+i-1]);
      }
      assert(recv_plus_displs[ep_rank-ep_rank_loc+num_ep-1] >= displs[ep_rank-ep_rank_loc+num_ep-2]);
    }


    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    void *local_gather_recvbuf;
    int buffer_size;

    if(ep_rank_loc==0)
    {
      buffer_size = *std::max_element(recv_plus_displs+ep_rank, recv_plus_displs+ep_rank+num_ep);

      local_gather_recvbuf = new void*[datasize*buffer_size];
    }

    // local gather to master
    MPI_Gatherv_local(sendbuf, count, datatype, local_gather_recvbuf, recvcounts+ep_rank-ep_rank_loc, displs+ep_rank-ep_rank_loc, comm);

    //MPI_Gather
    if(ep_rank_loc == 0)
    {
      int *mpi_recvcnt= new int[mpi_size];
      int *mpi_displs= new int[mpi_size];

      int buff_start = *std::min_element(displs+ep_rank, displs+ep_rank+num_ep);;
      int buff_end = buffer_size;

      int mpi_sendcnt = buff_end - buff_start;


      ::MPI_Allgather(&mpi_sendcnt, 1, MPI_INT_STD, mpi_recvcnt, 1, MPI_INT_STD, static_cast< ::MPI_Comm>(comm.mpi_comm));
      ::MPI_Allgather(&buff_start,  1, MPI_INT_STD, mpi_displs,  1, MPI_INT_STD, static_cast< ::MPI_Comm>(comm.mpi_comm));


      ::MPI_Allgatherv((char*)local_gather_recvbuf + datasize*buff_start, mpi_sendcnt, static_cast< ::MPI_Datatype>(datatype), recvbuf, mpi_recvcnt,
                       mpi_displs, static_cast< ::MPI_Datatype>(datatype), static_cast< ::MPI_Comm>(comm.mpi_comm));

      delete[] mpi_recvcnt;
      delete[] mpi_displs;
    }

    int global_min_displs = *std::min_element(displs, displs+ep_size);
    int global_recvcnt = *std::max_element(recv_plus_displs, recv_plus_displs+ep_size);

    MPI_Bcast_local(recvbuf+datasize*global_min_displs, global_recvcnt, datatype, comm);

    if(ep_rank_loc==0)
    {
      if(datatype == MPI_INT)
      {
        delete[] static_cast<int*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_FLOAT)
      {
        delete[] static_cast<float*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_DOUBLE)
      {
        delete[] static_cast<double*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_LONG)
      {
        delete[] static_cast<long*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_UNSIGNED_LONG)
      {
        delete[] static_cast<unsigned long*>(local_gather_recvbuf);
      }
      else // if(datatype == MPI_CHAR)
      {
        delete[] static_cast<char*>(local_gather_recvbuf);
      }
    }
  }


}
