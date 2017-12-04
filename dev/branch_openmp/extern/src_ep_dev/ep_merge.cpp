#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

using namespace std;


namespace ep_lib {

  int MPI_Intercomm_merge_unique_leader(MPI_Comm inter_comm, bool high, MPI_Comm *newintracomm)
  {
    Debug("intercomm_merge with unique leader\n");



    int ep_rank, ep_rank_loc, mpi_rank;
    int ep_size, num_ep, mpi_size;

    ep_rank = inter_comm.ep_comm_ptr->size_rank_info[0].first;
    ep_rank_loc = inter_comm.ep_comm_ptr->size_rank_info[1].first;
    mpi_rank = inter_comm.ep_comm_ptr->size_rank_info[2].first;
    ep_size = inter_comm.ep_comm_ptr->size_rank_info[0].second;
    num_ep = inter_comm.ep_comm_ptr->size_rank_info[1].second;
    mpi_size = inter_comm.ep_comm_ptr->size_rank_info[2].second;

    int local_high = high;
    int remote_high;

    int remote_ep_size = inter_comm.ep_comm_ptr->intercomm->remote_rank_map->size();

    int local_ep_rank, local_ep_rank_loc, local_mpi_rank;
    int local_ep_size, local_num_ep, local_mpi_size;

    local_ep_rank = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[0].first;
    local_ep_rank_loc = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[1].first;
    local_mpi_rank = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[2].first;
    local_ep_size = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[0].second;
    local_num_ep = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[1].second;
    local_mpi_size = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[2].second;


    if(local_ep_rank == 0)
    {
      MPI_Status status[2];
      MPI_Request request[2];
      MPI_Isend(&local_high, 1, MPI_INT, 0, inter_comm.ep_comm_ptr->intercomm->intercomm_tag, inter_comm, &request[0]);
      MPI_Irecv(&remote_high, 1, MPI_INT, 0, inter_comm.ep_comm_ptr->intercomm->intercomm_tag, inter_comm, &request[1]);

      MPI_Waitall(2, request, status);
    }


    MPI_Bcast(&remote_high, 1, MPI_INT, 0, *(inter_comm.ep_comm_ptr->intercomm->local_comm));



    MPI_Comm_dup(inter_comm, newintracomm);

    int my_ep_rank = local_high<remote_high? local_ep_rank: local_ep_rank+remote_ep_size;


    int intra_ep_rank, intra_ep_rank_loc, intra_mpi_rank;
    int intra_ep_size, intra_num_ep, intra_mpi_size;

    intra_ep_rank = newintracomm->ep_comm_ptr->size_rank_info[0].first;
    intra_ep_rank_loc = newintracomm->ep_comm_ptr->size_rank_info[1].first;
    intra_mpi_rank = newintracomm->ep_comm_ptr->size_rank_info[2].first;
    intra_ep_size = newintracomm->ep_comm_ptr->size_rank_info[0].second;
    intra_num_ep = newintracomm->ep_comm_ptr->size_rank_info[1].second;
    intra_mpi_size = newintracomm->ep_comm_ptr->size_rank_info[2].second;


    MPI_Barrier_local(*newintracomm);


    int *reorder;
    if(intra_ep_rank_loc == 0)
    {
      reorder = new int[intra_ep_size];
    }


    MPI_Gather(&my_ep_rank, 1, MPI_INT, reorder, 1, MPI_INT, 0, *newintracomm);
    if(intra_ep_rank_loc == 0)
    {
      ::MPI_Bcast(reorder, intra_ep_size, to_mpi_type(MPI_INT), 0, to_mpi_comm(newintracomm->mpi_comm));

      vector< pair<int, int> > tmp_rank_map(intra_ep_size);


      for(int i=0; i<intra_ep_size; i++)
      {
        tmp_rank_map[reorder[i]] = newintracomm->rank_map->at(i) ;
      }

      newintracomm->rank_map->swap(tmp_rank_map);

      tmp_rank_map.clear();
    }

    MPI_Barrier_local(*newintracomm);

    (*newintracomm).ep_comm_ptr->size_rank_info[0].first = my_ep_rank;

    if(intra_ep_rank_loc == 0)
    {
      delete[] reorder;
    }

    return MPI_SUCCESS;
  }





  int MPI_Intercomm_merge(MPI_Comm inter_comm, bool high, MPI_Comm *newintracomm)
  {

    assert(inter_comm.is_intercomm);

    if(inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->comm_label == -99)
    {
        return MPI_Intercomm_merge_unique_leader(inter_comm, high, newintracomm);
    }


    Debug("intercomm_merge kernel\n");

    int ep_rank_loc;
    int num_ep;

    ep_rank_loc = inter_comm.ep_comm_ptr->size_rank_info[1].first;
    num_ep = inter_comm.ep_comm_ptr->size_rank_info[1].second;



    int remote_ep_size = inter_comm.ep_comm_ptr->intercomm->remote_rank_map->size();


    MPI_Barrier(inter_comm);


    ::MPI_Comm *mpi_intracomm = new ::MPI_Comm;
    MPI_Comm *ep_intracomm;

    if(ep_rank_loc == 0)
    {

      ::MPI_Comm mpi_comm = to_mpi_comm(inter_comm.ep_comm_ptr->intercomm->mpi_inter_comm);

      ::MPI_Intercomm_merge(mpi_comm, high, mpi_intracomm);
      MPI_Info info;
      MPI_Comm_create_endpoints(mpi_intracomm, num_ep, info, ep_intracomm);

      inter_comm.ep_comm_ptr->comm_list->mem_bridge = ep_intracomm;

    }

    MPI_Barrier_local(inter_comm);

    int inter_rank;
    MPI_Comm_rank(inter_comm, &inter_rank);
    
    int my_ep_rank = high? inter_rank+remote_ep_size : inter_rank;
    int my_ep_rank_loc = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[1].first;
    int my_num_ep_loc = inter_comm.ep_comm_ptr->intercomm->local_comm->ep_comm_ptr->size_rank_info[1].second;
    int my_num_ep_total = inter_comm.ep_comm_ptr->comm_list->mem_bridge[0].ep_comm_ptr->size_rank_info[1].second;
    int my_ep_size = inter_comm.ep_comm_ptr->comm_list->mem_bridge[0].ep_comm_ptr->size_rank_info[0].second;

    int tmp_intra_ep_rank_loc = high?my_ep_rank_loc+my_num_ep_total-my_num_ep_loc: my_ep_rank_loc;


    *newintracomm = inter_comm.ep_comm_ptr->comm_list->mem_bridge[tmp_intra_ep_rank_loc];

    int newintracomm_ep_rank = (*newintracomm).ep_comm_ptr->size_rank_info[0].first;
    int newintracomm_ep_rank_loc = (*newintracomm).ep_comm_ptr->size_rank_info[1].first;
    int newintracomm_mpi_rank = (*newintracomm).ep_comm_ptr->size_rank_info[2].first;
    int newintracomm_ep_size = (*newintracomm).ep_comm_ptr->size_rank_info[0].second;
    int newintracomm_num_ep = (*newintracomm).ep_comm_ptr->size_rank_info[1].second;
    int newintracomm_mpi_size = (*newintracomm).ep_comm_ptr->size_rank_info[2].second;


    int buf[3];
    buf[0] = my_ep_rank;
    buf[1] = tmp_intra_ep_rank_loc;
    buf[2] = newintracomm->ep_comm_ptr->size_rank_info[2].first;

    // printf("my_ep_rank = %d, tmp_intra_ep_rank_loc = %d, mpi_rank = %d\n", my_ep_rank, tmp_intra_ep_rank_loc, newintracomm->ep_comm_ptr->size_rank_info[2].first);

    int *rankmap_buf;
    rankmap_buf = new int [3*my_ep_size];

    MPI_Allgather(buf, 3, MPI_INT, rankmap_buf, 3, MPI_INT, *newintracomm);

    
    // printf(" ID = %d : rankmap_buf = (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d), (%d %d %d)\n", newintracomm_ep_rank,
    //                     rankmap_buf[0], rankmap_buf[1], rankmap_buf[2], rankmap_buf[3], rankmap_buf[4], rankmap_buf[5], rankmap_buf[6], rankmap_buf[7], rankmap_buf[8], rankmap_buf[9],
    //                     rankmap_buf[10], rankmap_buf[11], rankmap_buf[12], rankmap_buf[13], rankmap_buf[14], rankmap_buf[15], rankmap_buf[16], rankmap_buf[17], rankmap_buf[18], rankmap_buf[19], 
    //                     rankmap_buf[20], rankmap_buf[21], rankmap_buf[22], rankmap_buf[23], rankmap_buf[24], rankmap_buf[25], rankmap_buf[26], rankmap_buf[27], rankmap_buf[28], rankmap_buf[29],
    //                     rankmap_buf[30], rankmap_buf[31], rankmap_buf[32], rankmap_buf[33], rankmap_buf[34], rankmap_buf[35], rankmap_buf[36], rankmap_buf[37], rankmap_buf[38], rankmap_buf[39],
    //                     rankmap_buf[40], rankmap_buf[41], rankmap_buf[42], rankmap_buf[43], rankmap_buf[44], rankmap_buf[45], rankmap_buf[46], rankmap_buf[47]);


    for(int i=0; i<newintracomm_ep_size; i++)
    {
      (*newintracomm).rank_map->at(rankmap_buf[3*i]).first  = rankmap_buf[3*i+1];
      (*newintracomm).rank_map->at(rankmap_buf[3*i]).second = rankmap_buf[3*i+2];
    }


    (*newintracomm).ep_comm_ptr->size_rank_info[0].first = my_ep_rank;
    (*newintracomm).ep_comm_ptr->size_rank_info[1].first = tmp_intra_ep_rank_loc;
    

    delete[] rankmap_buf;

    
    return MPI_SUCCESS;

  }


}
