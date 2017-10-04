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

  // int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm)
  // {

  //   if(!comm.is_ep && comm.mpi_comm)
  //   {
  //     ::MPI_Allgatherv(sendbuf, sendcount, to_mpi_type(sendtype), recvbuf, recvcounts, displs, to_mpi_type(recvtype), to_mpi_comm(comm.mpi_comm));
  //     return 0;
  //   }

  //   if(!comm.mpi_comm) return 0;




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


  //   assert(sendcount == recvcounts[ep_rank]);

  //   bool is_master = ep_rank_loc==0;

  //   void* local_recvbuf;
  //   void* tmp_recvbuf;

  //   int recvbuf_size = 0;
  //   for(int i=0; i<ep_size; i++)
  //     recvbuf_size = max(recvbuf_size, displs[i]+recvcounts[i]);


  //   vector<int>local_recvcounts(num_ep, 0);
  //   vector<int>local_displs(num_ep, 0);

  //   MPI_Gather_local(&sendcount, 1, MPI_INT, local_recvcounts.data(), 0, comm);
  //   for(int i=1; i<num_ep; i++) local_displs[i] = local_displs[i-1] + local_recvcounts[i-1]; 


  //   if(is_master)
  //   {
  //     local_recvbuf = new void*[datasize * std::accumulate(local_recvcounts.begin(), local_recvcounts.begin()+num_ep, 0)];
  //     tmp_recvbuf = new void*[datasize * std::accumulate(recvcounts, recvcounts+ep_size, 0)];
  //   }

  //   MPI_Gatherv_local(sendbuf, count, datatype, local_recvbuf, local_recvcounts.data(), local_displs.data(), 0, comm);


  //   if(is_master)
  //   {
  //     std::vector<int>mpi_recvcounts(mpi_size, 0);
  //     std::vector<int>mpi_displs(mpi_size, 0);

  //     int local_sendcount = std::accumulate(local_recvcounts.begin(), local_recvcounts.begin()+num_ep, 0);
  //     MPI_Allgather(&local_sendcount, 1, MPI_INT, mpi_recvcounts.data(), 1, MPI_INT, to_mpi_comm(comm.mpi_comm));

  //     for(int i=1; i<mpi_size; i++)
  //       mpi_displs[i] = mpi_displs[i-1] + mpi_recvcounts[i-1];


  //     ::MPI_Allgatherv(local_recvbuf, local_sendcount, to_mpi_type(datatype), tmp_recvbuf, mpi_recvcounts.data(), mpi_displs.data(), to_mpi_type(datatype), to_mpi_comm(comm.mpi_comm));



  //     // reorder 
  //     int offset;
  //     for(int i=0; i<ep_size; i++)
  //     {
  //       int extra = 0;
  //       for(int j=0, k=0; j<ep_size, k<comm.rank_map->at(i).first; j++)
  //         if(comm.rank_map->at(i).second == comm.rank_map->at(j).second)
  //         {
  //           extra += recvcounts[j];
  //           k++;
  //         }  

  //       offset = mpi_displs[comm.rank_map->at(i).second] +  extra;

  //       memcpy(recvbuf+displs[i]*datasize, tmp_recvbuf+offset*datasize, recvcounts[i]*datasize);
        
  //     }

  //   }

  //   MPI_Bcast_local(recvbuf, recvbuf_size, datatype, 0, comm);

  //   if(is_master)
  //   {
  //     delete[] local_recvbuf;
  //     delete[] tmp_recvbuf;
  //   }

  // }


  int MPI_Gatherv_local2(const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Comm comm)
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

    for(int j=0; count!=0? j<count: j<count+1; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            if(count!=0) copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
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

    for(int j=0; count!=0? j<count: j<count+1; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            if(count!=0) copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
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

    for(int j=0; count!=0? j<count: j<count+1; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            if(count!=0) copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
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

    for(int j=0; count!=0? j<count: j<count+1; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            if(count!=0)copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
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

    for(int j=0; count!=0? j<count: j<count+1; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            if(count!=0) copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
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

    for(int j=0; count!=0? j<count: j<count+1; j+=BUFFER_SIZE)
    {
      for(int k=1; k<num_ep; k++)
      {
        if(my_rank == k)
        {
          #pragma omp critical (write_to_buffer)
          {
            if(count!=0) copy(send_buf+j, send_buf + min(BUFFER_SIZE, count-j) , buffer);
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


  int MPI_Gatherv2(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[],
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

    if(ep_rank != root)
    {
      recvcounts = new int[ep_size];
      displs = new int[ep_size];
    }
    
    MPI_Bcast(const_cast< int* >(displs),     ep_size, MPI_INT, root, comm);
    MPI_Bcast(const_cast< int* >(recvcounts), ep_size, MPI_INT, root, comm);
                              

    int recv_plus_displs[ep_size];
    for(int i=0; i<ep_size; i++) recv_plus_displs[i] = recvcounts[i] + displs[i];

    for(int j=0; j<mpi_size; j++)
    {
      if(recv_plus_displs[j*num_ep] < displs[j*num_ep+1] ||
         recv_plus_displs[j*num_ep + num_ep -1] < displs[j*num_ep + num_ep -2])  
      {  
        Debug("Call special implementation of mpi_gatherv. 1st condition not OK\n");
        return MPI_Allgatherv_special(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
      }

      for(int i=1; i<num_ep-1; i++)
      {
        if(recv_plus_displs[j*num_ep+i] < displs[j*num_ep+i+1] || 
           recv_plus_displs[j*num_ep+i] < displs[j*num_ep+i-1])
        {
          Debug("Call special implementation of mpi_gatherv. 2nd condition not OK\n");
          return MPI_Allgatherv_special(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
        }
      }
    }


    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_loc = comm.rank_map->at(root).first;


    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    void *local_gather_recvbuf;
    int buffer_size;
    void *master_recvbuf;

    if(ep_rank_loc == 0 && mpi_rank == root_mpi_rank && root_ep_loc != 0) 
    {
      master_recvbuf = new void*[sizeof(recvbuf)];
      assert(root_ep_loc == 0);
    }

    if(ep_rank_loc==0)
    {
      buffer_size = *std::max_element(recv_plus_displs+ep_rank, recv_plus_displs+ep_rank+num_ep);

      local_gather_recvbuf = new void*[datasize*buffer_size];
    }

    MPI_Gatherv_local2(sendbuf, count, datatype, local_gather_recvbuf, recvcounts+ep_rank-ep_rank_loc, displs+ep_rank-ep_rank_loc, comm);

    //MPI_Gather
    if(ep_rank_loc == 0)
    {
      int *mpi_recvcnt= new int[mpi_size];
      int *mpi_displs= new int[mpi_size];

      int buff_start = *std::min_element(displs+ep_rank, displs+ep_rank+num_ep);;
      int buff_end = buffer_size;

      int mpi_sendcnt = buff_end - buff_start;


      ::MPI_Gather(&mpi_sendcnt, 1, MPI_INT, mpi_recvcnt, 1, MPI_INT, root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      ::MPI_Gather(&buff_start,  1, MPI_INT, mpi_displs,  1, MPI_INT, root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));

      if(root_ep_loc == 0)
      {  ::MPI_Gatherv(local_gather_recvbuf + datasize*buff_start, mpi_sendcnt, static_cast< ::MPI_Datatype>(datatype), recvbuf, mpi_recvcnt,
                       mpi_displs, static_cast< ::MPI_Datatype>(datatype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      }
      else  // gatherv to master_recvbuf
      {  ::MPI_Gatherv(local_gather_recvbuf + datasize*buff_start, mpi_sendcnt, static_cast< ::MPI_Datatype>(datatype), master_recvbuf, mpi_recvcnt,
                       mpi_displs, static_cast< ::MPI_Datatype>(datatype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      }

      delete[] mpi_recvcnt;
      delete[] mpi_displs;
    }

    int global_min_displs = *std::min_element(displs, displs+ep_size);
    int global_recvcnt = *std::max_element(recv_plus_displs, recv_plus_displs+ep_size);


    if(root_ep_loc != 0 && mpi_rank == root_mpi_rank) // root is not master, master send to root and root receive from master
    {
      innode_memcpy(0, master_recvbuf+datasize*global_min_displs, root_ep_loc, recvbuf+datasize*global_min_displs, global_recvcnt, datatype, comm);
      if(ep_rank_loc == 0) delete[] master_recvbuf;
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



  int MPI_Allgatherv2(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[],
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
    
    if(ep_size == mpi_size)  // needed by servers
      return ::MPI_Allgatherv(sendbuf, sendcount, static_cast< ::MPI_Datatype>(datatype), recvbuf, recvcounts, displs,
                              static_cast< ::MPI_Datatype>(datatype), static_cast< ::MPI_Comm>(comm.mpi_comm));

    int recv_plus_displs[ep_size];
    for(int i=0; i<ep_size; i++) recv_plus_displs[i] = recvcounts[i] + displs[i];


    for(int j=0; j<mpi_size; j++)
    {
      if(recv_plus_displs[j*num_ep] < displs[j*num_ep+1] ||
         recv_plus_displs[j*num_ep + num_ep -1] < displs[j*num_ep + num_ep -2])  
      {  
        printf("proc %d/%d Call special implementation of mpi_allgatherv.\n", ep_rank, ep_size);
        for(int k=0; k<ep_size; k++)
          printf("recv_plus_displs[%d] = %d\t displs[%d] = %d\n", k, recv_plus_displs[k], k, displs[k]);

        return MPI_Allgatherv_special(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
      }

      for(int i=1; i<num_ep-1; i++)
      {
        if(recv_plus_displs[j*num_ep+i] < displs[j*num_ep+i+1] || 
           recv_plus_displs[j*num_ep+i] < displs[j*num_ep+i-1])
        {
          printf("proc %d/%d Call special implementation of mpi_allgatherv.\n", ep_rank, ep_size);
          return MPI_Allgatherv_special(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
        }
      }
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
    MPI_Gatherv_local2(sendbuf, count, datatype, local_gather_recvbuf, recvcounts+ep_rank-ep_rank_loc, displs+ep_rank-ep_rank_loc, comm);

    //MPI_Gather
    if(ep_rank_loc == 0)
    {
      int *mpi_recvcnt= new int[mpi_size];
      int *mpi_displs= new int[mpi_size];

      int buff_start = *std::min_element(displs+ep_rank, displs+ep_rank+num_ep);;
      int buff_end = buffer_size;

      int mpi_sendcnt = buff_end - buff_start;


      ::MPI_Allgather(&mpi_sendcnt, 1, MPI_INT, mpi_recvcnt, 1, MPI_INT, static_cast< ::MPI_Comm>(comm.mpi_comm));
      ::MPI_Allgather(&buff_start,  1, MPI_INT, mpi_displs,  1, MPI_INT, static_cast< ::MPI_Comm>(comm.mpi_comm));


      ::MPI_Allgatherv((char*)local_gather_recvbuf + datasize*buff_start, mpi_sendcnt, static_cast< ::MPI_Datatype>(datatype), recvbuf, mpi_recvcnt,
                       mpi_displs, static_cast< ::MPI_Datatype>(datatype), static_cast< ::MPI_Comm>(comm.mpi_comm));

      delete[] mpi_recvcnt;
      delete[] mpi_displs;
    }

    int global_min_displs = *std::min_element(displs, displs+ep_size);
    int global_recvcnt = *std::max_element(recv_plus_displs, recv_plus_displs+ep_size);

    MPI_Bcast_local2(recvbuf+datasize*global_min_displs, global_recvcnt, datatype, comm);

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

  int MPI_Gatherv_special(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[],
                          MPI_Datatype recvtype, int root, MPI_Comm comm)
  {
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
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(sendtype), &lb, &datasize);

    void *local_gather_recvbuf;
    int buffer_size;

    int *local_displs = new int[num_ep];
    int *local_rvcnts = new int[num_ep];
    for(int i=0; i<num_ep; i++) local_rvcnts[i] = recvcounts[ep_rank-ep_rank_loc + i];
    local_displs[0] = 0;
    for(int i=1; i<num_ep; i++) local_displs[i] = local_displs[i-1] + local_rvcnts[i-1];

    if(ep_rank_loc==0)
    {
      buffer_size = local_displs[num_ep-1] + recvcounts[ep_rank+num_ep-1];
      local_gather_recvbuf = new void*[datasize*buffer_size];
    }

    // local gather to master
    MPI_Gatherv_local2(sendbuf, sendcount, sendtype, local_gather_recvbuf, local_rvcnts, local_displs, comm); // all sendbuf gathered to master

    int **mpi_recvcnts = new int*[num_ep];
    int **mpi_displs   = new int*[num_ep];
    for(int i=0; i<num_ep; i++) 
    {
      mpi_recvcnts[i] = new int[mpi_size];
      mpi_displs[i]   = new int[mpi_size];
      for(int j=0; j<mpi_size; j++)
      {
        mpi_recvcnts[i][j] = recvcounts[j*num_ep + i];
        mpi_displs[i][j]   = displs[j*num_ep + i];
      }
    } 

    void *master_recvbuf;
    if(ep_rank_loc == 0 && mpi_rank == root_mpi_rank && root_ep_loc != 0) master_recvbuf = new void*[sizeof(recvbuf)];

    if(ep_rank_loc == 0 && root_ep_loc == 0) // master in MPI_Allgatherv loop
      for(int i=0; i<num_ep; i++)
      {
        ::MPI_Gatherv(local_gather_recvbuf + datasize*local_displs[i], recvcounts[ep_rank+i], static_cast< ::MPI_Datatype>(sendtype), recvbuf, mpi_recvcnts[i], mpi_displs[i],
                    static_cast< ::MPI_Datatype>(recvtype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      }
    if(ep_rank_loc == 0 && root_ep_loc != 0)
      for(int i=0; i<num_ep; i++)
      {
        ::MPI_Gatherv(local_gather_recvbuf + datasize*local_displs[i], recvcounts[ep_rank+i], static_cast< ::MPI_Datatype>(sendtype), master_recvbuf, mpi_recvcnts[i], mpi_displs[i],
                    static_cast< ::MPI_Datatype>(recvtype), root_mpi_rank, static_cast< ::MPI_Comm>(comm.mpi_comm));
      }


    if(root_ep_loc != 0 && mpi_rank == root_mpi_rank) // root is not master, master send to root and root receive from master
    {
      for(int i=0; i<ep_size; i++)
        innode_memcpy(0, master_recvbuf + datasize*displs[i], root_ep_loc, recvbuf + datasize*displs[i], recvcounts[i], sendtype, comm);

      if(ep_rank_loc == 0) delete[] master_recvbuf;
    }

    
    delete[] local_displs;
    delete[] local_rvcnts;
    for(int i=0; i<num_ep; i++) { delete[] mpi_recvcnts[i]; 
                                  delete[] mpi_displs[i]; }
    delete[] mpi_recvcnts;
    delete[] mpi_displs;
    if(ep_rank_loc==0)
    {
      if(sendtype == MPI_INT)
      {
        delete[] static_cast<int*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_FLOAT)
      {
        delete[] static_cast<float*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_DOUBLE)
      {
        delete[] static_cast<double*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_LONG)
      {
        delete[] static_cast<long*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_UNSIGNED_LONG)
      {
        delete[] static_cast<unsigned long*>(local_gather_recvbuf);
      }
      else // if(sendtype == MPI_CHAR)
      {
        delete[] static_cast<char*>(local_gather_recvbuf);
      }
    }
  }

  int MPI_Allgatherv_special(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[],
                             MPI_Datatype recvtype, MPI_Comm comm)
  {
    int ep_rank, ep_rank_loc, mpi_rank;
    int ep_size, num_ep, mpi_size;

    ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;


    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(sendtype), &lb, &datasize);

    void *local_gather_recvbuf;
    int buffer_size;

    int *local_displs = new int[num_ep];
    int *local_rvcnts = new int[num_ep];
    for(int i=0; i<num_ep; i++) local_rvcnts[i] = recvcounts[ep_rank-ep_rank_loc + i];
    local_displs[0] = 0;
    for(int i=1; i<num_ep; i++) local_displs[i] = local_displs[i-1] + local_rvcnts[i-1];

    if(ep_rank_loc==0)
    {
      buffer_size = local_displs[num_ep-1] + recvcounts[ep_rank+num_ep-1];
      local_gather_recvbuf = new void*[datasize*buffer_size];
    }

    // local gather to master
    MPI_Gatherv_local2(sendbuf, sendcount, sendtype, local_gather_recvbuf, local_rvcnts, local_displs, comm); // all sendbuf gathered to master

    int **mpi_recvcnts = new int*[num_ep];
    int **mpi_displs   = new int*[num_ep];
    for(int i=0; i<num_ep; i++) 
    {
      mpi_recvcnts[i] = new int[mpi_size];
      mpi_displs[i]   = new int[mpi_size];
      for(int j=0; j<mpi_size; j++)
      {
        mpi_recvcnts[i][j] = recvcounts[j*num_ep + i];
        mpi_displs[i][j]   = displs[j*num_ep + i];
      }
    } 

    if(ep_rank_loc == 0) // master in MPI_Allgatherv loop
    for(int i=0; i<num_ep; i++)
    {
      ::MPI_Allgatherv(local_gather_recvbuf + datasize*local_displs[i], recvcounts[ep_rank+i], static_cast< ::MPI_Datatype>(sendtype), recvbuf, mpi_recvcnts[i], mpi_displs[i],
                  static_cast< ::MPI_Datatype>(recvtype), static_cast< ::MPI_Comm>(comm.mpi_comm));
    }

    for(int i=0; i<ep_size; i++)
      MPI_Bcast_local2(recvbuf + datasize*displs[i], recvcounts[i], recvtype, comm);

    
    delete[] local_displs;
    delete[] local_rvcnts;
    for(int i=0; i<num_ep; i++) { delete[] mpi_recvcnts[i]; 
                                  delete[] mpi_displs[i]; }
    delete[] mpi_recvcnts;
    delete[] mpi_displs;
    if(ep_rank_loc==0)
    {
      if(sendtype == MPI_INT)
      {
        delete[] static_cast<int*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_FLOAT)
      {
        delete[] static_cast<float*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_DOUBLE)
      {
        delete[] static_cast<double*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_LONG)
      {
        delete[] static_cast<long*>(local_gather_recvbuf);
      }
      else if(sendtype == MPI_UNSIGNED_LONG)
      {
        delete[] static_cast<unsigned long*>(local_gather_recvbuf);
      }
      else // if(sendtype == MPI_CHAR)
      {
        delete[] static_cast<char*>(local_gather_recvbuf);
      }
    }
  }


}
