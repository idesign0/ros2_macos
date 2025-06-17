# Copyright 2018 Open Source Robotics Foundation, Inc.
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

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/osrf_testing_tools_cpp_require_googletest.cmake)

set(versions)

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_EQ 1.8)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()

set(versions 1.7.0;1.8.0)

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_EQ 1.9)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_EQ 1.8.1)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_EQ 1.8)
if(NOT valid_versions EQUAL 1)
  message(FATAL_ERROR "expected valid version index of 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_EQ 1.8.0)
if(NOT valid_versions EQUAL 1)
  message(FATAL_ERROR "expected valid version index of 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_LT 1.7)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_LT 1.8)
if(NOT valid_versions EQUAL 0)
  message(FATAL_ERROR "expected valid version index of 0, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_LT 1.9)
if(NOT valid_versions EQUAL "0;1")
  message(FATAL_ERROR "expected valid version indexes of 0 and 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_LTE 1.7)
if(NOT valid_versions EQUAL 0)
  message(FATAL_ERROR "expected valid versions to be 0, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_LTE 1.8)
if(NOT valid_versions EQUAL "0;1")
  message(FATAL_ERROR "expected valid version indexes of 0 and 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_LTE 1.6)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_GT 1.8)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_GT 1.7)
if(NOT valid_versions EQUAL 1)
  message(FATAL_ERROR "expected valid version index of 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_GT 1.6)
if(NOT valid_versions EQUAL "0;1")
  message(FATAL_ERROR "expected valid version indexes of 0 and 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_GTE 1.8)
if(NOT valid_versions EQUAL 1)
  message(FATAL_ERROR "expected valid versions to be 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_GTE 1.7)
if(NOT valid_versions EQUAL "0;1")
  message(FATAL_ERROR "expected valid version indexes of 0 and 1, got '${valid_versions}'")
endif()

_osrf_testing_tools_cpp_filter_versions(valid_versions versions VERSION_GTE 1.9)
if(valid_versions)
  message(FATAL_ERROR "expected valid versions to be empty, got '${valid_versions}'")
endif()
