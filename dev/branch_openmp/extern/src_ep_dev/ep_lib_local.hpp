#ifndef EP_LIB_LOCAL_HPP_INCLUDED
#define EP_LIB_LOCAL_HPP_INCLUDED

namespace ep_lib
{
  #ifdef _intelmpi
  typedef int MPI_Datatype;
  typedef int MPI_Op;
  #elif _openmpi
  typedef void* MPI_Datatype;
  typedef void* MPI_Op;
  #endif

  int MPI_Reduce_local       (const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int local_root, MPI_Comm comm);

  int MPI_Scan_local       (const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

  int MPI_Exscan_local       (const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

  int MPI_Bcast_local      (void *buffer, int count, MPI_Datatype datatype, int local_root, MPI_Comm comm);
  
  int MPI_Gather_local      (const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf, int local_root, MPI_Comm comm);

  int MPI_Gatherv_local       (const void *sendbuf, int count, MPI_Datatype datatype, void *recvbuf,
                               const int recvcounts[], const int displs[], int local_root, MPI_Comm comm);

  int MPI_Scatter_local(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int local_root, MPI_Comm comm);

  int MPI_Scatterv_local(const void *sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount,
                   MPI_Datatype recvtype, int local_root, MPI_Comm comm);

  int MPI_Barrier_local(MPI_Comm comm);

}

#endif // EP_LIB_LOCAL_HPP_INCLUDED
