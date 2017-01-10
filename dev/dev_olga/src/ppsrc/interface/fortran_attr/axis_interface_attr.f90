! * ************************************************************************** *
! * Interface auto generated - do not modify *
! * ************************************************************************** *


MODULE axis_interface_attr
  USE, INTRINSIC :: ISO_C_BINDING

  INTERFACE
    ! Do not call directly / interface FORTRAN 2003 <-> C99

    SUBROUTINE cxios_set_axis_axis_ref(axis_hdl, axis_ref, axis_ref_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: axis_ref
      INTEGER (kind = C_INT) , VALUE :: axis_ref_size
    END SUBROUTINE cxios_set_axis_axis_ref

    SUBROUTINE cxios_get_axis_axis_ref(axis_hdl, axis_ref, axis_ref_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: axis_ref
      INTEGER (kind = C_INT) , VALUE :: axis_ref_size
    END SUBROUTINE cxios_get_axis_axis_ref

    FUNCTION cxios_is_defined_axis_axis_ref(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_axis_ref
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_axis_ref


    SUBROUTINE cxios_set_axis_begin(axis_hdl, begin) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , VALUE :: begin
    END SUBROUTINE cxios_set_axis_begin

    SUBROUTINE cxios_get_axis_begin(axis_hdl, begin) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) :: begin
    END SUBROUTINE cxios_get_axis_begin

    FUNCTION cxios_is_defined_axis_begin(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_begin
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_begin


    SUBROUTINE cxios_set_axis_bounds(axis_hdl, bounds, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      REAL (KIND=C_DOUBLE) , DIMENSION(*) :: bounds
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_set_axis_bounds

    SUBROUTINE cxios_get_axis_bounds(axis_hdl, bounds, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      REAL (KIND=C_DOUBLE) , DIMENSION(*) :: bounds
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_get_axis_bounds

    FUNCTION cxios_is_defined_axis_bounds(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_bounds
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_bounds


    SUBROUTINE cxios_set_axis_data_begin(axis_hdl, data_begin) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , VALUE :: data_begin
    END SUBROUTINE cxios_set_axis_data_begin

    SUBROUTINE cxios_get_axis_data_begin(axis_hdl, data_begin) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) :: data_begin
    END SUBROUTINE cxios_get_axis_data_begin

    FUNCTION cxios_is_defined_axis_data_begin(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_data_begin
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_data_begin


    SUBROUTINE cxios_set_axis_data_index(axis_hdl, data_index, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , DIMENSION(*) :: data_index
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_set_axis_data_index

    SUBROUTINE cxios_get_axis_data_index(axis_hdl, data_index, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , DIMENSION(*) :: data_index
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_get_axis_data_index

    FUNCTION cxios_is_defined_axis_data_index(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_data_index
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_data_index


    SUBROUTINE cxios_set_axis_data_n(axis_hdl, data_n) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , VALUE :: data_n
    END SUBROUTINE cxios_set_axis_data_n

    SUBROUTINE cxios_get_axis_data_n(axis_hdl, data_n) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) :: data_n
    END SUBROUTINE cxios_get_axis_data_n

    FUNCTION cxios_is_defined_axis_data_n(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_data_n
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_data_n


    SUBROUTINE cxios_set_axis_index(axis_hdl, index, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , DIMENSION(*) :: index
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_set_axis_index

    SUBROUTINE cxios_get_axis_index(axis_hdl, index, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , DIMENSION(*) :: index
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_get_axis_index

    FUNCTION cxios_is_defined_axis_index(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_index
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_index


    SUBROUTINE cxios_set_axis_long_name(axis_hdl, long_name, long_name_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: long_name
      INTEGER (kind = C_INT) , VALUE :: long_name_size
    END SUBROUTINE cxios_set_axis_long_name

    SUBROUTINE cxios_get_axis_long_name(axis_hdl, long_name, long_name_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: long_name
      INTEGER (kind = C_INT) , VALUE :: long_name_size
    END SUBROUTINE cxios_get_axis_long_name

    FUNCTION cxios_is_defined_axis_long_name(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_long_name
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_long_name


    SUBROUTINE cxios_set_axis_mask(axis_hdl, mask, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      LOGICAL (KIND=C_BOOL) , DIMENSION(*) :: mask
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_set_axis_mask

    SUBROUTINE cxios_get_axis_mask(axis_hdl, mask, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      LOGICAL (KIND=C_BOOL) , DIMENSION(*) :: mask
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_get_axis_mask

    FUNCTION cxios_is_defined_axis_mask(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_mask
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_mask


    SUBROUTINE cxios_set_axis_n(axis_hdl, n) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , VALUE :: n
    END SUBROUTINE cxios_set_axis_n

    SUBROUTINE cxios_get_axis_n(axis_hdl, n) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) :: n
    END SUBROUTINE cxios_get_axis_n

    FUNCTION cxios_is_defined_axis_n(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_n
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_n


    SUBROUTINE cxios_set_axis_n_distributed_partition(axis_hdl, n_distributed_partition) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , VALUE :: n_distributed_partition
    END SUBROUTINE cxios_set_axis_n_distributed_partition

    SUBROUTINE cxios_get_axis_n_distributed_partition(axis_hdl, n_distributed_partition) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) :: n_distributed_partition
    END SUBROUTINE cxios_get_axis_n_distributed_partition

    FUNCTION cxios_is_defined_axis_n_distributed_partition(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_n_distributed_partition
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_n_distributed_partition


    SUBROUTINE cxios_set_axis_n_glo(axis_hdl, n_glo) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) , VALUE :: n_glo
    END SUBROUTINE cxios_set_axis_n_glo

    SUBROUTINE cxios_get_axis_n_glo(axis_hdl, n_glo) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      INTEGER (KIND=C_INT) :: n_glo
    END SUBROUTINE cxios_get_axis_n_glo

    FUNCTION cxios_is_defined_axis_n_glo(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_n_glo
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_n_glo


    SUBROUTINE cxios_set_axis_name(axis_hdl, name, name_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: name
      INTEGER (kind = C_INT) , VALUE :: name_size
    END SUBROUTINE cxios_set_axis_name

    SUBROUTINE cxios_get_axis_name(axis_hdl, name, name_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: name
      INTEGER (kind = C_INT) , VALUE :: name_size
    END SUBROUTINE cxios_get_axis_name

    FUNCTION cxios_is_defined_axis_name(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_name
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_name


    SUBROUTINE cxios_set_axis_positive(axis_hdl, positive, positive_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: positive
      INTEGER (kind = C_INT) , VALUE :: positive_size
    END SUBROUTINE cxios_set_axis_positive

    SUBROUTINE cxios_get_axis_positive(axis_hdl, positive, positive_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: positive
      INTEGER (kind = C_INT) , VALUE :: positive_size
    END SUBROUTINE cxios_get_axis_positive

    FUNCTION cxios_is_defined_axis_positive(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_positive
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_positive


    SUBROUTINE cxios_set_axis_standard_name(axis_hdl, standard_name, standard_name_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: standard_name
      INTEGER (kind = C_INT) , VALUE :: standard_name_size
    END SUBROUTINE cxios_set_axis_standard_name

    SUBROUTINE cxios_get_axis_standard_name(axis_hdl, standard_name, standard_name_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: standard_name
      INTEGER (kind = C_INT) , VALUE :: standard_name_size
    END SUBROUTINE cxios_get_axis_standard_name

    FUNCTION cxios_is_defined_axis_standard_name(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_standard_name
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_standard_name


    SUBROUTINE cxios_set_axis_unit(axis_hdl, unit, unit_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: unit
      INTEGER (kind = C_INT) , VALUE :: unit_size
    END SUBROUTINE cxios_set_axis_unit

    SUBROUTINE cxios_get_axis_unit(axis_hdl, unit, unit_size) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      CHARACTER(kind = C_CHAR) , DIMENSION(*) :: unit
      INTEGER (kind = C_INT) , VALUE :: unit_size
    END SUBROUTINE cxios_get_axis_unit

    FUNCTION cxios_is_defined_axis_unit(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_unit
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_unit


    SUBROUTINE cxios_set_axis_value(axis_hdl, value, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      REAL (KIND=C_DOUBLE) , DIMENSION(*) :: value
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_set_axis_value

    SUBROUTINE cxios_get_axis_value(axis_hdl, value, extent) BIND(C)
      USE ISO_C_BINDING
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
      REAL (KIND=C_DOUBLE) , DIMENSION(*) :: value
      INTEGER (kind = C_INT), DIMENSION(*) :: extent
    END SUBROUTINE cxios_get_axis_value

    FUNCTION cxios_is_defined_axis_value(axis_hdl) BIND(C)
      USE ISO_C_BINDING
      LOGICAL(kind=C_BOOL) :: cxios_is_defined_axis_value
      INTEGER (kind = C_INTPTR_T), VALUE :: axis_hdl
    END FUNCTION cxios_is_defined_axis_value

  END INTERFACE

END MODULE axis_interface_attr