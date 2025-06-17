cmake_minimum_required(VERSION 3.5)

project(rosidl_typesupport_fastrtps_c)

if(DEFINED ENV{FASTRTPS_STATIC_DISABLE})
  set(FASTRTPS_STATIC_DISABLE $ENV{FASTRTPS_STATIC_DISABLE}
    CACHE BOOL "If fastrtps Static should be disabled.")
else()
  set(FASTRTPS_STATIC_DISABLE FALSE
    CACHE BOOL "If fastrtps Static should be disabled.")
endif()

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_ros REQUIRED)

if(FASTRTPS_STATIC_DISABLE)
  ament_package()
  message(STATUS "fastrtps static rmw implementation explicitly disabled - skipping '${PROJECT_NAME}'")
  return()
endif()

find_package(ament_cmake_python REQUIRED)
find_package(rosidl_runtime_c REQUIRED)

ament_export_dependencies(rosidl_cmake)
ament_export_dependencies(rosidl_runtime_c)

ament_python_install_package(${PROJECT_NAME})

add_library(${PROJECT_NAME}
  src/identifier.cpp
  src/wstring_conversion.cpp)
set_property(TARGET ${PROJECT_NAME}
  PROPERTY DEFINE_SYMBOL "ROSIDL_TYPESUPPORT_FASTRTPS_C_BUILDING_DLL")
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
target_link_libraries(${PROJECT_NAME} PUBLIC
  rosidl_runtime_c::rosidl_runtime_c)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_index_register_resource("rosidl_typesupport_c")

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  find_package(performance_test_fixture REQUIRED)

  # Give cppcheck hints about macro definitions coming from outside this package
  get_target_property(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS
    performance_test_fixture::performance_test_fixture INTERFACE_INCLUDE_DIRECTORIES)

  ament_lint_auto_find_test_dependencies()

  get_target_property(memory_tools_ld_preload_env_var
    osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE)
  get_target_property(memory_tools_is_available
    osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_IS_AVAILABLE)

  set(SKIP_MEMORY_TOOLS_TEST "")
  if(NOT memory_tools_is_available)
    set(SKIP_MEMORY_TOOLS_TEST "SKIP_TEST")
  endif()

  ament_add_gtest(test_wstring_conversion test/test_wstring_conversion.cpp)
  if(TARGET test_wstring_conversion)
    target_include_directories(test_wstring_conversion PRIVATE
      ${osrf_testing_tools_cpp_INCLUDE_DIRS})
    target_link_libraries(test_wstring_conversion
      ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_wstring_conversion_mem test/test_wstring_conversion_mem.cpp
    ENV ${memory_tools_ld_preload_env_var}
    ${SKIP_MEMORY_TOOLS_TEST})
  if(TARGET test_wstring_conversion_mem)
    target_link_libraries(test_wstring_conversion_mem
      ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools)
  endif()

  add_performance_test(benchmark_string_conversions test/benchmark/benchmark_string_conversions.cpp)
  if(TARGET benchmark_string_conversions)
    target_link_libraries(benchmark_string_conversions ${PROJECT_NAME})
  endif()

  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(test_cli_extension test/test_cli_extension.py)
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_typesupport_fastrtps_c-extras.cmake.in"
)

install(
  PROGRAMS bin/rosidl_typesupport_fastrtps_c
  DESTINATION lib/rosidl_typesupport_fastrtps_c
)

install(
  DIRECTORY cmake resource
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

install(
  TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
