#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include <iostream>
#include <fstream>

using namespace std;

std::list< ep_lib::MPI_Request* > * EP_PendingRequests = 0;
#pragma omp threadprivate(EP_PendingRequests)

namespace ep_lib
{ 

  int tag_combine(int real_tag, int src, int dest)
  {
    int a = real_tag << 16;
    int b = src << 8;
    int c = dest;

    return a+b+c;
  }

  int get_ep_rank(MPI_Comm comm, int ep_rank_loc, int mpi_rank)
  {
    for(int i=0; i<comm.rank_map->size(); i++)
    {
      if(   ( comm.rank_map->at(i).first  == ep_rank_loc )
         && ( comm.rank_map->at(i).second == mpi_rank ) )
      {
        return i;
      }
    }
    printf("rank not find\n");
  }
  
  int get_ep_rank_intercomm(MPI_Comm comm, int ep_rank_loc, int mpi_rank)
  {
    // intercomm
    int inter_rank;
    for(int i=0; i<comm.ep_comm_ptr->intercomm->intercomm_rank_map->size(); i++)
    {
      if(   ( comm.ep_comm_ptr->intercomm->intercomm_rank_map->at(i).first  == ep_rank_loc )
         && ( comm.ep_comm_ptr->intercomm->intercomm_rank_map->at(i).second == mpi_rank ) )
      {
        inter_rank =  i;
        break;
      }
    }

    for(int i=0; i<comm.ep_comm_ptr->intercomm->remote_rank_map->size(); i++)
    {
      if(  comm.ep_comm_ptr->intercomm->remote_rank_map->at(i).first  == inter_rank  )
      {
        //printf("get_ep_rank for intercomm, ep_rank_loc = %d, mpi_rank = %d => ep_src = %d\n", ep_rank_loc, mpi_rank, i);
        return i;
      }
    }

    printf("rank not find\n");
    
  }


  int MPI_Get_count(const MPI_Status *status, MPI_Datatype datatype, int *count)
  {

    ::MPI_Status *mpi_status = static_cast< ::MPI_Status* >(status->mpi_status);
    ::MPI_Datatype mpi_datatype = static_cast< ::MPI_Datatype >(datatype);

    ::MPI_Get_count(mpi_status, mpi_datatype, count);
  }

  double MPI_Wtime()
  {
    return ::MPI_Wtime();

  }

  void check_sum_send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, int type)
  {
    int src_rank;
    int int_count;
    ::MPI_Aint datasize, intsize, charsize, lb;
    
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype> (MPI_CHAR), &lb, &intsize);

    int_count = count * datasize / intsize ;

    char *buffer = static_cast< char* >(const_cast< void*> (buf));
    
    unsigned long sum = 0;
    for(int i = 0; i<int_count; i++)
      sum += *(buffer+i); 


    MPI_Comm_rank(comm, &src_rank);
    
    ofstream myfile;
    myfile.open ("send_log.txt", ios::app);
    if (myfile.is_open())
    {
      myfile << "type = " << type << " src = "<< src_rank<< " dest = "<< dest <<" tag = "<< tag << "  count = "<< count << " sum = "<< sum << "\n";
      myfile.close();  
    }
    else printf("Unable to open file\n");

  }


  void check_sum_recv(void *buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, int type)
  {
    int dest_rank;
    int int_count;
    ::MPI_Aint datasize, intsize, charsize, lb;
    
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(datatype), &lb, &datasize);
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype> (MPI_CHAR), &lb, &intsize);

    int_count = count * datasize / intsize ;

    char *buffer = static_cast< char* >(buf);
    
    unsigned long sum = 0;
    for(int i = 0; i<int_count; i++)
      sum += *(buffer+i); 


    MPI_Comm_rank(comm, &dest_rank);
    
    ofstream myfile;
    myfile.open ("recv_log.txt", ios::app);
    if (myfile.is_open())
    {
      myfile << "type = " << type << " src = "<< src << " dest = "<< dest_rank <<" tag = "<< tag << "  count = "<< count << " sum = "<< sum << "\n";
      myfile.close();  
    }
    else printf("Unable to open file\n");

  }

  int test_sendrecv(MPI_Comm comm)
  {
    int myRank;
    MPI_Comm_rank(comm, &myRank);
    bool amClient = false;
    bool amServer = false;
    if(myRank<=3) amClient = true;
    else amServer = true;

    if(amServer)
    {
      int send_buf[4];
      MPI_Request send_request[8];
      MPI_Status send_status[8];

      
      
      for(int j=0; j<4; j++)  // 4 buffers
      {
        for(int i=0; i<2; i++)
        {
          send_buf[j] = (myRank+1)*100 + j;
          MPI_Isend(&send_buf[j], 1, MPI_INT, i*2, 9999, comm, &send_request[i*4+j]);
        }
      }
      

      MPI_Waitall(8, send_request, send_status);
    }


    if(amClient&&myRank%2==0) // Clients leaders
    {
      int recv_buf[8];
      MPI_Request recv_request[8];
      MPI_Status recv_status[8];

      for(int i=0; i<2; i++)  // 2 servers
      {
        for(int j=0; j<4; j++)
        {
          MPI_Irecv(&recv_buf[i*4+j], 1, MPI_INT, i+4, 9999, comm, &recv_request[i*4+j]);
        }
      }

      MPI_Waitall(8, recv_request, recv_status);
      printf("============ client %d, recv_buf = %d, %d, %d, %d, %d, %d, %d, %d ================\n", 
              myRank, recv_buf[0], recv_buf[1], recv_buf[2], recv_buf[3], recv_buf[4], recv_buf[5], recv_buf[6], recv_buf[7]);
    }

    MPI_Barrier(comm);

  }

  bool valid_type(MPI_Datatype datatype)
  {
    if(   datatype == MPI_INT 
       || datatype == MPI_FLOAT
       || datatype == MPI_DOUBLE 
       || datatype == MPI_CHAR 
       || datatype == MPI_LONG 
       || datatype == MPI_UNSIGNED_LONG)
    {
      return true;
    }

    return false;
  }


  bool valid_op(MPI_Op op)
  {
    if(   op == MPI_MAX 
       || op == MPI_MIN
       || op == MPI_SUM)
    {
      return true;
    }

    return false;
  }


}


MPI_Datatype to_mpi_type(ep_lib::MPI_Datatype type)
{
  return static_cast< MPI_Datatype >(type);
}

MPI_Op to_mpi_op(ep_lib::MPI_Op op)
{
  return static_cast< MPI_Op >(op);
}

MPI_Comm to_mpi_comm(int comm)
{
  return static_cast< MPI_Comm >(comm);
}  






