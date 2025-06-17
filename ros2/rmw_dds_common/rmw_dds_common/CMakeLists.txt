cmake_minimum_required(VERSION 3.5)

project(rmw_dds_common)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rcpputils REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(rosidl_default_generators REQUIRED)

ament_export_dependencies(ament_cmake_core)
ament_export_dependencies(rcpputils)
ament_export_dependencies(rcutils)
ament_export_dependencies(rmw)

rosidl_generate_interfaces(
  ${PROJECT_NAME}
  "msg/Gid.msg"
  "msg/NodeEntitiesInfo.msg"
  "msg/ParticipantEntitiesInfo.msg"
)

add_library(${PROJECT_NAME}_library SHARED
  src/gid_utils.cpp
  src/graph_cache.cpp
  src/qos.cpp
  src/security.cpp
  src/time_utils.cpp)

set_target_properties(${PROJECT_NAME}_library
  PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_library PUBLIC
  rcutils::rcutils
  rmw::rmw)
target_link_libraries(${PROJECT_NAME}_library PRIVATE
  rcpputils::rcpputils)
target_include_directories(${PROJECT_NAME}_library
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

# Wait for all rosidl generators to finish before building this library
add_dependencies(${PROJECT_NAME}_library
  ${PROJECT_NAME})

# Depend on the generated C++ messages
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
target_link_libraries(${PROJECT_NAME}_library PUBLIC
  ${cpp_typesupport_target})

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}_library
  PRIVATE "RMW_DDS_COMMON_BUILDING_LIBRARY")

install(
  TARGETS ${PROJECT_NAME}_library EXPORT ${PROJECT_NAME}_library
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME}_library)

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME}_library)

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gmock REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(performance_test_fixture REQUIRED)
  # Give cppcheck hints about macro definitions coming from outside this package
  get_target_property(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS performance_test_fixture::performance_test_fixture
    INTERFACE_INCLUDE_DIRECTORIES)

  ament_add_gmock(test_graph_cache test/test_graph_cache.cpp)
  if(TARGET test_graph_cache)
    target_link_libraries(test_graph_cache ${PROJECT_NAME}_library osrf_testing_tools_cpp::memory_tools)
    target_compile_definitions(test_graph_cache PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  ament_add_gmock(test_gid_utils test/test_gid_utils.cpp)
  if(TARGET test_gid_utils)
    target_link_libraries(test_gid_utils ${PROJECT_NAME}_library)
  endif()

  ament_add_gmock(test_qos test/test_qos.cpp)
  if(TARGET test_qos)
    target_link_libraries(test_qos ${PROJECT_NAME}_library)
  endif()

  ament_add_gmock(test_time_utils test/test_time_utils.cpp)
  if(TARGET test_time_utils)
    target_link_libraries(test_time_utils ${PROJECT_NAME}_library)
  endif()

  ament_add_gmock(test_security test/test_security.cpp)
  if(TARGET test_security)
    target_link_libraries(test_security
      ${PROJECT_NAME}_library
      rcpputils::rcpputils)
  endif()

  add_performance_test(benchmark_graph_cache test/benchmark/benchmark_graph_cache.cpp)
  if(TARGET benchmark_graph_cache)
    target_link_libraries(benchmark_graph_cache ${PROJECT_NAME}_library)
  endif()
endif()

ament_package()
