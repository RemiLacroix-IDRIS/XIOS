#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"
#include "ep_mpi.hpp"

namespace ep_lib
{

  int MPI_Finalize()
  {
    printf("calling EP Finalize\n");

    int id = omp_get_thread_num();

    if(id == 0)
    {
      ::MPI_Finalize();
    }
    return 0;
  }

  int MPI_Abort(MPI_Comm comm, int errorcode)
  {
    int id = omp_get_thread_num();

    if(id == 0)
    {
      ::MPI_Comm mpi_comm = to_mpi_comm(comm.mpi_comm);
      ::MPI_Abort(mpi_comm, errorcode);
    }
    return 0;
  }

}


