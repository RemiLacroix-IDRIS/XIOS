#include "ep_lib.hpp"
#include <mpi.h>
//#include "ep_declaration.hpp"

namespace ep_lib
{



  int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
  {
    assert(static_cast< ::MPI_Datatype>(sendtype) == static_cast< ::MPI_Datatype>(recvtype));
    ::MPI_Aint typesize, llb;
    ::MPI_Type_get_extent(static_cast< ::MPI_Datatype>(sendtype), &llb, &typesize);
    
    int ep_size;
    MPI_Comm_size(comm, &ep_size);

    for(int i=0; i<ep_size; i++)
    {
      MPI_Gather(sendbuf+i*sendcount*typesize, sendcount, sendtype, recvbuf, recvcount, recvtype, i, comm);
    }

    return 0;
  }

}


