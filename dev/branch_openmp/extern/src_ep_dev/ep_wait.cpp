/*!
  \file ep_wait.cpp
  \since 2 may 2016

  \brief Definitions of MPI wait function: MPI_Wait, MPI_Waitall, MPI_Waitsome, MPI_Waitany
  */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"

using namespace std;

extern std::list< ep_lib::MPI_Request* > * EP_PendingRequests;
#pragma omp threadprivate(EP_PendingRequests)



namespace ep_lib 
{
  
  int MPI_Wait(MPI_Request *request, MPI_Status *status)
  {
    if(request->type !=1 && request->type !=2 && request->type !=3) 
    {
      printf("Error in request type\n");
 
      exit(1);
    }

    while(request->type == 2) Request_Check();


    //::MPI_Request mpi_request = static_cast< ::MPI_Request >(request->mpi_request);
    ::MPI_Status mpi_status;
    ::MPI_Wait(&(request->mpi_request), &mpi_status);
      
    // request->mpi_request = mpi_request;

    status->mpi_status = &mpi_status;
    status->ep_src = request->ep_src;
    status->ep_tag = request->ep_tag;
    status->ep_datatype = request->ep_datatype;

    return MPI_SUCCESS;

  }   /*end of mpi_wait*/






  int MPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
  {
    std::vector<int> finished(count, 0);

    ::MPI_Request* mpi_request = new ::MPI_Request[count];
    ::MPI_Status* mpi_status = new ::MPI_Status[count];

    while(std::accumulate(finished.begin(), finished.end(), 0) < count)
    {
      
      for(int i=0; i<count; i++)
      {
        if(array_of_requests[i].type !=1 && array_of_requests[i].type !=2 && array_of_requests[i].type !=3) 
        {
          printf("Error in request type\n");
 
          exit(1);
        }
        
        if(array_of_requests[i].type == 2) Request_Check(); 
        if(array_of_requests[i].type != 2 && finished.at(i) == 0) 
        {
          finished.at(i) = 1;
          mpi_request[i] = static_cast< ::MPI_Request >(array_of_requests[i].mpi_request);
        }
      }    
    }

    ::MPI_Waitall(count, mpi_request, mpi_status);

    for(int i=0; i<count; i++)
    {
      array_of_statuses[i].mpi_status = &mpi_status;
      array_of_statuses[i].ep_src = array_of_requests[i].ep_src;
      array_of_statuses[i].ep_tag = array_of_requests[i].ep_tag;
      array_of_statuses[i].ep_datatype = array_of_requests[i].ep_datatype;
    }

    delete[] mpi_request;
    delete[] mpi_status;
    
    return MPI_SUCCESS;
  }  /* end of mpi_waitall*/


}

