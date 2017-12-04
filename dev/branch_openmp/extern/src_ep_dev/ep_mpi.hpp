#ifndef EP_MPI_HPP_INCLUDED
#define EP_MPI_HPP_INCLUDED

#include "ep_type.hpp"

MPI_Datatype to_mpi_type(ep_lib::MPI_Datatype type);
MPI_Op       to_mpi_op(ep_lib::MPI_Op op);
MPI_Comm     to_mpi_comm(void* comm);

#endif // EP_MPI_HPP_INCLUDED
