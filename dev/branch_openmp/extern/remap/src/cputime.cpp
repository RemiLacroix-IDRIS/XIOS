#include "mpi.hpp"
using namespace ep_lib;

namespace sphereRemap {
  
double cputime()
{
	return MPI_Wtime();
}

}
