/*!
   \file ep_reduce.cpp
   \since 2 may 2016

   \brief Definitions of MPI collective function: MPI_Reduce, MPI_Allreduce
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;


namespace ep_lib {

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

  int MPI_Reduce_local(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int local_root, MPI_Comm comm)
  {
    assert(valid_type(datatype));
    assert(valid_op(op));

    ::MPI_Aint datasize, lb;
    ::MPI_Type_get_extent(to_mpi_type(datatype), &lb, &datasize);

    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;

    #pragma omp critical (_reduce)
    comm.my_buffer->void_buffer[ep_rank_loc] = const_cast< void* >(sendbuf);

    MPI_Barrier_local(comm);

    if(ep_rank_loc == local_root)
    {

      memcpy(recvbuf, comm.my_buffer->void_buffer[0], datasize * count);

      if(op == MPI_MAX)
      {
        if(datatype == MPI_INT)
        {
          assert(datasize == sizeof(int));
          for(int i=1; i<num_ep; i++)
            reduce_max<int>(static_cast<int*>(comm.my_buffer->void_buffer[i]), static_cast<int*>(recvbuf), count);
        }

        else if(datatype == MPI_FLOAT)
        {
          assert(datasize == sizeof(float));
          for(int i=1; i<num_ep; i++)
            reduce_max<float>(static_cast<float*>(comm.my_buffer->void_buffer[i]), static_cast<float*>(recvbuf), count);
        }

        else if(datatype == MPI_DOUBLE)
        {
          assert(datasize == sizeof(double));
          for(int i=1; i<num_ep; i++)
            reduce_max<double>(static_cast<double*>(comm.my_buffer->void_buffer[i]), static_cast<double*>(recvbuf), count);
        }

        else if(datatype == MPI_CHAR)
        {
          assert(datasize == sizeof(char));
          for(int i=1; i<num_ep; i++)
            reduce_max<char>(static_cast<char*>(comm.my_buffer->void_buffer[i]), static_cast<char*>(recvbuf), count);
        }

        else if(datatype == MPI_LONG)
        {
          assert(datasize == sizeof(long));
          for(int i=1; i<num_ep; i++)
            reduce_max<long>(static_cast<long*>(comm.my_buffer->void_buffer[i]), static_cast<long*>(recvbuf), count);
        }

        else if(datatype == MPI_UNSIGNED_LONG)
        {
          assert(datasize == sizeof(unsigned long));
          for(int i=1; i<num_ep; i++)
            reduce_max<unsigned long>(static_cast<unsigned long*>(comm.my_buffer->void_buffer[i]), static_cast<unsigned long*>(recvbuf), count);
        }

        else printf("datatype Error\n");

      }

      if(op == MPI_MIN)
      {
        if(datatype ==MPI_INT)
        {
          assert(datasize == sizeof(int));
          for(int i=1; i<num_ep; i++)
            reduce_min<int>(static_cast<int*>(comm.my_buffer->void_buffer[i]), static_cast<int*>(recvbuf), count);
        }

        else if(datatype == MPI_FLOAT)
        {
          assert(datasize == sizeof(float));
          for(int i=1; i<num_ep; i++)
            reduce_min<float>(static_cast<float*>(comm.my_buffer->void_buffer[i]), static_cast<float*>(recvbuf), count);
        }

        else if(datatype == MPI_DOUBLE)
        {
          assert(datasize == sizeof(double));
          for(int i=1; i<num_ep; i++)
            reduce_min<double>(static_cast<double*>(comm.my_buffer->void_buffer[i]), static_cast<double*>(recvbuf), count);
        }

        else if(datatype == MPI_CHAR)
        {
          assert(datasize == sizeof(char));
          for(int i=1; i<num_ep; i++)
            reduce_min<char>(static_cast<char*>(comm.my_buffer->void_buffer[i]), static_cast<char*>(recvbuf), count);
        }

        else if(datatype == MPI_LONG)
        {
          assert(datasize == sizeof(long));
          for(int i=1; i<num_ep; i++)
            reduce_min<long>(static_cast<long*>(comm.my_buffer->void_buffer[i]), static_cast<long*>(recvbuf), count);
        }

        else if(datatype == MPI_UNSIGNED_LONG)
        {
          assert(datasize == sizeof(unsigned long));
          for(int i=1; i<num_ep; i++)
            reduce_min<unsigned long>(static_cast<unsigned long*>(comm.my_buffer->void_buffer[i]), static_cast<unsigned long*>(recvbuf), count);
        }

        else printf("datatype Error\n");

      }


      if(op == MPI_SUM)
      {
        if(datatype==MPI_INT)
        {
          assert(datasize == sizeof(int));
          for(int i=1; i<num_ep; i++)
            reduce_sum<int>(static_cast<int*>(comm.my_buffer->void_buffer[i]), static_cast<int*>(recvbuf), count);
        }

        else if(datatype == MPI_FLOAT)
        {
          assert(datasize == sizeof(float));
          for(int i=1; i<num_ep; i++)
            reduce_sum<float>(static_cast<float*>(comm.my_buffer->void_buffer[i]), static_cast<float*>(recvbuf), count);
        }

        else if(datatype == MPI_DOUBLE)
        {
          assert(datasize == sizeof(double));
          for(int i=1; i<num_ep; i++)
            reduce_sum<double>(static_cast<double*>(comm.my_buffer->void_buffer[i]), static_cast<double*>(recvbuf), count);
        }

        else if(datatype == MPI_CHAR)
        {
          assert(datasize == sizeof(char));
          for(int i=1; i<num_ep; i++)
            reduce_sum<char>(static_cast<char*>(comm.my_buffer->void_buffer[i]), static_cast<char*>(recvbuf), count);
        }

        else if(datatype == MPI_LONG)
        {
          assert(datasize == sizeof(long));
          for(int i=1; i<num_ep; i++)
            reduce_sum<long>(static_cast<long*>(comm.my_buffer->void_buffer[i]), static_cast<long*>(recvbuf), count);
        }

        else if(datatype ==MPI_UNSIGNED_LONG)
        {
          assert(datasize == sizeof(unsigned long));
          for(int i=1; i<num_ep; i++)
            reduce_sum<unsigned long>(static_cast<unsigned long*>(comm.my_buffer->void_buffer[i]), static_cast<unsigned long*>(recvbuf), count);
        }

        else printf("datatype Error\n");

      }
    }

    MPI_Barrier_local(comm);

  }


  int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
  {

    if(!comm.is_ep && comm.mpi_comm)
    {
      return ::MPI_Reduce(sendbuf, recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), root, to_mpi_comm(comm.mpi_comm));
    }



    int ep_rank = comm.ep_comm_ptr->size_rank_info[0].first;
    int ep_rank_loc = comm.ep_comm_ptr->size_rank_info[1].first;
    int mpi_rank = comm.ep_comm_ptr->size_rank_info[2].first;
    int ep_size = comm.ep_comm_ptr->size_rank_info[0].second;
    int num_ep = comm.ep_comm_ptr->size_rank_info[1].second;
    int mpi_size = comm.ep_comm_ptr->size_rank_info[2].second;

    int root_mpi_rank = comm.rank_map->at(root).second;
    int root_ep_loc = comm.rank_map->at(root).first;

    ::MPI_Aint datasize, lb;

    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);

    bool is_master = (ep_rank_loc==0 && mpi_rank != root_mpi_rank ) || ep_rank == root;
    bool is_root = ep_rank == root;

    void* local_recvbuf;

    if(is_master)
    {
      local_recvbuf = new void*[datasize * count];
    }

    if(mpi_rank == root_mpi_rank) MPI_Reduce_local(sendbuf, local_recvbuf, count, datatype, op, root_ep_loc, comm);
    else                          MPI_Reduce_local(sendbuf, local_recvbuf, count, datatype, op, 0, comm);



    if(is_master)
    {
      ::MPI_Reduce(local_recvbuf, recvbuf, count, to_mpi_type(datatype), to_mpi_op(op), root_mpi_rank, to_mpi_comm(comm.mpi_comm));
      
    }

    if(is_master)
    {
      delete[] local_recvbuf;
    }

    MPI_Barrier_local(comm);
  }


}

