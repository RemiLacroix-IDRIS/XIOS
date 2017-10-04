#include "ep_lib.hpp"
#include <mpi.h>
#include "ep_declaration.hpp"

namespace ep_lib
{



  int MPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr)
  {
    ::MPI_Alloc_mem(size.mpi_aint, static_cast< ::MPI_Info>(info.mpi_info), baseptr);
    //::MPI_Alloc_mem(size.mpi_aint, MPI_INFO_NULL_STD, baseptr);
    return 0;
   }
   
  int MPI_Alloc_mem(unsigned long size, MPI_Info info, void *baseptr)
  {
    ::MPI_Alloc_mem(size, static_cast< ::MPI_Info>(info.mpi_info), baseptr);
    //::MPI_Alloc_mem(size, MPI_INFO_NULL_STD, baseptr);
    return 0;
  }



}


