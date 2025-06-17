# Copyright 2020 Real-Time Innovations, Inc.
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

cmake_minimum_required(VERSION 3.5)

project(rmw_connextdds)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake REQUIRED)
find_package(rmw_connextdds_common REQUIRED)

if(NOT TARGET rmw_connextdds_common::rmw_connextdds_common_pro)
    message(STATUS "Common RMW implementation not found. Skipping ${PROJECT_NAME}.")
    ament_package()
    return()
endif()

rti_find_connextpro()

if(NOT RTIConnextDDS_FOUND)
    message(STATUS "RTI Connext DDS Professional not found. Skipping ${PROJECT_NAME}.")
    ament_package()
    return()
endif()

add_library(${PROJECT_NAME} SHARED
    src/rmw_api_impl_ndds.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(${PROJECT_NAME}
    rmw_connextdds_common::rmw_connextdds_common_pro)

configure_rmw_library(${PROJECT_NAME})

register_rmw_implementation(
    "c:rosidl_typesupport_fastrtps_c:rosidl_typesupport_introspection_c"
    "cpp:rosidl_typesupport_fastrtps_cpp:rosidl_typesupport_introspection_cpp")

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
endif()

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(rmw_connextdds_common)

ament_package(
  CONFIG_EXTRAS_POST "${PROJECT_NAME}-extras.cmake.in"
)

ament_index_register_resource("rmw_output_prefixes"
  CONTENT "RTI Data Distribution Service\nRTI Connext DDS\nExpires on")