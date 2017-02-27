PROGRAM server_main
  USE xios
  USE mod_wait
  IMPLICIT NONE
  INCLUDE "mpif.h"
  INTEGER :: ierr
    
    CALL MPI_INIT(ierr)
    CALL init_wait
    CALL xios_init_server
    
    CALL MPI_FINALIZE(ierr)

  END PROGRAM server_main
