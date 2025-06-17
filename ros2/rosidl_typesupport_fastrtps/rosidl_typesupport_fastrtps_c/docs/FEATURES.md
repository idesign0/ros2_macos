# rosidl_typesupport_fastrtps_c features

`rosidl_typesupport_fastrtps_c` provides a Python generator executable based on Empy to create rosidl C source files for use with eProsima's FastRTPS.

The templates utilized by this generator executable are located in resource directory and generate both headers and sources.
They are generated for both messages and services.
It does not generate separate files for actions.

The generator also generates a visibility_control header based on https://gcc.gnu.org/wiki/Visibility.

## Non-Generated helper files

`rosidl_typesupport_fastrtps_c` defines a typesupport identifier, which is declared in `identifier.h`.

`rosidl_typesupport_fastrtps_c` provides the following functionality for incorporation into generated typesupport source files.

* `wstring_conversion.hpp`: Simple conversion functions from u16string types to wstring and vice versa.
