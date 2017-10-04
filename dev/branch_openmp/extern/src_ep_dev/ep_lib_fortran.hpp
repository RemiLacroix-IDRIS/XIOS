#ifndef EP_LIB_FORTRAN_HPP_INCLUDED
#define EP_LIB_FORTRAN_HPP_INCLUDED

#include "ep_type.hpp"

namespace ep_lib
{
  
  // #ifdef _intelmpi
  
  // MPI_Fint MPI_Comm_c2f(MPI_Comm comm);
  // MPI_Comm MPI_Comm_f2c(MPI_Fint comm);
  
  // #elif _openmpi
  
  // int MPI_Comm_c2f(MPI_Comm comm);
  // ep_lib::MPI_Comm MPI_Comm_f2c(MPI_Fint comm);
  
  // #endif

  int EP_Comm_c2f(MPI_Comm comm);
  MPI_Comm EP_Comm_f2c(int comm);
}


#endif // EP_LIB_FORTRAN_HPP_INCLUDED
