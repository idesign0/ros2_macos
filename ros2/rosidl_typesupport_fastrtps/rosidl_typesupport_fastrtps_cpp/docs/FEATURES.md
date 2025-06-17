# rosidl_typesupport_fastrtps_cpp features

## Empy Generated Files

`rosidl_typesupport_fastrtps_cpp` provides a Python generator executable `rosidl_typesupport_fastrtps_cpp`, based on Empy to create rosidl C source files for use with eProsima's FastRTPS.

The templates utilized by this generator executable are located in `resource` directory and generate both headers and sources for messages and services.
It does not generate separate files for actions.

The generator also generates a visibility_control header based on https://gcc.gnu.org/wiki/Visibility.

## Non-Generated helper files

`rosidl_typesupport_fastrtps_cpp` defines a typesupport identifier, which is declared in `identifier.hpp`.

`rosidl_typesupport_fastrtps_cpp` provides the following functionality for incorporation into generated typesupport source files.

* `wstring_conversion.hpp`: Simple conversion functions from u16string types to wstring and vice versa.

`rosidl_typesupport_fastrtps_cpp` includes definitions for type support callback structs.
They are defined for both messages and services in `message_type_support.h` and `service_type_support.h` respectively.

`rosidl_typesupport_fastrtps_cpp` also defines several headers that declare simple handle getter functions for rosidl types that are implemented in the generated source files.
They are defined for messages and services in `message_type_support_decl.hpp` and `service_type_support_decl.hpp` respectively.
