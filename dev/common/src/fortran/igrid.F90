#include "xios_fortran_prefix.hpp"

MODULE IGRID
   USE, INTRINSIC :: ISO_C_BINDING
   USE GRID_INTERFACE
   USE GRIDGROUP_INTERFACE
   
   TYPE XGridHandle
      INTEGER(kind = C_INTPTR_T) :: daddr
   END TYPE XGridHandle
   
   TYPE XGridGroupHandle
      INTEGER(kind = C_INTPTR_T) :: daddr
   END TYPE XGridGroupHandle

   TYPE txios(grid)
      INTEGER(kind = C_INTPTR_T) :: daddr
   END TYPE txios(grid)
   
   TYPE txios(gridgroup)
      INTEGER(kind = C_INTPTR_T) :: daddr
   END TYPE txios(gridgroup)
   
   !----------------------------------------------------------------------------
   INTERFACE set_grid_attributes
      MODULE PROCEDURE set_grid_attributes_id,set_grid_attributes_hdl
   END INTERFACE  
   
   INTERFACE set_grid_group_attributes
      MODULE PROCEDURE set_gridgroup_attributes_id,set_gridgroup_attributes_hdl
   END INTERFACE  
   !----------------------------------------------------------------------------
   
   CONTAINS ! Fonctions disponibles pour les utilisateurs.



   SUBROUTINE xios(set_grid_attr)(grid_id, name, description, domain_ref, axis_ref)
      IMPLICIT NONE
      TYPE(txios(grid))                             :: grid_hdl
      CHARACTER(len = *)               , INTENT(IN) :: grid_id
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref
      
      CALL xios(get_grid_handle)(grid_id,grid_hdl)
      CALL xios(set_grid_attr_hdl_)(grid_hdl, name, description, domain_ref, axis_ref)

   END SUBROUTINE xios(set_grid_attr)
   
   SUBROUTINE xios(set_grid_attr_hdl)(grid_hdl, name, description, domain_ref, axis_ref)
      IMPLICIT NONE
      TYPE      (txios(grid))                      :: grid_hdl
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref
      
      CALL xios(set_grid_attr_hdl_)(grid_hdl, name, description, domain_ref, axis_ref)

   END SUBROUTINE xios(set_grid_attr_hdl)   


   SUBROUTINE xios(set_grid_attr_hdl_)(grid_hdl, name_, description_, domain_ref_, axis_ref_)
      IMPLICIT NONE
      TYPE      (txios(grid))                      :: grid_hdl
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref_
      
      IF (PRESENT(name_))        THEN
       CALL cxios_set_grid_name(grid_hdl%daddr, name_, len(name_))
      END IF
      IF (PRESENT(description_)) THEN
       CALL cxios_set_grid_description(grid_hdl%daddr, description_, len(description_))
      END IF
      IF (PRESENT(domain_ref_))  THEN
       CALL cxios_set_grid_domain_ref(grid_hdl%daddr, domain_ref_, len(domain_ref_))
      END IF
      IF (PRESENT(axis_ref_))    THEN
       CALL cxios_set_grid_axis_ref(grid_hdl%daddr, axis_ref_, len(axis_ref_))
      END IF
   END SUBROUTINE xios(set_grid_attr_hdl_)


   
   SUBROUTINE xios(set_gridgroup_attr)(gridgroup_id, name, description, domain_ref, axis_ref)
      IMPLICIT NONE
      TYPE(txios(gridgroup))                       :: gridgroup_hdl
      CHARACTER(len = *)               , INTENT(IN) :: gridgroup_id
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref
      
      CALL xios(get_gridgroup_handle)(gridgroup_id, gridgroup_hdl)
      CALL xios(set_gridgroup_attr_hdl_)(gridgroup_hdl, name, description, domain_ref, axis_ref)

   END SUBROUTINE xios(set_gridgroup_attr)
   
   SUBROUTINE xios(set_gridgroup_attr_hdl)(gridgroup_hdl, name, description, domain_ref, axis_ref)
      IMPLICIT NONE
      TYPE      (txios(gridgroup))                 :: gridgroup_hdl
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref

      CALL xios(set_gridgroup_attr_hdl_)(gridgroup_hdl, name, description, domain_ref, axis_ref)

   END SUBROUTINE xios(set_gridgroup_attr_hdl)
   
   SUBROUTINE xios(set_gridgroup_attr_hdl_)(gridgroup_hdl, name_, description_, domain_ref_, axis_ref_)
      IMPLICIT NONE
      TYPE      (txios(gridgroup))                 :: gridgroup_hdl
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref_
      
      IF (PRESENT(name_))        THEN
       CALL cxios_set_gridgroup_name(gridgroup_hdl%daddr, name_, len(name_))
      END IF
      IF (PRESENT(description_)) THEN
       CALL cxios_set_gridgroup_description(gridgroup_hdl%daddr, description_, len(description_))
      END IF
      IF (PRESENT(domain_ref_))  THEN
       CALL cxios_set_gridgroup_domain_ref(gridgroup_hdl%daddr, domain_ref_, len(domain_ref_))
      END IF
      IF (PRESENT(axis_ref_))    THEN
       CALL cxios_set_gridgroup_axis_ref(gridgroup_hdl%daddr, axis_ref_, len(axis_ref_))
      END IF
   END SUBROUTINE xios(set_gridgroup_attr_hdl_)
   
   

   SUBROUTINE xios(get_grid_handle)(idt,ret)
      IMPLICIT NONE
      CHARACTER(len = *), INTENT(IN) :: idt      
      TYPE(txios(grid)), INTENT(OUT):: ret

      CALL cxios_grid_handle_create(ret%daddr, idt, len(idt))            

   END SUBROUTINE xios(get_grid_handle)
   
   SUBROUTINE xios(get_gridgroup_handle)(idt,ret)
      IMPLICIT NONE
      CHARACTER(len = *)     , INTENT(IN) :: idt      
      TYPE(txios(gridgroup))     , INTENT(OUT):: ret

      CALL cxios_gridgroup_handle_create(ret%daddr, idt, len(idt))            

   END SUBROUTINE xios(get_gridgroup_handle)

   LOGICAL FUNCTION xios(is_valid_grid)(idt)
      IMPLICIT NONE
      CHARACTER(len  = *)    , INTENT(IN) :: idt
      LOGICAL  (kind = 1)                 :: val

      CALL cxios_grid_valid_id(val, idt, len(idt));
      xios(is_valid_grid) = val

   END FUNCTION  xios(is_valid_grid)

   LOGICAL FUNCTION xios(is_valid_gridgroup)(idt)
      IMPLICIT NONE
      CHARACTER(len  = *)    , INTENT(IN) :: idt
      LOGICAL  (kind = 1)                 :: val

      CALL cxios_gridgroup_valid_id(val, idt, len(idt));
      xios(is_valid_gridgroup) = val

   END FUNCTION  xios(is_valid_gridgroup)





















!!!!!!!!!!!!!!!!!!!!  Anciennes interfaces !!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   
   SUBROUTINE set_grid_attributes_id(grid_id, name_, description_, domain_ref_, axis_ref_)
      IMPLICIT NONE
      TYPE(XGridHandle)                             :: grid_hdl
      CHARACTER(len = *)               , INTENT(IN) :: grid_id
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref_
      
      CALL grid_handle_create(grid_hdl, grid_id)
      CALL set_grid_attributes_hdl(grid_hdl, name_, description_, domain_ref_, axis_ref_)

   END SUBROUTINE set_grid_attributes_id

   SUBROUTINE set_grid_attributes_hdl(grid_hdl, name_, description_, domain_ref_, axis_ref_)
      IMPLICIT NONE
      TYPE      (XGridHandle)                       :: grid_hdl
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref_
      
      IF (PRESENT(name_))        THEN
       CALL cxios_set_grid_name(grid_hdl%daddr, name_, len(name_))
      END IF
      IF (PRESENT(description_)) THEN
       CALL cxios_set_grid_description(grid_hdl%daddr, description_, len(description_))
      END IF
      IF (PRESENT(domain_ref_))  THEN
       CALL cxios_set_grid_domain_ref(grid_hdl%daddr, domain_ref_, len(domain_ref_))
      END IF
      IF (PRESENT(axis_ref_))    THEN
       CALL cxios_set_grid_axis_ref(grid_hdl%daddr, axis_ref_, len(axis_ref_))
      END IF
   END SUBROUTINE set_grid_attributes_hdl
   
   SUBROUTINE set_gridgroup_attributes_id(gridgroup_id, name_, description_, domain_ref_, axis_ref_)
      IMPLICIT NONE
      TYPE(XGridGroupHandle)                        :: gridgroup_hdl
      CHARACTER(len = *)               , INTENT(IN) :: gridgroup_id
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref_
      
      CALL gridgroup_handle_create(gridgroup_hdl, gridgroup_id)
      CALL set_gridgroup_attributes_hdl(gridgroup_hdl, name_, description_, domain_ref_, axis_ref_)

   END SUBROUTINE set_gridgroup_attributes_id

   SUBROUTINE set_gridgroup_attributes_hdl(gridgroup_hdl, name_, description_, domain_ref_, axis_ref_)
      IMPLICIT NONE
      TYPE      (XGridGroupHandle)                  :: gridgroup_hdl
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: name_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: description_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: domain_ref_
      CHARACTER(len = *)     , OPTIONAL, INTENT(IN) :: axis_ref_
      
      IF (PRESENT(name_))        THEN
       CALL cxios_set_gridgroup_name(gridgroup_hdl%daddr, name_, len(name_))
      END IF
      IF (PRESENT(description_)) THEN
       CALL cxios_set_gridgroup_description(gridgroup_hdl%daddr, description_, len(description_))
      END IF
      IF (PRESENT(domain_ref_))  THEN
       CALL cxios_set_gridgroup_domain_ref(gridgroup_hdl%daddr, domain_ref_, len(domain_ref_))
      END IF
      IF (PRESENT(axis_ref_))    THEN
       CALL cxios_set_gridgroup_axis_ref(gridgroup_hdl%daddr, axis_ref_, len(axis_ref_))
      END IF
   END SUBROUTINE set_gridgroup_attributes_hdl

   SUBROUTINE grid_handle_create(ret, idt)
      IMPLICIT NONE
      TYPE(XGridHandle), INTENT(OUT):: ret
      CHARACTER(len = *), INTENT(IN) :: idt      
      CALL cxios_grid_handle_create(ret%daddr, idt, len(idt))            
   END SUBROUTINE grid_handle_create
   
   SUBROUTINE gridgroup_handle_create(ret, idt)
      IMPLICIT NONE
      TYPE(XGridGroupHandle), INTENT(OUT):: ret
      CHARACTER(len = *)     , INTENT(IN) :: idt      
      CALL cxios_gridgroup_handle_create(ret%daddr, idt, len(idt))            
   END SUBROUTINE gridgroup_handle_create

   LOGICAL FUNCTION grid_valid_id(idt)
      IMPLICIT NONE
      CHARACTER(len  = *)    , INTENT(IN) :: idt
      LOGICAL  (kind = 1)                 :: val
      CALL cxios_grid_valid_id(val, idt, len(idt));
      grid_valid_id = val
   END FUNCTION  grid_valid_id

   LOGICAL FUNCTION gridgroup_valid_id(idt)
      IMPLICIT NONE
      CHARACTER(len  = *)    , INTENT(IN) :: idt
      LOGICAL  (kind = 1)                 :: val
      CALL cxios_gridgroup_valid_id(val, idt, len(idt));
      gridgroup_valid_id = val
   END FUNCTION  gridgroup_valid_id
   
END MODULE IGRID
