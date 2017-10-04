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

  // int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
  // {

  //   if(!comm.is_ep && comm.mpi_comm)
  //   {
  //     ::MPI_Allgather(const_cast<void*>(sendbuf), sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, recvcount, static_cast< ::MPI_Datatype>(recvtype),
  //                     static_cast< ::MPI_Comm>(comm.mpi_comm));
  //     return 0;
  //   }

  //   if(!comm.mpi_comm) return 0;

  //   assert(sendcount == recvcount);

  //   assert(valid_type(sendtype) && valid_type(recvtype));

  //   MPI_Datatype datatype = sendtype;
  //   int count = sendcount;

  //   ::MPI_Aint datasize, lb;

  //   ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);


  //   int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
  //   int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
  //   int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
  //   int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
  //   int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
  //   int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

  //   bool is_master = ep_rank_loc==0;

  //   void* local_recvbuf;
  //   void* tmp_recvbuf;


  //   if(is_master)
  //   {
  //     local_recvbuf = new void*[datasize * num_ep * count];
  //     tmp_recvbuf = new void*[datasize * count * ep_size];
  //   }

  //   MPI_Gather_local(sendbuf, count, datatype, local_recvbuf, 0, comm);


  //   int* mpi_recvcounts;
  //   int *mpi_displs;
    
  //   if(is_master)
  //   {
      
  //     mpi_recvcounts = new int[mpi_size];
  //     mpi_displs = new int[mpi_size];

  //     int local_sendcount = num_ep * count;

  //     ::MPI_Allgather(&local_sendcount, 1, to_mpi_type(MPI_INT), mpi_recvcounts, 1, to_mpi_type(MPI_INT), to_mpi_comm(comm.mpi_comm));

  //     mpi_displs[0] = 0;
  //     for(int i=1; i<mpi_size; i++)
  //     {
  //       mpi_displs[i] = mpi_displs[i-1] + mpi_recvcounts[i-1];
  //     }

    
  //     ::MPI_Allgatherv(local_recvbuf, num_ep * count, to_mpi_type(datatype), tmp_recvbuf, mpi_recvcounts, mpi_displs, to_mpi_type(datatype), to_mpi_comm(comm.mpi_comm));


  //     // reorder 
  //     int offset;
  //     for(int i=0; i<ep_size; i++)
  //     {
  //       offset = mpi_displs[comm.rank_map->at(i).second] + comm.rank_map->at(i).first * sendcount; 
  //       memcpy(recvbuf + i*sendcount*datasize, tmp_recvbuf+offset*datasize, sendcount*datasize);
  //     }

  //     delete[] mpi_recvcounts;
  //     delete[] mpi_displs;
  //   }

  //   MPI_Bcast_local(recvbuf, count*ep_size, datatype, 0, comm);

  //   MPI_Barrier(comm);


  //   if(is_master)
  //   {
  //     delete[] local_recvbuf;
  //     delete[] tmp_recvbuf;

  //   }

  // }

  int MPI_Gather_local2(const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf, MPI_Comm comm)
  {
    if(datatype == MPI_INT)
    {
      Debug("datatype is INT\n");
      return MPI_Gather_local_int(sendbuf, count, recvbuf, comm);
    }
    else if(datatype == MPI_FLOAT)
    {
      Debug("datatype is FLOAT\n");
      return MPI_Gather_local_float(sendbuf, count, recvbuf, comm);
    }
    else if(datatype == MPI_DOUBLE)
    {
      Debug("datatype is DOUBLE\n");
      return MPI_Gather_local_double(sendbuf, count, recvbuf, comm);
    }
    else if(datatype == MPI_LONG)
    {
      Debug("datatype is LONG\n");
      return MPI_Gather_local_long(sendbuf, count, recvbuf, comm);
    }
    else if(datatype == MPI_UNSIGNED_LONG)
    {
      Debug("datatype is uLONG\n");
      return MPI_Gather_local_ulong(sendbuf, count, recvbuf, comm);
    }
    else if(datatype == MPI_CHAR)
    {
      Debug("datatype is CHAR\n");
      return MPI_Gather_local_char(sendbuf, count, recvbuf, comm);
    }
    else
    {
      printf("MPI_Gather Datatype not supported!\n");
      exit(0);
    }
  }

  int MPI_Gather_local_int(const void *sendbuf, int count, void *recvbuf, MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    int *buffer = comm.my_buffer->buf_int;
    int *send_buf = static_cast<int*>(const_cast<void*>(sendbuf));
    int *recv_buf = static_cast<int*>(recvbuf);

    if(my_rank == 0)
    {
      copy(send_buf, send_buf+count, recv_buf);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf+j+min(BUFFER_SIZE, count-j), buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, count-j), recv_buf+j+k*count);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gather_local_float(const void *sendbuf, int count, void *recvbuf, MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    float *buffer = comm.my_buffer->buf_float;
    float *send_buf = static_cast<float*>(const_cast<void*>(sendbuf));
    float *recv_buf = static_cast<float*>(recvbuf);

    if(my_rank == 0)
    {
      copy(send_buf, send_buf+count, recv_buf);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf+j+min(BUFFER_SIZE, count-j), buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, count-j), recv_buf+j+k*count);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gather_local_double(const void *sendbuf, int count, void *recvbuf, MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    double *buffer = comm.my_buffer->buf_double;
    double *send_buf = static_cast<double*>(const_cast<void*>(sendbuf));
    double *recv_buf = static_cast<double*>(recvbuf);

    if(my_rank == 0)
    {
      copy(send_buf, send_buf+count, recv_buf);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf+j+min(BUFFER_SIZE, count-j), buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, count-j), recv_buf+j+k*count);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gather_local_long(const void *sendbuf, int count, void *recvbuf, MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    long *buffer = comm.my_buffer->buf_long;
    long *send_buf = static_cast<long*>(const_cast<void*>(sendbuf));
    long *recv_buf = static_cast<long*>(recvbuf);

    if(my_rank == 0)
    {
      copy(send_buf, send_buf+count, recv_buf);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf+j+min(BUFFER_SIZE, count-j), buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, count-j), recv_buf+j+k*count);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }

  int MPI_Gather_local_ulong(const void *sendbuf, int count, void *recvbuf, MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    unsigned long *buffer = comm.my_buffer->buf_ulong;
    unsigned long *send_buf = static_cast<unsigned long*>(const_cast<void*>(sendbuf));
    unsigned long *recv_buf = static_cast<unsigned long*>(recvbuf);

    if(my_rank == 0)
    {
      copy(send_buf, send_buf+count, recv_buf);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf+j+min(BUFFER_SIZE, count-j), buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, count-j), recv_buf+j+k*count);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }


  int MPI_Gather_local_char(const void *sendbuf, int count, void *recvbuf, MPI_Comm comm)
  {
    int my_rank = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep  = comm.ep_comm_ptr->size_rank_info[1].second;

    char *buffer = comm.my_buffer->buf_char;
    char *send_buf = static_cast<char*>(const_cast<void*>(sendbuf));
    char *recv_buf = static_cast<char*>(recvbuf);

    if(my_rank == 0)
    {
      copy(send_buf, send_buf+count, recv_buf);
    }

    for(int j=0; j<count; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            copy(send_buf+j, send_buf+j+min(BUFFER_SIZE, count-j), buffer);
            #pragma omp flush
          }
        }

        MPI_Barrier_local(comm);

        if(my_rank == 0)
        {
          #pragma omp flush
          #pragma omp critical (read_from_buffer)
          {
            copy(buffer, buffer+min(BUFFER_SIZE, count-j), recv_buf+j+k*count);
          }
        }

        MPI_Barrier_local(comm);
      }
    }
  }



  int MPI_Gather2(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
    if(!comm.is_ep && comm.mpi_comm)
    {
      ::MPI_Gather(const_cast<void*>(sendbuf), sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, recvcount, static_cast< ::MPI_Datatype>(recvtype),
                   root, static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
    }

    if(!comm.mpi_comm) return 0;
    
    MPI_Bcast(&recvcount, 1, MPI_INT, root, comm);

    assert(static_cast< ::MPI_Datatype>(sendtype) == static_cast< ::MPI_Datatype>(recvtype) && sendcount == recvcount);

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


    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_loc = comm.rank_map->at(root).first;


    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    void *local_gather_recvbuf;
    void *master_recvbuf;
    if(ep_rank_loc == 0 && mpi_rank == root_mpi_rank && root_ep_loc != 0) 
    {
      master_recvbuf = new void*[datasize*ep_size*count];
    }

    if(ep_rank_loc==0)
    {
      local_gather_recvbuf = new void*[datasize*num_ep*count];
    }

    // local gather to master
    MPI_Gather_local2(sendbuf, count, datatype, local_gather_recvbuf, comm);

    //MPI_Gather

    if(ep_rank_loc == 0)
    {
      int *gatherv_recvcnt;
      int *gatherv_displs;
      int gatherv_cnt = count*num_ep;

      gatherv_recvcnt = new int[mpi_size];
      gatherv_displs = new int[mpi_size];


      ::MPI_Allgather(&gatherv_cnt, 1, MPI_INT, gatherv_recvcnt, 1, MPI_INT, static_cast< ::MPI_Comm>(comm.mpi_comm));

      gatherv_displs[0] = 0;
      for(int i=1; i<mpi_size; i++)
      {
        gatherv_displs[i] = gatherv_recvcnt[i-1] + gatherv_displs[i-1];
      }

      if(root_ep_loc != 0) // gather to root_master
      {
        ::MPI_Gatherv(local_gather_recvbuf, count*num_ep, static_cast< ::MPI_Datatype>(datatype), master_recvbuf, gatherv_recvcnt,
                    gatherv_displs, static_cast< ::MPI_Datatype>(datatype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      }
      else
      {
        ::MPI_Gatherv(local_gather_recvbuf, count*num_ep, static_cast< ::MPI_Datatype>(datatype), recvbuf, gatherv_recvcnt,
                    gatherv_displs, static_cast< ::MPI_Datatype>(datatype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      }

      delete[] gatherv_recvcnt;
      delete[] gatherv_displs;
    }


    if(root_ep_loc != 0 && mpi_rank == root_mpi_rank) // root is not master, master send to root and root receive from master
    {
      innode_memcpy(0, master_recvbuf, root_ep_loc, recvbuf, count*ep_size, datatype, comm);
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
      else if(datatype == MPI_CHAR)
      {
        delete[] static_cast<char*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_LONG)
      {
        delete[] static_cast<long*>(local_gather_recvbuf);
      }
      else// if(datatype == MPI_UNSIGNED_LONG)
      {
        delete[] static_cast<unsigned long*>(local_gather_recvbuf);
      }
      
      if(root_ep_loc != 0 && mpi_rank == root_mpi_rank) delete[] master_recvbuf;
    }
  }


  int MPI_Allgather2(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
  {
    if(!comm.is_ep && comm.mpi_comm)
    {
      ::MPI_Allgather(const_cast<void*>(sendbuf), sendcount, static_cast< ::MPI_Datatype>(sendtype), recvbuf, recvcount, static_cast< ::MPI_Datatype>(recvtype),
                      static_cast< ::MPI_Comm>(comm.mpi_comm));
      return 0;
    }

    if(!comm.mpi_comm) return 0;

    assert(static_cast< ::MPI_Datatype>(sendtype) == static_cast< ::MPI_Datatype>(recvtype) && sendcount == recvcount);

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


    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    void *local_gather_recvbuf;

    if(ep_rank_loc==0)
    {
      local_gather_recvbuf = new void*[datasize*num_ep*count];
    }

    // local gather to master
    MPI_Gather_local2(sendbuf, count, datatype, local_gather_recvbuf, comm);

    //MPI_Gather

    if(ep_rank_loc == 0)
    {
      int *gatherv_recvcnt;
      int *gatherv_displs;
      int gatherv_cnt = count*num_ep;

      gatherv_recvcnt = new int[mpi_size];
      gatherv_displs = new int[mpi_size];

      ::MPI_Allgather(&gatherv_cnt, 1, MPI_INT, gatherv_recvcnt, 1, MPI_INT, static_cast< ::MPI_Comm>(comm.mpi_comm));

      gatherv_displs[0] = 0;
      for(int i=1; i<mpi_size; i++)
      {
        gatherv_displs[i] = gatherv_recvcnt[i-1] + gatherv_displs[i-1];
      }

      ::MPI_Allgatherv(local_gather_recvbuf, count*num_ep, static_cast< ::MPI_Datatype>(datatype), recvbuf, gatherv_recvcnt,
                    gatherv_displs, static_cast< ::MPI_Datatype>(datatype), static_cast< ::MPI_Comm>(comm.mpi_comm));

      delete[] gatherv_recvcnt;
      delete[] gatherv_displs;
    }

    MPI_Bcast_local2(recvbuf, count*ep_size, datatype, comm);


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
      else if(datatype == MPI_CHAR)
      {
        delete[] static_cast<char*>(local_gather_recvbuf);
      }
      else if(datatype == MPI_LONG)
      {
        delete[] static_cast<long*>(local_gather_recvbuf);
      }
      else// if(datatype == MPI_UNSIGNED_LONG)
      {
        delete[] static_cast<unsigned long*>(local_gather_recvbuf);
      }
    }
  }


}
