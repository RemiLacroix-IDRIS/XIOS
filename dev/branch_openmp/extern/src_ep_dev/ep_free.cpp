#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"

namespace ep_lib
{

  int MPI_Comm_free(MPI_Comm *comm)
  {

    if(! comm->is_ep)
    {
      if(comm->mpi_comm != static_cast< ::MPI_Comm>(MPI_COMM_NULL.mpi_comm))
      {
        ::MPI_Comm mpi_comm = static_cast< ::MPI_Comm>(comm->mpi_comm);

        ::MPI_Comm_free(&mpi_comm);
        Debug("comm is MPI, freed\n");
      }
      return 0;
    }

    else if(comm->is_intercomm)
    {
      return MPI_Comm_free_intercomm(comm);
    }

    else
    {
      int ep_rank_loc, num_ep;

      ep_rank_loc = comm->ep_comm_ptr->size_rank_info[1].first;
      num_ep = comm->ep_comm_ptr->size_rank_info[1].second;

      MPI_Barrier(*comm);

      if(ep_rank_loc == 0)
      {
        Debug("comm is EP, mpi_comm_ptr != NULL\n");

        delete comm->my_buffer;


        if(comm->ep_barrier != NULL)
        {
          comm->ep_barrier->~OMPbarrier();
          Debug("ep_barrier freed\n");
        }


        if( ! comm->rank_map->empty() )
        {
          comm->rank_map->clear();
          Debug("rank_map emptied\n");
        }

        for(int i=0; i<num_ep; i++)
        {
          comm->ep_comm_ptr->comm_list[i].ep_comm_ptr->message_queue->clear();
          Debug("message queue freed\n");
          

          if(comm->ep_comm_ptr->comm_list[i].ep_comm_ptr != NULL)
          {
            delete comm->ep_comm_ptr->comm_list[i].ep_comm_ptr;
            Debug("ep_comm_ptr freed\n");
          }
        }

        if(  comm->mpi_comm != static_cast< ::MPI_Comm>(MPI_COMM_NULL.mpi_comm) 
          && comm->mpi_comm != static_cast< ::MPI_Comm>(MPI_COMM_WORLD.mpi_comm))
        {
          ::MPI_Comm mpi_comm = static_cast< ::MPI_Comm>(comm->mpi_comm);
          ::MPI_Comm_free(&mpi_comm);
          Debug("mpi_comm freed\n");
        }

       if(comm != NULL) {delete[] comm->ep_comm_ptr->comm_list; Debug("comm freed\n");}

      }
      return 0;
    }



  }

  int MPI_Comm_free_intercomm(MPI_Comm *comm)
  {
    int ep_rank_loc, num_ep;

    ep_rank_loc = comm->ep_comm_ptr->size_rank_info[1].first;
    num_ep = comm->ep_comm_ptr->size_rank_info[1].second;

    //MPI_Barrier(*comm);

    if(ep_rank_loc == 0)
    {
      Debug("comm is EP, mpi_comm_ptr != NULL\n");


      if(comm->ep_barrier != NULL)
      {
        comm->ep_barrier->~OMPbarrier();
        Debug("ep_barrier freed\n");
      }


      if( ! comm->rank_map->empty() )
      {
        comm->rank_map->clear();
        Debug("rank_map emptied\n");
      }

      for(int i=0; i<num_ep; i++)
      {
        comm->ep_comm_ptr->comm_list[i].ep_comm_ptr->message_queue->clear();
        Debug("message queue freed\n");

        #pragma omp critical (memory_free)
        if(comm->ep_comm_ptr->comm_list[i].ep_comm_ptr->intercomm != NULL)
        {
          comm->ep_comm_ptr->comm_list[i].ep_comm_ptr->intercomm->local_rank_map->clear();
          comm->ep_comm_ptr->comm_list[i].ep_comm_ptr->intercomm->remote_rank_map->clear();
          comm->ep_comm_ptr->comm_list[i].ep_comm_ptr->intercomm->intercomm_rank_map->clear();
          Debug("intercomm local/remote/intercomm_rank_map emptied\n");
        }  

        if(comm->ep_comm_ptr->comm_list[i].ep_comm_ptr != NULL)
        {
          delete comm->ep_comm_ptr->comm_list[i].ep_comm_ptr;
          Debug("ep_comm_ptr freed\n");
        }
      }

      if(comm->mpi_comm != static_cast< ::MPI_Comm>(MPI_COMM_NULL.mpi_comm))
      {
        ::MPI_Comm mpi_comm = static_cast< ::MPI_Comm>(comm->mpi_comm);
        ::MPI_Comm_free(&mpi_comm);
        Debug("mpi_comm freed\n");
      }

      // if(comm->ep_comm_ptr->intercomm->mpi_inter_comm != MPI_COMM_NULL_STD)
      // {
         // ::MPI_Comm mpi_comm = static_cast< ::MPI_Comm>(comm->ep_comm_ptr->comm_list->ep_comm_ptr->intercomm->mpi_inter_comm);
         // ::MPI_Comm_free(&mpi_comm);
      //   Debug("mpi_intercomm freed\n");
      // }

     if(comm != NULL) {delete[] comm->ep_comm_ptr->comm_list; Debug("comm freed\n");}

    }
    
    return 0;
  }



}


