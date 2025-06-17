cmake_minimum_required(VERSION 3.14.4)
project(filters)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

##############################################################################
# Find dependencies
##############################################################################

find_package(ament_cmake REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Boost QUIET COMPONENTS headers)
if(NOT Boost_headers_FOUND)
  find_package(Boost REQUIRED)
endif()

##############################################################################
# Build
##############################################################################


if("${CMAKE_VERSION}" VERSION_GREATER_EQUAL 3.19)
  add_library(realtime_circular_buffer
    INTERFACE
      include/filters/realtime_circular_buffer.h
      include/filters/realtime_circular_buffer.hpp
  )
else()
  add_library(realtime_circular_buffer INTERFACE)
endif()

target_link_libraries(realtime_circular_buffer
  INTERFACE
    Boost::boost
)

# filter_base is an INTERFACE library because it only has header files.
# It is not compiled, but sets up the following properties for consumers
# to link to a target.
# For more info, see the CMake Wiki on INTERFACE targets.
# https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries


if("${CMAKE_VERSION}" VERSION_GREATER_EQUAL 3.19)
  add_library(filter_base
    INTERFACE
      include/filters/filter_base.h
      include/filters/filter_base.hpp
  )
else()
  add_library(filter_base INTERFACE)
endif()

target_link_libraries(filter_base
  INTERFACE
    rclcpp::rclcpp
)

if("${CMAKE_VERSION}" VERSION_GREATER_EQUAL 3.19)
  add_library(filter_chain
    INTERFACE
      include/filters/filter_chain.h
      include/filters/filter_chain.hpp
  )
else()
  add_library(filter_chain INTERFACE)
endif()


target_link_libraries(filter_chain
  INTERFACE
    filter_base
    pluginlib::pluginlib
)

set(interface_targets realtime_circular_buffer filter_base filter_chain)
foreach(target ${interface_targets})
  target_include_directories(${target}
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  )
endforeach()



# Plugins
add_library(mean SHARED src/mean.cpp)
target_link_libraries(mean
  PUBLIC
    realtime_circular_buffer
    filter_base
    pluginlib::pluginlib
)

add_library(params SHARED src/test_params.cpp)
target_link_libraries(params
  PUBLIC
    filter_base
    pluginlib::pluginlib
)

add_library(increment SHARED src/increment.cpp)
target_link_libraries(increment
  PUBLIC
    realtime_circular_buffer
    filter_base
    pluginlib::pluginlib
)

add_library(median SHARED src/median.cpp)
target_link_libraries(median
  PUBLIC
    realtime_circular_buffer
    filter_base
    pluginlib::pluginlib
)

add_library(transfer_function SHARED src/transfer_function.cpp)
target_link_libraries(transfer_function
  PUBLIC
    realtime_circular_buffer
    filter_base
    pluginlib::pluginlib
)

set(plugin_targets mean params increment median transfer_function)
foreach(target ${plugin_targets})
  target_include_directories(${target}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  )
endforeach()

install(TARGETS ${interface_targets} ${plugin_targets}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

if(BUILD_TESTING)
  # TODO(hidmic): use ament_lint_common once ament_copyright complains are resolved
  find_package(ament_cmake_cppcheck REQUIRED)
  find_package(ament_cmake_cpplint REQUIRED)
  find_package(ament_cmake_uncrustify REQUIRED)
  find_package(ament_cmake_xmllint REQUIRED)

  # Enforce ROS2 style
  ament_cpplint()
  ament_cppcheck(
    LANGUAGE c++  # Force C++ for .h headers
    INCLUDE_DIRS include
  )
  ament_uncrustify()
  ament_xmllint()

  find_package(ament_cmake_gtest REQUIRED)
  # Test median filter
  ament_add_gtest(median_test test/test_median.cpp)
  target_link_libraries(median_test median)

  # Test transfer function filter
  ament_add_gtest(transfer_function_test test/test_transfer_function.cpp)
  target_link_libraries(transfer_function_test transfer_function)

  # Test mean filter
  ament_add_gtest(mean_test test/test_mean.cpp)
  target_link_libraries(mean_test mean)

  # Test params filter
  ament_add_gtest(params_test test/test_params.cpp)
  target_link_libraries(params_test params)

  # Test realtime safe buffer class
  ament_add_gtest(realtime_buffer_test test/test_realtime_circular_buffer.cpp)
  target_link_libraries(realtime_buffer_test realtime_circular_buffer)

  # Pluginlib specific testing for filter chain
  set(test_prefix "${CMAKE_CURRENT_BINARY_DIR}/pluginlib_test_prefix")
  set(test_package_name "${PROJECT_NAME}")
  set(test_plugins_xml "default_plugins.xml")
  set(test_package_xml "package.xml")
  set(test_plugin_targets mean increment median transfer_function)

  # Write the necessary files to detect a package with pluginlib plugins
  file(COPY "${test_plugins_xml}" DESTINATION "${test_prefix}/share/${test_package_name}/")
  configure_file("${test_package_xml}" "${test_prefix}/share/${test_package_name}/package.xml" COPYONLY)
  file(WRITE "${test_prefix}/share/ament_index/resource_index/${test_package_name}__pluginlib__plugin/pluginlib"
    "share/${test_package_name}/${test_plugins_xml}\n")
  file(WRITE "${test_prefix}/share/ament_index/resource_index/packages/${test_package_name}" "")

  # Copy library files to test folder so they can be loaded
  foreach(test_plugin_target IN LISTS test_plugin_targets)
    set(_resource "$<TARGET_FILE:${test_plugin_target}>")
    set(_output "${test_prefix}/lib/$<TARGET_FILE_NAME:${test_plugin_target}>")
    add_custom_command(TARGET ${test_plugin_target}
      COMMENT "Copying target ${test_plugin_target} for testing"
      DEPENDS "${_resource}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${_resource}"
        "${_output}"
    )
    add_custom_target("copy_${test_plugin_target}_for_testing" DEPENDS "${test_plugin_target}")
  endforeach()

  # Test plugin loading into filter chain
  ament_add_gtest(chain_test test/test_chain.cpp
    ENV "AMENT_PREFIX_PATH=${test_prefix}")
  target_include_directories(chain_test PRIVATE include)
  foreach(test_plugin_target IN LISTS test_plugin_targets)
    add_dependencies(chain_test "copy_${test_plugin_target}_for_testing")
  endforeach()
  target_link_libraries(chain_test
    filter_chain
  )
endif()

##############################################################################
# Install
##############################################################################

# Export old-style CMake variables for includes
ament_export_include_directories("include/${PROJECT_NAME}")

# Export modern CMake targets
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_export_dependencies(
  Boost
  pluginlib
  rclcpp
)

# Install pluginlib xml
pluginlib_export_plugin_description_file(filters "default_plugins.xml")
ament_package()
