#include "xios_fortran_prefix.hpp"

MODULE IDATA
   USE, INTRINSIC :: ISO_C_BINDING
   USE ICONTEXT
   
   INTERFACE ! Ne pas appeler directement/Interface FORTRAN 2003 <-> C99

      SUBROUTINE  cxios_init_server() BIND(C)
      END SUBROUTINE cxios_init_server

     SUBROUTINE cxios_init_client(client_id, len_client_id, f_local_comm, f_return_comm) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)    , DIMENSION(*) :: client_id
         INTEGER  (kind = C_INT)     , VALUE        :: len_client_id
         INTEGER  (kind = C_INT)                    :: f_local_comm
         INTEGER  (kind = C_INT)                    :: f_return_comm
      END SUBROUTINE cxios_init_client
      
      SUBROUTINE  cxios_context_initialize(context_id,len_context_id,f_comm) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)    , DIMENSION(*) :: context_id
         INTEGER  (kind = C_INT)     , VALUE        :: len_context_id
         INTEGER  (kind = C_INT)                    :: f_comm
      END SUBROUTINE cxios_context_initialize

       SUBROUTINE  cxios_context_close_definition() BIND(C)
         USE ISO_C_BINDING
      END SUBROUTINE cxios_context_close_definition
     

       SUBROUTINE  cxios_context_finalize() BIND(C)
         USE ISO_C_BINDING
      END SUBROUTINE cxios_context_finalize
     
      SUBROUTINE  cxios_init_ioserver(comm_client,comm_parent) BIND(C)
         USE ISO_C_BINDING
         INTEGER  (kind = C_INT) :: comm_client
         INTEGER  (kind = C_INT) :: comm_parent
      END SUBROUTINE cxios_init_ioserver

      SUBROUTINE  cxios_finalize_ioserver BIND(C)
      END SUBROUTINE cxios_finalize_ioserver
 
      SUBROUTINE  cxios_finalize BIND(C)
      END SUBROUTINE cxios_finalize

      SUBROUTINE cxios_dtreatment_start() BIND(C)
         USE ISO_C_BINDING
      END SUBROUTINE cxios_dtreatment_start

      SUBROUTINE cxios_dtreatment_end() BIND(C)
         ! Sans argument
      END SUBROUTINE cxios_dtreatment_end

      SUBROUTINE cxios_write_data_k81(fieldid, fieldid_size, data_k8, data_Xsize) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)  , DIMENSION(*) :: fieldid
         REAL     (kind = C_DOUBLE), DIMENSION(*) :: data_k8
         INTEGER  (kind = C_INT)   , VALUE        :: fieldid_size
         INTEGER  (kind = C_INT)   , VALUE        :: data_Xsize
      END SUBROUTINE cxios_write_data_k81
      
      SUBROUTINE cxios_write_data_k82(fieldid, fieldid_size, data_k8, data_Xsize, data_Ysize) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)  , DIMENSION(*) :: fieldid
         REAL     (kind = C_DOUBLE), DIMENSION(*) :: data_k8
         INTEGER  (kind = C_INT)   , VALUE        :: fieldid_size
         INTEGER  (kind = C_INT)   , VALUE        :: data_Xsize, data_Ysize
      END SUBROUTINE cxios_write_data_k82
      
      SUBROUTINE cxios_write_data_k83(fieldid, fieldid_size, data_k8, data_Xsize, data_Ysize, data_Zsize) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)  , DIMENSION(*) :: fieldid
         REAL     (kind = C_DOUBLE), DIMENSION(*) :: data_k8
         INTEGER  (kind = C_INT)   , VALUE        :: fieldid_size
         INTEGER  (kind = C_INT)   , VALUE        :: data_Xsize, data_Ysize, data_Zsize
      END SUBROUTINE cxios_write_data_k83
      
      SUBROUTINE cxios_write_data_k41(fieldid, fieldid_size, data_k4, data_Xsize) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)  , DIMENSION(*) :: fieldid
         REAL     (kind = C_FLOAT) , DIMENSION(*) :: data_k4
         INTEGER  (kind = C_INT)   , VALUE        :: fieldid_size
         INTEGER  (kind = C_INT)   , VALUE        :: data_Xsize
      END SUBROUTINE cxios_write_data_k41
      
      SUBROUTINE cxios_write_data_k42(fieldid, fieldid_size, data_k4, data_Xsize, data_Ysize) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)  , DIMENSION(*) :: fieldid
         REAL     (kind = C_FLOAT) , DIMENSION(*) :: data_k4
         INTEGER  (kind = C_INT)   , VALUE        :: fieldid_size
         INTEGER  (kind = C_INT)   , VALUE        :: data_Xsize, data_Ysize
      END SUBROUTINE cxios_write_data_k42
      
      SUBROUTINE cxios_write_data_k43(fieldid, fieldid_size, data_k4, data_Xsize, data_Ysize, data_Zsize) BIND(C)
         USE ISO_C_BINDING
         CHARACTER(kind = C_CHAR)  , DIMENSION(*) :: fieldid
         REAL     (kind = C_FLOAT) , DIMENSION(*) :: data_k4
         INTEGER  (kind = C_INT)   , VALUE        :: fieldid_size
         INTEGER  (kind = C_INT)   , VALUE        :: data_Xsize, data_Ysize, data_Zsize
      END SUBROUTINE cxios_write_data_k43
      
   END INTERFACE
   
   INTERFACE write_data
      MODULE PROCEDURE write_data_k81,write_data_k82,write_data_k83,write_data_k41,write_data_k42,write_data_k43
   END INTERFACE
   
   CONTAINS ! Fonctions disponibles pour les utilisateurs.

   SUBROUTINE  xios(init_server)()
   IMPLICIT NONE
     CALL cxios_init_server()
   END SUBROUTINE xios(init_server)
   
   SUBROUTINE  xios(initialize)(client_id, local_comm, return_comm)
   IMPLICIT NONE
   INCLUDE 'mpif.h'
   CHARACTER(LEN=*),INTENT(IN) :: client_id
   INTEGER,INTENT(IN),OPTIONAL         :: local_comm  
   INTEGER,INTENT(OUT),OPTIONAL        :: return_comm
   INTEGER :: f_local_comm
   INTEGER :: f_return_comm
   
      IF (PRESENT(local_comm)) THEN
        f_local_comm=local_comm 
      ELSE
        f_local_comm = MPI_COMM_NULL 
      ENDIF
      
      CALL cxios_init_client(client_id,LEN(client_id),f_local_comm,f_return_comm)
 
      IF (PRESENT(return_comm)) return_comm=f_return_comm

   END SUBROUTINE  xios(initialize)


   SUBROUTINE  xios(context_initialize)(context_id,comm)
   IMPLICIT NONE
   CHARACTER(LEN=*),INTENT(IN)  :: context_id
   INTEGER, INTENT(IN)          :: comm
      
      CALL cxios_context_initialize(context_id,LEN(context_id),comm)
 
    END SUBROUTINE  xios(context_initialize)
    
    
   SUBROUTINE  xios(finalize)
   IMPLICIT NONE

      CALL cxios_finalize

    END SUBROUTINE  xios(finalize)

   
   SUBROUTINE xios(close_context_definition)()
   IMPLICIT NONE
      CALL cxios_context_close_definition()
   END SUBROUTINE xios(close_context_definition)

   
   SUBROUTINE xios(context_finalize)()
   IMPLICIT NONE
      CALL cxios_context_finalize()
   END SUBROUTINE xios(context_finalize)
   

   
   SUBROUTINE xios(send_field_r8_1d)(fieldid, data1d_k8)
   IMPLICIT NONE
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 8), DIMENSION(*), INTENT(IN) :: data1d_k8(:)
      CALL cxios_write_data_k81(fieldid, len(fieldid), data1d_k8, size(data1d_k8, 1))
   END SUBROUTINE xios(send_field_r8_1d)
   
   SUBROUTINE  xios(send_field_r8_2d)(fieldid, data2d_k8)
   IMPLICIT NONE
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 8), DIMENSION(*), INTENT(IN) :: data2d_k8(:,:)
      CALL cxios_write_data_k82(fieldid, len(fieldid), data2d_k8, size(data2d_k8, 1), size(data2d_k8, 2))
   END SUBROUTINE  xios(send_field_r8_2d)
   
   SUBROUTINE  xios(send_field_r8_3d)(fieldid, data3d_k8)
   IMPLICIT NONE
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 8), DIMENSION(*), INTENT(IN) :: data3d_k8(:,:,:)
      CALL cxios_write_data_k83(fieldid, len(fieldid), data3d_k8, size(data3d_k8, 1), size(data3d_k8, 2), size(data3d_k8, 3))
   END SUBROUTINE  xios(send_field_r8_3d)
   
   SUBROUTINE xios(send_field_r4_1d)(fieldid, data1d_k4)
   IMPLICIT NONE
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 4), DIMENSION(*), INTENT(IN) :: data1d_k4(:)
      CALL cxios_write_data_k41(fieldid, len(fieldid), data1d_k4, size(data1d_k4, 1))
   END SUBROUTINE xios(send_field_r4_1d)
   
   SUBROUTINE xios(send_field_r4_2d)(fieldid, data2d_k4)
   IMPLICIT NONE
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 4), DIMENSION(*), INTENT(IN) :: data2d_k4(:,:)
      CALL cxios_write_data_k42(fieldid, len(fieldid), data2d_k4, size(data2d_k4, 1), size(data2d_k4, 2))
   END SUBROUTINE xios(send_field_r4_2d)
   
   SUBROUTINE xios(send_field_r4_3d)(fieldid, data3d_k4)
   IMPLICIT NONE
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 4), DIMENSION(*), INTENT(IN) :: data3d_k4(:,:,:)
      CALL cxios_write_data_k43(fieldid, len(fieldid), data3d_k4, size(data3d_k4, 1), size(data3d_k4, 2), size(data3d_k4, 3))
   END SUBROUTINE xios(send_field_r4_3d)
   



!!!!!!!!!!!!!! anciennes Interfaces !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   SUBROUTINE  init_ioserver(local_comm,return_comm )
   IMPLICIT NONE
   INCLUDE 'mpif.h'
      INTEGER, INTENT(OUT),OPTIONAL :: return_comm
      INTEGER, INTENT(IN),OPTIONAL :: local_comm

      INTEGER  :: comm_client
      INTEGER  :: comm_parent
      
      IF (PRESENT(local_comm)) THEN
        comm_parent=local_comm
      ELSE
        comm_parent=MPI_COMM_WORLD
      ENDIF
      
      CALL cxios_init_ioserver(comm_client,comm_parent)
      IF (PRESENT(return_comm)) return_comm=comm_client ;

    END SUBROUTINE  init_ioserver

   SUBROUTINE  finalize_ioserver
   IMPLICIT NONE

      CALL cxios_finalize_ioserver

    END SUBROUTINE  finalize_ioserver

   
   SUBROUTINE dtreatment_start(context_hdl, filetype)
      TYPE(XContextHandle), INTENT(IN)           :: context_hdl
      INTEGER             , INTENT(IN), OPTIONAL :: filetype 
      INTEGER                                    :: filetype_
      IF (PRESENT(filetype)) THEN
         filetype_ = filetype
      ELSE
         filetype_ = NETCDF4
      END IF
      CALL context_set_current(context_hdl)
      CALL cxios_dtreatment_start()
   END SUBROUTINE dtreatment_start
   
   SUBROUTINE dtreatment_end(context_hdl)
      TYPE(XContextHandle), INTENT(IN), OPTIONAL :: context_hdl
      CALL cxios_dtreatment_end()
   END SUBROUTINE dtreatment_end
   
   SUBROUTINE write_data_k81(fieldid, data1d_k8)
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 8), DIMENSION(*), INTENT(IN) :: data1d_k8(:)
      CALL cxios_write_data_k81(fieldid, len(fieldid), data1d_k8, size(data1d_k8, 1))
   END SUBROUTINE write_data_k81
   
   SUBROUTINE write_data_k82(fieldid, data2d_k8)
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 8), DIMENSION(*), INTENT(IN) :: data2d_k8(:,:)
      CALL cxios_write_data_k82(fieldid, len(fieldid), data2d_k8, size(data2d_k8, 1), size(data2d_k8, 2))
   END SUBROUTINE write_data_k82
   
   SUBROUTINE write_data_k83(fieldid, data3d_k8)
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 8), DIMENSION(*), INTENT(IN) :: data3d_k8(:,:,:)
      CALL cxios_write_data_k83(fieldid, len(fieldid), data3d_k8, size(data3d_k8, 1), size(data3d_k8, 2), size(data3d_k8, 3))
   END SUBROUTINE write_data_k83
   
   SUBROUTINE write_data_k41(fieldid, data1d_k4)
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 4), DIMENSION(*), INTENT(IN) :: data1d_k4(:)
      CALL cxios_write_data_k41(fieldid, len(fieldid), data1d_k4, size(data1d_k4, 1))
   END SUBROUTINE write_data_k41
   
   SUBROUTINE write_data_k42(fieldid, data2d_k4)
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 4), DIMENSION(*), INTENT(IN) :: data2d_k4(:,:)
      CALL cxios_write_data_k42(fieldid, len(fieldid), data2d_k4, size(data2d_k4, 1), size(data2d_k4, 2))
   END SUBROUTINE write_data_k42
   
   SUBROUTINE write_data_k43(fieldid, data3d_k4)
      CHARACTER(len = *)               , INTENT(IN) :: fieldid
      REAL     (kind = 4), DIMENSION(*), INTENT(IN) :: data3d_k4(:,:,:)
      CALL cxios_write_data_k43(fieldid, len(fieldid), data3d_k4, size(data3d_k4, 1), size(data3d_k4, 2), size(data3d_k4, 3))
   END SUBROUTINE write_data_k43
   
END MODULE IDATA
