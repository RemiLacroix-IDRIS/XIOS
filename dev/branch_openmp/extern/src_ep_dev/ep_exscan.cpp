/*!
   \file ep_scan.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Exscan
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;

namespace ep_lib
{
  template<typename T>
  T max_op(T a, T b)
  {
    return max(a,b);
  }

  template<typename T>
  T min_op(T a, T b)
  {
    return min(a,b);
  }

  template<typename T>
  void reduce_max(const T * buffer, T* recvbuf, int count)
  {
    transform(buffer, buffer+count, recvbuf, recvbuf, max_op<T>);
  }

  template<typename T>
  void reduce_min(const T * buffer, T* recvbuf, int count)
  {
    transform(buffer, buffer+count, recvbuf, recvbuf, min_op<T>);
  }

  template<typename T>
  void reduce_sum(const T * buffer, T* recvbuf, int count)
  {
    transform(buffer, buffer+count, recvbuf, recvbuf, std::plus<T>());
  }


  int MPI_Exscan_local(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
  {
    valid_op(op);

    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);

    if(ep_rank_loc == 0 && mpi_rank != 0)
    {
      comm.my_buffer->void_buffer[0] = recvbuf;
    }
    if(ep_rank_loc == 0 && mpi_rank == 0)
    {
      comm.my_buffer->void_buffer[0] = const_cast<void*>(sendbuf);  
    } 
      

    MPI_Barrier_local(comm);

    memcpy(recvbuf, comm.my_buffer->void_buffer[0], datasize*count);

    MPI_Barrier_local(comm);

    comm.my_buffer->void_buffer[ep_rank_loc] = const_cast<void*>(sendbuf);  
    
    MPI_Barrier_local(comm);

    if(op == MPI_SUM)
    {
      if(datatype == MPI_INT )
      {
        assert(datasize == sizeof(int));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_sum<int>(static_cast<int*>(comm.my_buffer->void_buffer[i]), static_cast<int*>(recvbuf), count);    
      }
     
      else if(datatype == MPI_FLOAT )
      {
        assert(datasize == sizeof(float));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_sum<float>(static_cast<float*>(comm.my_buffer->void_buffer[i]), static_cast<float*>(recvbuf), count);    
      }
      

      else if(datatype == MPI_DOUBLE )
      {
        assert(datasize == sizeof(double));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_sum<double>(static_cast<double*>(comm.my_buffer->void_buffer[i]), static_cast<double*>(recvbuf), count);
      }

      else if(datatype == MPI_CHAR )
      {
        assert(datasize == sizeof(char));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_sum<char>(static_cast<char*>(comm.my_buffer->void_buffer[i]), static_cast<char*>(recvbuf), count);
      }

      else if(datatype == MPI_LONG )
      {
        assert(datasize == sizeof(long));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_sum<long>(static_cast<long*>(comm.my_buffer->void_buffer[i]), static_cast<long*>(recvbuf), count);
      }

      else if(datatype == MPI_UNSIGNED_LONG )
      {
        assert(datasize == sizeof(unsigned long));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_sum<unsigned long>(static_cast<unsigned long*>(comm.my_buffer->void_buffer[i]), static_cast<unsigned long*>(recvbuf), count);    
      }

      else printf("datatype Error\n");

      
    }

    else if(op == MPI_MAX)
    {
      if(datatype == MPI_INT )
      {
        assert(datasize == sizeof(int));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_max<int>(static_cast<int*>(comm.my_buffer->void_buffer[i]), static_cast<int*>(recvbuf), count);    
      }

      else if(datatype == MPI_FLOAT )
      {
        assert(datasize == sizeof(float));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_max<float>(static_cast<float*>(comm.my_buffer->void_buffer[i]), static_cast<float*>(recvbuf), count);    
      }

      else if(datatype == MPI_DOUBLE )
      {
        assert(datasize == sizeof(double));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_max<double>(static_cast<double*>(comm.my_buffer->void_buffer[i]), static_cast<double*>(recvbuf), count);
      }

      else if(datatype == MPI_CHAR )
      {
        assert(datasize == sizeof(char));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_max<char>(static_cast<char*>(comm.my_buffer->void_buffer[i]), static_cast<char*>(recvbuf), count);
      }

      else if(datatype == MPI_LONG )
      {
        assert(datasize == sizeof(long));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_max<long>(static_cast<long*>(comm.my_buffer->void_buffer[i]), static_cast<long*>(recvbuf), count);
      }

      else if(datatype == MPI_UNSIGNED_LONG )
      {
        assert(datasize == sizeof(unsigned long));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_max<unsigned long>(static_cast<unsigned long*>(comm.my_buffer->void_buffer[i]), static_cast<unsigned long*>(recvbuf), count);    
      }
     
      else printf("datatype Error\n");
    }

    else //if(op == MPI_MIN)
    {
      if(datatype == MPI_INT )
      {
        assert(datasize == sizeof(int));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_min<int>(static_cast<int*>(comm.my_buffer->void_buffer[i]), static_cast<int*>(recvbuf), count);    
      }

      else if(datatype == MPI_FLOAT )
      {
        assert(datasize == sizeof(float));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_min<float>(static_cast<float*>(comm.my_buffer->void_buffer[i]), static_cast<float*>(recvbuf), count);    
      }

      else if(datatype == MPI_DOUBLE )
      {
        assert(datasize == sizeof(double));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_min<double>(static_cast<double*>(comm.my_buffer->void_buffer[i]), static_cast<double*>(recvbuf), count);
      }

      else if(datatype == MPI_CHAR )
      {
        assert(datasize == sizeof(char));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_min<char>(static_cast<char*>(comm.my_buffer->void_buffer[i]), static_cast<char*>(recvbuf), count);
      }

      else if(datatype == MPI_LONG )
      {
        assert(datasize == sizeof(long));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_min<long>(static_cast<long*>(comm.my_buffer->void_buffer[i]), static_cast<long*>(recvbuf), count);
      }

      else if(datatype == MPI_UNSIGNED_LONG )
      {
        assert(datasize == sizeof(unsigned long));
        for(int i=0; i<ep_rank_loc; i++)
          reduce_min<unsigned long>(static_cast<unsigned long*>(comm.my_buffer->void_buffer[i]), static_cast<unsigned long*>(recvbuf), count);    
      }

      else printf("datatype Error\n");
    }

    MPI_Barrier_local(comm);

  }

  int MPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Scan(sendbuf, recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), to_mpi_comm(comm.mpi_comm));
    }
    
    valid_type(datatype);

    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);
    
    void* tmp_sendbuf;
    tmp_sendbuf = new void*[datasize * count];

    int my_src = 0;
    int my_dst = ep_rank;

    std::vector<int> my_map(mpi_size, 0);

    for(int i=0; i<comm.rank_map->size(); i++) my_map[comm.rank_map->at(i).second]++;

    for(int i=0; i<mpi_rank; i++) my_src += my_map[i];
    my_src += ep_rank_loc;

     
    for(int i=0; i<mpi_size; i++)
    {
      if(my_dst < my_map[i])
      {
        my_dst = get_ep_rank(comm, my_dst, i); 
        break;
      }
      else
        my_dst -= my_map[i];
    }

    if(ep_rank != my_dst) 
    {
      MPI_Request request[2];
      MPI_Status status[2];

      MPI_Isend(sendbuf,     count, datatype, my_dst, my_dst,  comm, &request[0]);
    
      MPI_Irecv(tmp_sendbuf, count, datatype, my_src, ep_rank, comm, &request[1]);
    
      MPI_Waitall(2, request, status);
    }

    else memcpy(tmp_sendbuf, sendbuf, datasize*count);
    

    void* tmp_recvbuf;
    tmp_recvbuf = new void*[datasize * count];    

    MPI_Reduce_local(tmp_sendbuf, tmp_recvbuf, count, datatype, op, 0, comm);

    if(ep_rank_loc == 0)
      ::MPI_Exscan(MPI_IN_PLACE, tmp_recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), to_mpi_comm(comm.mpi_comm));

    // printf(" ID=%d : %d  %d \n", ep_rank, static_cast<int*>(tmp_recvbuf)[0], static_cast<int*>(tmp_recvbuf)[1]);
    
    MPI_Exscan_local(tmp_sendbuf, tmp_recvbuf, count, datatype, op, comm);

     // printf(" ID=%d : after local tmp_sendbuf = %d %d ; tmp_recvbuf = %d  %d \n", ep_rank, static_cast<int*>(tmp_sendbuf)[0], static_cast<int*>(tmp_sendbuf)[1], static_cast<int*>(tmp_recvbuf)[0], static_cast<int*>(tmp_recvbuf)[1]);



    if(ep_rank != my_src) 
    {
      MPI_Request request[2];
      MPI_Status status[2];

      MPI_Isend(tmp_recvbuf, count, datatype, my_src, my_src,  comm, &request[0]);
    
      MPI_Irecv(recvbuf,     count, datatype, my_dst, ep_rank, comm, &request[1]);
    
      MPI_Waitall(2, request, status);
    }

    else memcpy(recvbuf, tmp_recvbuf, datasize*count);
    



    delete[] tmp_sendbuf;
    delete[] tmp_recvbuf;

  }

}