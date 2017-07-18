/*!
   \file ep_recv.cpp
   \since 2 may 2016

   \brief Definitions of MPI receive functions: MPI_Recv, MPI_Mrecv, MPI_Irecv, MPI_Imrecv
 */


#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"

using namespace std;

extern std::list< ep_lib::MPI_Request* > * EP_PendingRequests;
#pragma omp threadprivate(EP_PendingRequests)

namespace ep_lib 
{
  

  int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, MPI_Status *status)
  {
    int dest_rank;
    MPI_Comm_rank(comm, &dest_rank);

    if(!comm.is_ep)
    {
      ::MPI_Comm mpi_comm = static_cast< ::MPI_Comm >(comm.mpi_comm);
      ::MPI_Status mpi_status;
      ::MPI_Recv(buf, count, static_cast< ::MPI_Datatype >(datatype), src<0? MPI_ANY_SOURCE : src, tag<0? MPI_ANY_TAG: tag, mpi_comm, &mpi_status);

      status->ep_src = src;
      status->ep_tag = tag;
      status->ep_datatype = datatype;

      return 0;  
    }

    Message_Check(comm);

    MPI_Request request;
    MPI_Irecv(buf, count, datatype, src, tag, comm, &request);
    MPI_Wait(&request, status);

    return 0;
  }




  int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, MPI_Request *request)
  {

    Debug("MPI_Irecv with EP");
    int dest_rank;
    MPI_Comm_rank(comm, &dest_rank);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD_STD, &world_rank);

    if(!comm.is_ep)
    {
      ::MPI_Request mpi_request;
      ::MPI_Comm mpi_comm = static_cast< ::MPI_Comm > (comm.mpi_comm);
      ::MPI_Irecv(buf, count, static_cast< ::MPI_Datatype> (datatype), src<0? MPI_ANY_SOURCE : src, tag<0? MPI_ANY_TAG: tag, mpi_comm, &mpi_request);

      request->mpi_request = mpi_request;
      request->ep_src = src;
      request->ep_datatype = datatype;
      request->ep_tag = tag;
    }

    request->mpi_request = MPI_REQUEST_NULL_STD;
    request->buf = buf;
    request->comm = comm;
    request->type = 2;

    request->ep_src = src;
    request->ep_tag = tag;
    request->ep_datatype = datatype;



    /* With Improbe*/
    Message_Check(comm);

    if(EP_PendingRequests == 0 ) 
    {
      EP_PendingRequests = new std::list< MPI_Request* >;
      //printf("proc %d(%d) : EP_PendingRequests allocated, add = %p\n", dest_rank, world_rank, EP_PendingRequests);  
    }


    EP_PendingRequests->push_back(request);

    Request_Check();
    //printf("proc %d(%d) : EP_PendingRequests insert one request, src = %d(%d), tag = %d(%d), size = %d; request add = %p\n", 
    //        dest_rank, world_rank, EP_PendingRequests->back()->ep_src, request->ep_src, 
    //        EP_PendingRequests->back()->ep_tag, request->ep_tag, 
    //        EP_PendingRequests->size(), request);
    
    // check all EP_PendingRequests      
    //for(std::list<MPI_Request* >::iterator it = EP_PendingRequests->begin(); it!=EP_PendingRequests->end(); )
    //{
    //if((*it)->type == 3) 
    //{
    //    EP_PendingRequests->erase(it);
   //     it = EP_PendingRequests->begin();
    //    continue;
     // }
        
      //int probed = false;
      //MPI_Message pending_message;
      //MPI_Status pending_status;
    
      //MPI_Improbe((*it)->ep_src, (*it)->ep_tag, (*it)->comm, &probed, &pending_message, &pending_status);
    
      //if(probed) 
      //{ 
        //int count;
        //MPI_Get_count(&pending_status, (*it)->ep_datatype, &count);
        //MPI_Imrecv((*it)->buf, count, (*it)->ep_datatype, &pending_message, *it);

        //EP_PendingRequests->erase(it);
        //if(EP_PendingRequests->empty()) return 0;
        
        //it = EP_PendingRequests->begin();
        //continue;
     // }

      //it++;
   // }
    
    return 0;
  }

  int MPI_Imrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Request *request)
  {
    Debug("MPI_Imrecv");

    request->type = 3;

    ::MPI_Request mpi_request;
    ::MPI_Message mpi_message = static_cast< ::MPI_Message >(message->mpi_message);
		
    ::MPI_Imrecv(buf, count, static_cast< ::MPI_Datatype>(datatype), &mpi_message, &mpi_request);

    request->mpi_request = mpi_request;
    request->ep_datatype = datatype;
    request->ep_tag = message->ep_tag;
    request->ep_src = message->ep_src;

    return 0;
  }


  int MPI_Mrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Status *status)
  {
    Debug("EP Mrecv called\n");

    ::MPI_Status mpi_status;
    ::MPI_Message mpi_message = static_cast< ::MPI_Message >(message->mpi_message);
    
    ::MPI_Mrecv(buf, count, static_cast< ::MPI_Datatype>(datatype), &mpi_message, &mpi_status);

    status->mpi_status = new ::MPI_Status(mpi_status);
    status->ep_src = message->ep_src;
    status->ep_datatype = datatype;
    status->ep_tag = message->ep_tag;

    //check_sum_recv(buf, count, datatype, message->ep_src, message->ep_tag);

    return 0;
  }

}


