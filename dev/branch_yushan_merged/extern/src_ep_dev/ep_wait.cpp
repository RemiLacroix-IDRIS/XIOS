/*!
  \file ep_wait.cpp
  \since 2 may 2016

  \brief Definitions of MPI wait function: MPI_Wait, MPI_Waitall, MPI_Waitsome, MPI_Waitany
  */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"

using namespace std;



namespace ep_lib 
{	

  int MPI_Wait(MPI_Request *request, MPI_Status *status)
  {

    if(request->type == 1)  //=>isend
    {
      ::MPI_Request mpi_request = static_cast< ::MPI_Request >(request->mpi_request);
      ::MPI_Status mpi_status;
      ::MPI_Errhandler_set(MPI_COMM_WORLD_STD, MPI_ERRORS_RETURN);
      int error_code = ::MPI_Wait(&mpi_request, &mpi_status);
      if (error_code != MPI_SUCCESS) {
      
         char error_string[BUFSIZ];
         int length_of_error_string, error_class;
      
         ::MPI_Error_class(error_code, &error_class);
         ::MPI_Error_string(error_class, error_string, &length_of_error_string);
         printf("%s\n", error_string);
      }

      status->mpi_status = &mpi_status;
      status->ep_src = request->ep_src;
      status->ep_tag = request->ep_tag;
      status->ep_datatype = request->ep_datatype;

      return 0;
    }

    if(request->type == 3) //=>imrecv
    {
      ::MPI_Request mpi_request = static_cast< ::MPI_Request >(request->mpi_request);
      ::MPI_Status mpi_status;
      ::MPI_Errhandler_set(MPI_COMM_WORLD_STD, MPI_ERRORS_RETURN);
      int error_code = ::MPI_Wait(&mpi_request, &mpi_status);
      if (error_code != MPI_SUCCESS) {
      
         char error_string[BUFSIZ];
         int length_of_error_string, error_class;
      
         ::MPI_Error_class(error_code, &error_class);
         ::MPI_Error_string(error_class, error_string, &length_of_error_string);
         printf("%s\n", error_string);
      }
      

      status->mpi_status = new ::MPI_Status(mpi_status);
      status->ep_src = request->ep_src;
      status->ep_tag = request->ep_tag;
      status->ep_datatype = request->ep_datatype;

      //check_sum_recv(request->buf, count, request->ep_datatype, request->ep_src, request->ep_tag, request->comm, 2);
      return 0;

    }

    if(request->type == 2) //=>irecv not probed
    {
      
      while(true)
      {
        Message_Check(request->comm);
        // parcours pending list
        for(std::list<MPI_Request* >::iterator it = (request->pending_ptr)->begin(); it!=(request->pending_ptr)->end(); )
        {
          if(*it == request)
          { 
            int probed = false;
            MPI_Message message;

            MPI_Improbe(request->ep_src, request->ep_tag, request->comm, &probed, &message, status);
                  
            if(probed)
            {
              int recv_count;
              MPI_Get_count(status, request->ep_datatype, &recv_count);
              MPI_Mrecv(request->buf, recv_count, request->ep_datatype, &message, status);
              (request->pending_ptr)->erase(it);
              //printf("wait  : pending request processed, size = %d\n", (request->pending_ptr)->size());
              request->type = 3;
            
              return 0;
            }

            it++;
          }
          else 
          {
            int probed = false;
            MPI_Message message;
            MPI_Status status;

            MPI_Improbe((*it)->ep_src, (*it)->ep_tag, (*it)->comm, &probed, &message, &status);
                  
            if(probed)
            {
              int recv_count;
              MPI_Get_count(&status, (*it)->ep_datatype, &recv_count);
              MPI_Imrecv((*it)->buf, recv_count, (*it)->ep_datatype, &message, *it);
                  
              (request->pending_ptr)->erase(it);
                  
              it = (request->pending_ptr)->begin();
              //printf("wait  : pending request processed, size = %d\n", (request->pending_ptr)->size());
            }
            else it++;
          }
        }

      }
      

    }

    return MPI_SUCCESS;

  }   /*end of mpi_wait*/






  int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
  {
    //int dest_rank;
    //MPI_Comm_rank(MPI_COMM_WORLD, &dest_rank);
    //printf("proc %d enters waitall\n", dest_rank);

    int finished = 0;
    int finished_index[count];

    for(int i=0; i<count; i++)
      printf("pending add = %p\n",  array_of_requests[i].pending_ptr);

    //if(EP_PendingRequests == 0) EP_PendingRequests = new std::list< MPI_Request* >;  
    //printf("pending size = %d, add = %p\n", EP_PendingRequests->size(), EP_PendingRequests);

    for(int i=0; i<count; i++)
    {
      finished_index[i] = false;
    }

    while(finished < count)
    {
      
      for(int i=0; i<count; i++)
      {
        if(finished_index[i] == false) // this request has not been tested.
        {
          if(array_of_requests[i].type == 1 || array_of_requests[i].type == 3) // isend or imrecv
          {      
            //MPI_Wait(&array_of_requests[i], &array_of_statuses[i]);
            int tested;
            MPI_Test(&array_of_requests[i], &tested, &array_of_statuses[i]);
            if(!tested) MPI_Wait(&array_of_requests[i], &array_of_statuses[i]);
            finished++;
            finished_index[i] = true;
          }
          else // irecv
          {
            
            Message_Check(array_of_requests[i].comm);
            // parcours pending list
            for(std::list<MPI_Request* >::iterator it = (array_of_requests[i].pending_ptr)->begin(); it!=(array_of_requests[i].pending_ptr)->end(); )
            {
              bool matched = false;
              for(int j=0; j<count; j++)
              {
                if(*it == &array_of_requests[j])
                { 
                  int probed = false;
                  MPI_Message message;

                  MPI_Improbe((*it)->ep_src, (*it)->ep_tag, (*it)->comm, &probed, &message, &array_of_statuses[j]);
                  
                  if(probed)
                  {
                    int recv_count;
                    MPI_Get_count(&array_of_statuses[j], array_of_requests[j].ep_datatype, &recv_count);
                    MPI_Mrecv(array_of_requests[j].buf, recv_count, array_of_requests[j].ep_datatype, &message, &array_of_statuses[j]);
                    //check_sum_recv(array_of_requests[i].buf, recv_count, array_of_requests[i].ep_datatype, array_of_requests[i].ep_src, array_of_requests[i].ep_tag, array_of_requests[i].comm, 2);
                    (array_of_requests[i].pending_ptr)->erase(it);
                    array_of_requests[j].type = 3;
                    finished++;
                    finished_index[j] = true;
                    matched = true;
                    it = (array_of_requests[i].pending_ptr)->begin();
                    j=count;
                    //printf("waitall  : pending request processed, size = %d\n", (*(array_of_requests[i].pending_ptr))->size());
                    //printf("matched = %d, j=%d, src = %d, tag = %d, probed = %d\n", matched, j, (*it)->ep_src, (*it)->ep_tag, probed);
                  }
                }

              }

              if(!matched)
              {
                int probed = false;
                MPI_Message message;
                MPI_Status status;

                MPI_Improbe((*it)->ep_src, (*it)->ep_tag, (*it)->comm, &probed, &message, &status);
                  
                if(probed)
                {
                  int recv_count;
                  MPI_Get_count(&status, (*it)->ep_datatype, &recv_count);
                  MPI_Imrecv((*it)->buf, recv_count, (*it)->ep_datatype, &message, *it);
                  
                  (array_of_requests[i].pending_ptr)->erase(it);
                  
                  it = (array_of_requests[i].pending_ptr)->begin();
                  //printf("waitall  : pending request processed, size = %d\n", (*(array_of_requests[i].pending_ptr))->size());
                }
                else it++;
              }
            }

          }
        }
      }    
    }
    //printf("proc %d exits waitall\n", dest_rank);
    return MPI_SUCCESS;
  }  /* end of mpi_waitall*/


}
