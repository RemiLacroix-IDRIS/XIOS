/*!
   \file ep_send.hpp
   \since 2 may 2016

   \brief Definitions of MPI send functions: MPI_Send, MPI_Ssend, MPI_Isend, MPI_Issend
 */

#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"


namespace ep_lib {


  int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
  {
    if(!comm.is_ep)
      return ::MPI_Send(buf, count, to_mpi_type(datatype), dest, tag, to_mpi_comm(comm.mpi_comm));
    if(comm.is_intercomm)
    {
      MPI_Request request;
      MPI_Status status;
      MPI_Isend(buf, count, datatype, dest, tag, comm, &request);
      MPI_Wait(&request, &status);
    }
    else
    {
      int ep_src_loc  = comm.ep_comm_ptr->size_rank_info[1].first;
      int ep_dest_loc = comm.ep_comm_ptr->comm_list->rank_map->at(dest).first;
      int mpi_tag     = tag_combine(tag, ep_src_loc, ep_dest_loc);
      int mpi_dest    = comm.ep_comm_ptr->comm_list->rank_map->at(dest).second;

      ::MPI_Send(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.mpi_comm));
      //printf("call mpi_send for intracomm, dest = %d, tag = %d\n", dest, tag);
    }
    //check_sum_send(buf, count, datatype, dest, tag, comm);

    return 0;
  }


  int MPI_Ssend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
  {
    if(!comm.is_ep)
    {
      return ::MPI_Ssend(buf, count, to_mpi_type(datatype), dest, tag, to_mpi_comm(comm.mpi_comm));
    }

    MPI_Request request;
    MPI_Status status;
    MPI_Issend(buf, count, datatype, dest, tag, comm, &request);
    MPI_Wait(&request, &status);
    //check_sum_send(buf, count, datatype, dest, tag, comm);
    return 0;
  }
  




  int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
  {
    Debug("\nMPI_Isend with EP\n");
    int src_rank;
    MPI_Comm_rank(comm, &src_rank);

    

    if(!comm.is_ep)
    {
      ::MPI_Request mpi_request;
      ::MPI_Isend(buf, count, to_mpi_type(datatype), dest, tag, to_mpi_comm(comm.mpi_comm), &mpi_request);

      request->mpi_request = mpi_request;

      request->ep_src = src_rank;
      request->ep_tag = tag;
      request->ep_datatype = datatype;
      request->type = 1;
      request->comm = comm;

      return 0;
    }

    if(comm.is_intercomm) return MPI_Isend_intercomm(buf, count, datatype, dest, tag, comm, request);

    // EP intracomm

    //check_sum_send(buf, count, datatype, dest, tag, comm, 1);

    int ep_src_loc  = comm.ep_comm_ptr->size_rank_info[1].first;
    int ep_dest_loc = comm.ep_comm_ptr->comm_list->rank_map->at(dest).first;
    int mpi_tag     = tag_combine(tag, ep_src_loc, ep_dest_loc);
    int mpi_dest    = comm.ep_comm_ptr->comm_list->rank_map->at(dest).second;

    request->ep_src  = src_rank;
    request->ep_tag  = tag;
    request->ep_datatype = datatype;

    ::MPI_Request mpi_request;

    ::MPI_Isend(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.mpi_comm), &mpi_request);

    request->mpi_request = mpi_request;
    request->type = 1;    // used in wait
    request->comm = comm;
    request->buf = const_cast<void*>(buf);

    //Message_Check(comm);

    return 0;
  }




  int MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
  {
    Debug("\nMPI_Issend with EP\n");

    int src_rank;
    MPI_Comm_rank(comm, &src_rank);

    

    if(!comm.is_ep)
    {
      ::MPI_Request mpi_request;
      ::MPI_Issend(buf, count, to_mpi_type(datatype), dest, tag, to_mpi_comm(comm.mpi_comm), &mpi_request);

      request->mpi_request = mpi_request;
      request->ep_src = src_rank;
      request->ep_tag = tag;
      request->ep_datatype = datatype;
      request->type = 1;
      request->comm = comm;

      return 0;
    }

    if(comm.is_intercomm) return MPI_Issend_intercomm(buf, count, datatype, dest, tag, comm, request);

    // EP intracomm

    //check_sum_send(buf, count, datatype, dest, tag, comm, 1);

    int ep_src_loc  = comm.ep_comm_ptr->size_rank_info[1].first;
    int ep_dest_loc = comm.ep_comm_ptr->comm_list->rank_map->at(dest).first;
    int mpi_tag     = tag_combine(tag, ep_src_loc, ep_dest_loc);
    int mpi_dest    = comm.ep_comm_ptr->comm_list->rank_map->at(dest).second;
    
    request->ep_src = src_rank;
    request->ep_tag = tag;
    request->ep_datatype = datatype;

    ::MPI_Request mpi_request;

    ::MPI_Issend(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.mpi_comm), &mpi_request);

    request->mpi_request = mpi_request;
    request->type = 1;    // used in wait
    request->comm = comm;
    request->buf = NULL;
    

    //Message_Check(comm);

    return 0;
  }
  



  int MPI_Isend_intercomm(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
  {
    Debug("MPI_Isend with intercomm\n");

    //check_sum_send(buf, count, datatype, dest, tag, comm, 1);

    int dest_remote_ep_rank    = comm.ep_comm_ptr->intercomm->remote_rank_map->at(dest).first;
    int dest_remote_comm_label = comm.ep_comm_ptr->intercomm->remote_rank_map->at(dest).second;

    int src_ep_rank    = comm.ep_comm_ptr->intercomm->size_rank_info[0].first;
    int src_comm_label;

    src_comm_label = comm.ep_comm_ptr->intercomm->local_rank_map->at(src_ep_rank).second;


    

    //Message_Check(comm);


    if(dest_remote_comm_label == src_comm_label)       // mpi_dest differs
    {
      int inter_src = comm.ep_comm_ptr->intercomm->local_rank_map->at(src_ep_rank).first;
      int ep_src_loc = comm.rank_map->at(inter_src).first;
      int ep_dest_loc = comm.rank_map->at(dest_remote_ep_rank).first;
      int mpi_dest    = comm.rank_map->at(dest_remote_ep_rank).second;
      int mpi_tag = tag_combine(tag, ep_src_loc, ep_dest_loc);

      ::MPI_Request mpi_request;
  
      ::MPI_Isend(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.mpi_comm), &mpi_request);

      request->mpi_request = mpi_request;
      request->type = 1;    // used in wait
      request->comm = comm;

      request->ep_src = src_ep_rank;
      request->ep_tag = tag;
      request->ep_datatype = datatype;
    }

    else   // dest_remote_comm_label != src_comm_label
    { 
      int inter_src = comm.ep_comm_ptr->intercomm->local_rank_map->at(src_ep_rank).first;
      int ep_src_loc = comm.rank_map->at(inter_src).first;
      int ep_dest_loc = comm.ep_comm_ptr->intercomm->intercomm_rank_map->at(dest_remote_ep_rank).first;
      int mpi_dest    = comm.ep_comm_ptr->intercomm->intercomm_rank_map->at(dest_remote_ep_rank).second;
      int mpi_tag = tag_combine(tag, ep_src_loc, ep_dest_loc);

      ::MPI_Request mpi_request;

      ::MPI_Isend(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.ep_comm_ptr->intercomm->mpi_inter_comm), &mpi_request);

      request->mpi_request = mpi_request;
      request->type = 1;    // used in wait
      request->comm = comm;
   
      request->ep_src = src_ep_rank;
      request->ep_tag = tag;
      request->ep_datatype = datatype;
    }

    return 0;

  }


  int MPI_Issend_intercomm(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
  {
    Debug("MPI_Issend with intercomm\n");

    //check_sum_send(buf, count, datatype, dest, tag, comm, 1);

    int dest_remote_ep_rank    = comm.ep_comm_ptr->intercomm->remote_rank_map->at(dest).first;
    int dest_remote_comm_label = comm.ep_comm_ptr->intercomm->remote_rank_map->at(dest).second;

    int src_ep_rank    = comm.ep_comm_ptr->intercomm->size_rank_info[0].first;
    int src_comm_label;

    for(int i=0; i<comm.ep_comm_ptr->intercomm->local_rank_map->size(); i++)
    {
      if(comm.ep_comm_ptr->intercomm->local_rank_map->at(i).first == src_ep_rank)
      {
        src_comm_label = comm.ep_comm_ptr->intercomm->local_rank_map->at(i).second;
        break;
      }
    }

    //Message_Check(comm);


    if(dest_remote_comm_label == src_comm_label)       // dest rank (loc, mpi) differs
    {
      int inter_src = comm.ep_comm_ptr->intercomm->local_rank_map->at(src_ep_rank).first;
      int ep_src_loc = comm.rank_map->at(inter_src).first;
      int ep_dest_loc = comm.rank_map->at(dest_remote_ep_rank).first;
      int mpi_dest    = comm.rank_map->at(dest_remote_ep_rank).second;
      int mpi_tag = tag_combine(tag, ep_src_loc, ep_dest_loc);

      ::MPI_Request mpi_request;
  
      ::MPI_Issend(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.mpi_comm), &mpi_request);

      request->mpi_request = mpi_request;
      request->type = 1;    // used in wait
      request->comm = comm;

      request->ep_src = src_ep_rank;
      request->ep_tag = tag;
      request->ep_datatype = datatype;
    }

    else   // dest_remote_comm_label != src_comm_label
    { 
      int inter_src = comm.ep_comm_ptr->intercomm->local_rank_map->at(src_ep_rank).first;
      int ep_src_loc = comm.rank_map->at(inter_src).first;
      int ep_dest_loc = comm.ep_comm_ptr->intercomm->intercomm_rank_map->at(dest_remote_ep_rank).first;
      int mpi_dest    = comm.ep_comm_ptr->intercomm->intercomm_rank_map->at(dest_remote_ep_rank).second;
      int mpi_tag = tag_combine(tag, ep_src_loc, ep_dest_loc);

      ::MPI_Request mpi_request;

      ::MPI_Issend(buf, count, to_mpi_type(datatype), mpi_dest, mpi_tag, to_mpi_comm(comm.ep_comm_ptr->intercomm->mpi_inter_comm), &mpi_request);

      request->mpi_request = mpi_request;
      request->type = 1;    // used in wait
      request->comm = comm;
   
      request->ep_src = src_ep_rank;
      request->ep_tag = tag;
      request->ep_datatype = datatype;
    }

    return 0;

  }
  
}






