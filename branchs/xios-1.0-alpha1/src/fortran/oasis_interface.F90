SUBROUTINE fxios_oasis_init(server_id,str_len) BIND(C,NAME="fxios_oasis_init")
  USE, INTRINSIC :: ISO_C_BINDING
#ifdef USE_OASIS
  USE mod_prism_proto
#endif
    CHARACTER(kind = C_CHAR),DIMENSION(*) :: server_id
    INTEGER(kind = C_INT),VALUE   :: str_len
    
    INTEGER                :: comp_id
    CHARACTER(len=str_len) :: oasis_server_id
    INTEGER                :: ierr
    INTEGER :: i
    
    DO i=1,str_len
      oasis_server_id(i:i)=server_id(i)
    ENDDO

#ifdef USE_OASIS
    CALL prism_init_comp_proto (comp_id, oasis_server_id, ierr)
#endif
    PRINT *,"---> prism_init",oasis_server_id,ierr

END SUBROUTINE fxios_oasis_init


SUBROUTINE fxios_oasis_finalize BIND(C,NAME="fxios_oasis_finalize")
  USE, INTRINSIC :: ISO_C_BINDING
#ifdef USE_OASIS
  USE mod_prism_proto
#endif
  IMPLICIT NONE
  INTEGER :: ierr
  
#ifdef USE_OASIS
    CALL prism_terminate_proto(ierr)
#endif
    
END SUBROUTINE fxios_oasis_finalize


SUBROUTINE fxios_oasis_get_intracomm(f_comm_client_server,client_id,str_len) BIND(C,NAME="fxios_oasis_get_intracomm")
  USE, INTRINSIC :: ISO_C_BINDING
#ifdef USE_OASIS
  USE mod_prism_get_comm 
#endif
  IMPLICIT NONE
  INTEGER(kind=C_INT) :: f_comm_client_server
  CHARACTER,DIMENSION(*) :: client_id
  INTEGER,VALUE          :: str_len
  
  INTEGER :: comm_client_server
  CHARACTER(len=str_len) :: oasis_client_id
  INTEGER :: ierr
  INTEGER :: i
    
    DO i=1,str_len
      oasis_client_id(i:i)=client_id(i)
    ENDDO
    
#ifdef USE_OASIS
    CALL prism_get_intracomm(comm_client_server,oasis_client_id,ierr)
#endif
    PRINT *,"---> prism_get_intracomm ",oasis_client_id,comm_client_server,ierr
    f_comm_client_server=comm_client_server

END SUBROUTINE fxios_oasis_get_intracomm
