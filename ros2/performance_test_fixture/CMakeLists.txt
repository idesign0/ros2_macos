cmake_minimum_required(VERSION 3.5)
project(performance_test_fixture CXX)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libraries")

# find dependencies
find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_export_dependencies REQUIRED)
find_package(ament_cmake_export_targets REQUIRED)
find_package(ament_cmake_test REQUIRED)
find_package(benchmark REQUIRED PATHS /usr/local NO_DEFAULT_PATH)
find_package(osrf_testing_tools_cpp REQUIRED)

add_library(${PROJECT_NAME}
  src/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PUBLIC
  benchmark::benchmark
  osrf_testing_tools_cpp::memory_tools)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  "PERFORMANCE_TEST_FIXTURE_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(
  DIRECTORY include/
  DESTINATION include)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME})

ament_export_targets(${PROJECT_NAME})
ament_export_dependencies(
  ament_cmake_google_benchmark benchmark osrf_testing_tools_cpp)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  set(performance_test_fixture_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
  include(performance_test_fixture-extras.cmake)

  add_performance_test(
    benchmark_malloc_realloc
    test/benchmark/benchmark_malloc_realloc.cpp
    TIMEOUT 90)

  # Bypassing performance_test_fixture to avoid LD_PRELOAD of
  # osrf_testing_tools_cpp
  set(_no_mem_tools_skip_arg)
  if(TARGET benchmark_malloc_realloc)
    get_target_property(ARG_SKIP_RETURN_CODE benchmark_malloc_realloc SKIP_RETURN_CODE)
    if(ARG_SKIP_RETURN_CODE)
      set(_no_mem_tools_skip_arg SKIP_TEST)
    endif()
  else()
    set(_no_mem_tools_skip_arg SKIP_TEST)
  endif()
  ament_add_google_benchmark(
    benchmark_malloc_realloc_no_memory_tools
    test/benchmark/benchmark_malloc_realloc_no_memory_tools.cpp
    ${_no_mem_tools_skip_arg}
    TIMEOUT 60)

  add_performance_test(
    benchmark_pause_resume
    test/benchmark/benchmark_pause_resume.cpp)
endif()

ament_package(
  CONFIG_EXTRAS_POST "${PROJECT_NAME}-extras.cmake"
)
