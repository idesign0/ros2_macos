cmake_minimum_required(VERSION 3.5)
project(tf2)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Woverloaded-virtual)
endif()

find_package(ament_cmake)
find_package(console_bridge_vendor REQUIRED) # Provides console_bridge 0.4.0 on platforms without it.
find_package(console_bridge REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rcutils REQUIRED)
find_package(rosidl_runtime_cpp REQUIRED)

# export user definitions

#CPP Libraries
add_library(tf2 SHARED src/cache.cpp src/buffer_core.cpp src/static_cache.cpp src/time.cpp)
target_include_directories(tf2 PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
target_link_libraries(tf2 PUBLIC
  ${geometry_msgs_TARGETS})
target_link_libraries(tf2 PRIVATE
  ${builtin_interfaces_TARGETS}
  console_bridge::console_bridge
  rcutils::rcutils)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(tf2 PRIVATE "TF2_BUILDING_DLL")

install(TARGETS tf2 EXPORT export_tf2
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

# Tests
if(BUILD_TESTING)
  find_package(ament_cmake_google_benchmark REQUIRED)
  find_package(ament_cmake_copyright REQUIRED)
  find_package(ament_cmake_cppcheck REQUIRED)
  find_package(ament_cmake_cpplint REQUIRED)
  find_package(ament_cmake_lint_cmake REQUIRED)
  find_package(ament_cmake_uncrustify REQUIRED)
  find_package(ament_cmake_xmllint REQUIRED)

  # Should not lint external code
  set(
    _linter_excludes
    include/tf2/LinearMath/Matrix3x3.h
    include/tf2/LinearMath/MinMax.h
    include/tf2/LinearMath/QuadWord.h
    include/tf2/LinearMath/Quaternion.h
    include/tf2/LinearMath/Scalar.h
    include/tf2/LinearMath/Transform.h
    include/tf2/LinearMath/Vector3.h
    include/tf2/LinearMath/Matrix3x3.hpp
    include/tf2/LinearMath/MinMax.hpp
    include/tf2/LinearMath/QuadWord.hpp
    include/tf2/LinearMath/Quaternion.hpp
    include/tf2/LinearMath/Scalar.hpp
    include/tf2/LinearMath/Transform.hpp
    include/tf2/LinearMath/Vector3.hpp
  )

  ament_copyright(EXCLUDE ${_linter_excludes})
  ament_cppcheck(
    EXCLUDE ${_linter_excludes}
    LANGUAGE c++
  )
  ament_cpplint(EXCLUDE ${_linter_excludes})
  ament_lint_cmake()
  ament_uncrustify(
    EXCLUDE ${_linter_excludes}
    LANGUAGE c++
  )
  ament_xmllint()

  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_cache_unittest test/cache_unittest.cpp)
  if(TARGET test_cache_unittest)
    target_link_libraries(test_cache_unittest tf2)
  endif()

  ament_add_google_benchmark(cache_benchmark test/cache_benchmark.cpp)
  if(TARGET cache_benchmark)
    target_link_libraries(cache_benchmark tf2)
  endif()

  ament_add_gtest(test_static_cache_unittest test/static_cache_test.cpp)
  if(TARGET test_static_cache_unittest)
    target_link_libraries(test_static_cache_unittest tf2)
  endif()

  ament_add_gtest(test_simple_tf2_core test/simple_tf2_core.cpp)
  if(TARGET test_simple_tf2_core)
    target_link_libraries(test_simple_tf2_core
      ${builtin_interfaces_TARGETS}
      tf2
      # Used, but not linked to test tf2's exports:
      #   ${geometry_msgs_TARGETS}
    )
  endif()

  ament_add_gtest(test_time test/test_time.cpp)
  if(TARGET test_time)
    target_link_libraries(test_time tf2)
  endif()

  ament_add_gtest(test_storage test/test_storage.cpp)
  if(TARGET test_storage)
    target_link_libraries(test_storage tf2)
  endif()
endif()

ament_export_dependencies(console_bridge geometry_msgs rcutils rosidl_runtime_cpp)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(tf2)

# Export modern CMake targets
ament_export_targets(export_tf2)

ament_package()
