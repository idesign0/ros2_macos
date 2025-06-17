# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(NOT TARGET ${rosidl_generate_interfaces_TARGET}__rosidl_generator_cpp)
  message(FATAL_ERROR
    "The 'rosidl_generator_cpp' extension must be executed before the "
    "'rosidl_typesupport_fastrtps_cpp' extension.")
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(fastrtps_cmake_module QUIET)
find_package(fastcdr REQUIRED CONFIG)
find_package(rmw REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(rosidl_runtime_cpp REQUIRED)
find_package(rosidl_typesupport_interface REQUIRED)


set(_output_path "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_fastrtps_cpp/${PROJECT_NAME}")

# Create a list of files that will be generated from each IDL file
set(_generated_files "")
foreach(_abs_idl_file ${rosidl_generate_interfaces_ABS_IDL_FILES})
  get_filename_component(_parent_folder "${_abs_idl_file}" DIRECTORY)
  get_filename_component(_parent_folder "${_parent_folder}" NAME)
  get_filename_component(_idl_name "${_abs_idl_file}" NAME_WE)
  # Turn idl name into file names
  string_camel_case_to_lower_case_underscore("${_idl_name}" _header_name)
  list(APPEND _generated_files
    "${_output_path}/${_parent_folder}/detail/dds_fastrtps/${_header_name}__type_support.cpp"
    "${_output_path}/${_parent_folder}/detail/${_header_name}__rosidl_typesupport_fastrtps_cpp.hpp"
  )
endforeach()

# Create a list of IDL files from other packages that this generator should depend on
set(_dependency_files "")
set(_dependencies "")
foreach(_pkg_name ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES})
  foreach(_idl_file ${${_pkg_name}_IDL_FILES})
    # ${{_pkg_name}_DIR} is absolute path ending in 'share/<pkg_name>/cmake', so go back one
    # directory for IDL files
    set(_abs_idl_file "${${_pkg_name}_DIR}/../${_idl_file}")
    normalize_path(_abs_idl_file "${_abs_idl_file}")
    list(APPEND _dependency_files "${_abs_idl_file}")
    list(APPEND _dependencies "${_pkg_name}:${_abs_idl_file}")
  endforeach()
endforeach()

# Create a list of templates and source files this generator uses, and check that they exist
set(target_dependencies
  "${rosidl_typesupport_fastrtps_cpp_BIN}"
  ${rosidl_typesupport_fastrtps_cpp_GENERATOR_FILES}
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/idl__rosidl_typesupport_fastrtps_cpp.hpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/idl__type_support.cpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/msg__rosidl_typesupport_fastrtps_cpp.hpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/msg__type_support.cpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/srv__rosidl_typesupport_fastrtps_cpp.hpp.em"
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/srv__type_support.cpp.em"
  ${rosidl_generate_interfaces_ABS_IDL_FILES}
  ${_dependency_files})
foreach(dep ${target_dependencies})
  if(NOT EXISTS "${dep}")
    message(FATAL_ERROR "Target dependency '${dep}' does not exist")
  endif()
endforeach()

# Write all this to a file to work around command line length limitations on some platforms
set(generator_arguments_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_fastrtps_cpp__arguments.json")
rosidl_write_generator_arguments(
  "${generator_arguments_file}"
  PACKAGE_NAME "${PROJECT_NAME}"
  IDL_TUPLES "${rosidl_generate_interfaces_IDL_TUPLES}"
  ROS_INTERFACE_DEPENDENCIES "${_dependencies}"
  OUTPUT_DIR "${_output_path}"
  TEMPLATE_DIR "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}"
  TARGET_DEPENDENCIES ${target_dependencies}
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Add a command that invokes generator at build time
add_custom_command(
  OUTPUT ${_generated_files}
  COMMAND Python3::Interpreter
  ARGS ${rosidl_typesupport_fastrtps_cpp_BIN}
  --generator-arguments-file "${generator_arguments_file}"
  DEPENDS ${target_dependencies}
  COMMENT "Generating C++ type support for eProsima Fast-RTPS"
  VERBATIM
)

# generate header to switch between export and import for a specific package
set(_visibility_control_file
"${_output_path}/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h")
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
configure_file(
  "${rosidl_typesupport_fastrtps_cpp_TEMPLATE_DIR}/rosidl_typesupport_fastrtps_cpp__visibility_control.h.in"
  "${_visibility_control_file}"
  @ONLY
)

set(_target_suffix "__rosidl_typesupport_fastrtps_cpp")

# Create a library that builds the generated files
add_library(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  ${_generated_files})

# Change output library name if asked to
if(rosidl_generate_interfaces_LIBRARY_NAME)
  set_target_properties(${rosidl_generate_interfaces_TARGET}${_target_suffix}
    PROPERTIES OUTPUT_NAME "${rosidl_generate_interfaces_LIBRARY_NAME}${_target_suffix}")
endif()

set_target_properties(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PROPERTIES
    DEFINE_SYMBOL "ROSIDL_TYPESUPPORT_FASTRTPS_CPP_BUILDING_DLL_${PROJECT_NAME}"
    CXX_STANDARD 14)

# Set compiler flags
if(NOT WIN32)
  set(_target_compile_flags -Wall -Wextra -Wpedantic -Wredundant-decls)
else()
  set(_target_compile_flags /W4)
endif()
target_compile_options(${rosidl_generate_interfaces_TARGET}${_target_suffix} PRIVATE ${_target_compile_flags})

target_include_directories(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_fastrtps_cpp>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)

target_link_libraries(${rosidl_generate_interfaces_TARGET}${_target_suffix} PUBLIC
  fastcdr
  rmw::rmw
  rosidl_runtime_c::rosidl_runtime_c
  rosidl_runtime_cpp::rosidl_runtime_cpp
  rosidl_typesupport_interface::rosidl_typesupport_interface
  rosidl_typesupport_fastrtps_cpp::rosidl_typesupport_fastrtps_cpp)

# Depend on dependencies
foreach(_pkg_name ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES})
  target_link_libraries(${rosidl_generate_interfaces_TARGET}${_target_suffix} PUBLIC
    ${${_pkg_name}_TARGETS${_target_suffix}})
endforeach()

# Depend on the target created by rosidl_generator_cpp
target_link_libraries(${rosidl_generate_interfaces_TARGET}${_target_suffix} PUBLIC
  ${rosidl_generate_interfaces_TARGET}__rosidl_generator_cpp)

# Make top level generation target depend on this library
add_dependencies(
  ${rosidl_generate_interfaces_TARGET}
  ${rosidl_generate_interfaces_TARGET}${_target_suffix}
)

if(NOT rosidl_generate_interfaces_SKIP_INSTALL)
  install(
    DIRECTORY "${_output_path}/"
    DESTINATION "include/${PROJECT_NAME}/${PROJECT_NAME}"
    PATTERN "*.cpp" EXCLUDE
  )

  # Export old-style CMake variables
  ament_export_include_directories("include/${PROJECT_NAME}")
  rosidl_export_typesupport_libraries(${_target_suffix}
    ${rosidl_generate_interfaces_TARGET}${_target_suffix})

  # Export modern CMake targets
  ament_export_targets(export_${rosidl_generate_interfaces_TARGET}${_target_suffix})
  rosidl_export_typesupport_targets(${_target_suffix}
    ${rosidl_generate_interfaces_TARGET}${_target_suffix})

  install(
    TARGETS ${rosidl_generate_interfaces_TARGET}${_target_suffix}
    EXPORT export_${rosidl_generate_interfaces_TARGET}${_target_suffix}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
  )

  ament_export_dependencies(fastrtps_cmake_module)
  ament_export_dependencies(fastcdr)
  ament_export_dependencies(rmw)
  ament_export_dependencies(rosidl_runtime_c)
  ament_export_dependencies(rosidl_runtime_cpp)
  ament_export_dependencies(rosidl_typesupport_fastrtps_cpp)
  ament_export_dependencies(rosidl_typesupport_interface)
endif()

if(BUILD_TESTING AND rosidl_generate_interfaces_ADD_LINTER_TESTS)
  find_package(ament_cmake_cppcheck REQUIRED)
  ament_cppcheck(
    TESTNAME "cppcheck_rosidl_typesupport_fastrtps_cpp"
    ${_generated_files})

  find_package(ament_cmake_cpplint REQUIRED)
  get_filename_component(_cpplint_root "${_output_path}" DIRECTORY)
  ament_cpplint(
    TESTNAME "cpplint_rosidl_typesupport_fastrtps_cpp"
    # the generated code might contain longer lines for templated types
    MAX_LINE_LENGTH 999
    ROOT "${_cpplint_root}"
    ${_generated_files})

  find_package(ament_cmake_uncrustify REQUIRED)
  ament_uncrustify(
    TESTNAME "uncrustify_rosidl_typesupport_fastrtps_cpp"
    # the generated code might contain longer lines for templated types
    # set the value to zero to tell uncrustify to ignore line lengths
    MAX_LINE_LENGTH 0
    ${_generated_files})
endif()
