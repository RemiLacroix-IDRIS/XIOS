PROGRAM server_main
  USE xios
  USE mod_wait
  IMPLICIT NONE
  INCLUDE "mpif.h"
  INTEGER :: ierr
    
    CALL MPI_INIT(ierr)
    CALL init_wait
    CALL xios_init_server

  END PROGRAM server_main
