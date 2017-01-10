

MODULE IEXTRACT_DOMAIN_TO_AXIS
   USE, INTRINSIC :: ISO_C_BINDING
   USE EXTRACT_DOMAIN_TO_AXIS_INTERFACE

   TYPE xios_extract_domain_to_axis
      INTEGER(kind = C_INTPTR_T) :: daddr
   END TYPE xios_extract_domain_to_axis

   CONTAINS ! Fonctions disponibles pour les utilisateurs.

   SUBROUTINE xios_get_extract_domain_to_axis_handle(idt,ret)
      IMPLICIT NONE
      CHARACTER(len = *), INTENT(IN) :: idt
      TYPE(xios_extract_domain_to_axis) , INTENT(OUT):: ret
      CALL cxios_extract_domain_to_axis_handle_create(ret%daddr, idt, len(idt))
   END SUBROUTINE xios_get_extract_domain_to_axis_handle

   LOGICAL FUNCTION xios_is_valid_extract_domain_to_axis(idt)
      IMPLICIT NONE
      CHARACTER(len = *) , INTENT(IN) :: idt
      LOGICAL (kind = 1) :: val

      CALL cxios_extract_domain_to_axis_valid_id(val, idt, len(idt))
      xios_is_valid_extract_domain_to_axis = val

   END FUNCTION xios_is_valid_extract_domain_to_axis

END MODULE IEXTRACT_DOMAIN_TO_AXIS
