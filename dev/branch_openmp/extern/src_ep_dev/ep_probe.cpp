#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

namespace ep_lib
{

  int MPI_Iprobe(int src, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
  {
    *flag = false;

    if(!comm.is_ep)
    {
      ::MPI_Status *mpi_status = static_cast< ::MPI_Status* >(status->mpi_status);
      ::MPI_Iprobe(src<0? MPI_ANY_SOURCE : src, tag<0? MPI_ANY_TAG: tag, to_mpi_comm(comm.mpi_comm), flag, mpi_status);

      status->mpi_status = mpi_status;
      status->ep_src = src;
      status->ep_tag = tag;
      return 0;
    }

    Debug("calling MPI_Iprobe EP\n");

    Message_Check(comm);

    #pragma omp flush

    #pragma omp critical (_query)
    if(!comm.ep_comm_ptr->message_queue->empty())
    {
      for(Message_list::iterator it = comm.ep_comm_ptr->message_queue->begin(); it!= comm.ep_comm_ptr->message_queue->end(); ++it)
      {
        bool src_matched = src<0? true: it->ep_src == src;
        bool tag_matched = tag<0? true: it->ep_tag == tag;
        
        if(src_matched && tag_matched)        
        {
          Debug("find message\n");
          *flag = true;

          ::MPI_Status mpi_status = *(static_cast< ::MPI_Status *>(it->mpi_status));

          status->mpi_status = new ::MPI_Status(mpi_status);
          status->ep_src = it->ep_src;
          status->ep_tag = it->ep_tag;

          break;
        }

      }
    }

    return 0;
  }



  int MPI_Improbe(int src, int tag, MPI_Comm comm, int *flag, MPI_Message *message, MPI_Status *status)
  {
    *flag = false;
    if(!comm.is_ep)
    {
      Debug("calling MPI_Improbe MPI\n");

      ::MPI_Status mpi_status;
      ::MPI_Message mpi_message;

      #ifdef _openmpi
      #pragma omp critical (_mpi_call)
      {
        ::MPI_Iprobe(src<0? MPI_ANY_SOURCE : src, tag<0? MPI_ANY_TAG: tag, to_mpi_comm(comm.mpi_comm), flag, &mpi_status);
        if(*flag)
        {
          ::MPI_Mprobe(src<0? MPI_ANY_SOURCE : src, tag<0? MPI_ANY_TAG: tag, to_mpi_comm(comm.mpi_comm), &mpi_message, &mpi_status);
        }
      }
      #elif _intelmpi
    	::MPI_Improbe(src<0? MPI_ANY_SOURCE : src, tag<0? MPI_ANY_TAG: tag, to_mpi_comm(comm.mpi_comm), flag, &mpi_message, &mpi_status);
      #endif
    	
      status->mpi_status = new ::MPI_Status(mpi_status);
      status->ep_src = src;
      status->ep_tag = tag;

      message->mpi_message = mpi_message;
      message->ep_src = src;
      message->ep_tag = tag;
      return 0;
    }

    
    //Message_Check(comm);

    #pragma omp flush

    #pragma omp critical (_query)
    if(! comm.ep_comm_ptr->message_queue->empty())
    {
      for(Message_list::iterator it = comm.ep_comm_ptr->message_queue->begin(); it!= comm.ep_comm_ptr->message_queue->end(); ++it)
      {
        bool src_matched = src<0? true: it->ep_src == src;
        bool tag_matched = tag<0? true: it->ep_tag == tag;
        
        if(src_matched && tag_matched)
        {
          *flag = true;

          ::MPI_Status mpi_status;
          mpi_status = *(static_cast< ::MPI_Status *>(it->mpi_status));

          status->mpi_status = new ::MPI_Status(mpi_status);
          status->ep_src = it->ep_src;
          status->ep_tag = it->ep_tag;

          message->mpi_message = it->mpi_message;
          message->ep_tag = it->ep_tag;
          message->ep_src = it->ep_src;

          #pragma omp critical (_query2)
          {              
            delete it->mpi_status;
            comm.ep_comm_ptr->message_queue->erase(it);
            #pragma omp flush
          }

          break;
        }

      }
    }
  }

}

