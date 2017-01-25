#ifndef __XIOS_MPI_HPP__
#define __XIOS_MPI_HPP__

/* skip C++ Binding for mpich , intel MPI */
#define MPICH_SKIP_MPICXX

/* skip C++ Binding for SGI MPI library */
#define MPI_NO_CPPBIND

/* skip C++ Binding for OpenMPI */
#define OMPI_SKIP_MPICXX

#ifdef _usingEP
  #include "../extern/src_ep/ep_lib.hpp"
  using namespace ep_lib;
#elif _usingMPI
  #include <mpi.h>
#endif


#endif
