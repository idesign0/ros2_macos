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

if(OSRF_TESTING_TOOLS_CPP_REQUIRE_GOOGLETEST_CMAKE__INCLUDED)
  return()
endif()
set(OSRF_TESTING_TOOLS_CPP_REQUIRE_GOOGLETEST_CMAKE__INCLUDED TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/osrf_testing_tools_cpp_get_googletest_versions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/osrf_testing_tools_cpp_extract_and_build_googletest.cmake)
set(OSRF_TESTING_TOOLS_CPP_REQUIRE_GOOGLETEST_VERSION_SETUP)

#
# Provides and sets up googletest at the requested version.
#
# This package provides several versions of googletest, the best of which will
# be selected, built, and setup based on the version constraints given.
# This macro should only be called once, as two googletests being built
# simultaneously is not supported.
#
# :param VERSION_EQ: exact version to match, mutually exclusive with all other options
# :type VERSION_EQ: string
# :param VERSION_LT: version must be less than, mutually exclusive with VERSION_LTE
# :type VERSION_LT: string
# :param VERSION_LTE: version must be less than or equal, mutually exclusive with VERSION_LT
# :type VERSION_LTE: string
# :param VERSION_GT: version must be greater than, mutually exclusive with VERSION_GTE
# :type VERSION_GT: string
# :param VERSION_GTE: version must be greater than or equal, mutually exclusive with VERSION_GT
# :type VERSION_GTE: string
# :param VERSION_SELECTED_OUTPUT: name of variable to output selected version into, optional
# :type VERSION_SELECTED_OUTPUT: string
#
# @public
#
macro(osrf_testing_tools_cpp_require_googletest)
  # Make sure this was only called once.
  if(OSRF_TESTING_TOOLS_CPP_REQUIRE_GOOGLETEST_VERSION_SETUP)
    set(error_msg "osrf_testing_tools_cpp_require_googletest():")
    set(error_msg "${error_msg} googletest already setup for version")
    set(error_msg "${error_msg}  '${OSRF_TESTING_TOOLS_CPP_REQUIRE_GOOGLETEST_VERSION_SETUP}'")
    message(FATAL_ERROR ${error_msg})
  endif()

  # Check if any requested versions match constraints
  _osrf_testing_tools_cpp_require_googletest(__return_variable ${ARGN})
endmacro()

function(_osrf_testing_tools_cpp_filter_versions
  valid_version_indexes_var_name
  version_list
)
  # Arguments are validated in the calling function, _osrf_testing_tools_cpp_require_googletest()
  cmake_parse_arguments(ARG
    ""
    "VERSION_EQ;VERSION_LT;VERSION_LTE;VERSION_GT;VERSION_GTE;VERSION_SELECTED_OUTPUT"
    ""
    ${ARGN})

  set(valid_version_indexes)
  set(current_index 0)
  foreach(version IN LISTS ${version_list})
    set(version_valid FALSE)
    if(ARG_VERSION_EQ)
      if(version VERSION_EQUAL ARG_VERSION_EQ)
        set(version_valid TRUE)
      endif()
    else()
      if(ARG_VERSION_LT)
        if(version VERSION_LESS ARG_VERSION_LT)
          set(version_valid TRUE)
        endif()
      endif()
      if(ARG_VERSION_LTE)
        if(version VERSION_LESS ARG_VERSION_LTE OR version EQUAL ARG_VERSION_LTE)
          set(version_valid TRUE)
        endif()
      endif()
      if(ARG_VERSION_GT)
        if(version VERSION_GREATER ARG_VERSION_GT)
          set(version_valid TRUE)
        endif()
      endif()
      if(ARG_VERSION_GTE)
        if(version VERSION_GREATER ARG_VERSION_GTE OR version EQUAL ARG_VERSION_GTE)
          set(version_valid TRUE)
        endif()
      endif()
    endif()
    if(version_valid)
      list(APPEND valid_version_indexes ${current_index})
    endif()
    math(EXPR current_index "${current_index} + 1")
  endforeach()
  set(${valid_version_indexes_var_name} ${valid_version_indexes} PARENT_SCOPE)
endfunction()

function(_osrf_testing_tools_cpp_require_googletest return_variable)
  cmake_parse_arguments(ARG
    ""
    "VERSION_EQ;VERSION_LT;VERSION_LTE;VERSION_GT;VERSION_GTE;VERSION_SELECTED_OUTPUT;VENDOR_DIR"
    ""
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "osrf_testing_tools_cpp_require_googletest(): unknown args: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # Check for use of mutually exclusive options
  if(ARG_VERSION_EQ AND (ARG_VERSION_LT OR ARG_VERSION_LTE OR ARG_VERSION_GT OR ARG_VERSION_GTE))
    message(FATAL_ERROR
      "osrf_testing_tools_cpp_require_googletest(): VERSION_EQ is a mutually exclusive option")
  endif()
  if(ARG_VERSION_GT AND ARG_VERSION_GTE)
    message(FATAL_ERROR
      "osrf_testing_tools_cpp_require_googletest(): VERSION_GT and VERSION_GTE used together")
  endif()
  if(ARG_VERSION_LT AND ARG_VERSION_LTE)
    message(FATAL_ERROR
      "osrf_testing_tools_cpp_require_googletest(): VERSION_LT and VERSION_LTE used together")
  endif()

  osrf_testing_tools_cpp_get_googletest_versions(versions locations md5s)

  # First generate a list of versions which fit the constraints.
  _osrf_testing_tools_cpp_filter_versions(valid_version_indexes versions ${ARGN})
  list(LENGTH valid_version_indexes valid_version_indexes_length)
  if(valid_version_indexes_length EQUAL 0)
    message(FATAL_ERROR
      "No valid googletest version found in provided versions: ${versions}")
  endif()
  # Use the newest version available, of the valid versions.
  set(valid_versions)
  set(newest_valid_version_index 0)
  list(GET versions 0 newest_valid_version)
  foreach(current_valid_version_index IN LISTS valid_version_indexes)
    list(GET versions ${current_valid_version_index} current_valid_version)
    list(APPEND valid_versions ${current_valid_version})
    if(current_valid_version VERSION_GREATER newest_valid_version)
      set(newest_valid_version_index ${current_valid_version_index})
      set(newest_valid_version ${current_valid_version})
    endif()
  endforeach()
  message(STATUS
    "googletest version '${newest_valid_version}' selected, of versions: '${valid_versions}'")

  if(ARG_VENDOR_DIR)
    set(VENDOR_DIR ${ARG_VENDOR_DIR})
  else()
    if(NOT DEFINED osrf_testing_tools_cpp_DIR)
      message(FATAL_ERROR
        "Cannot locate the googletest vendor directory because osrf_testing_tools_cpp "
        "was not found and the VENDOR_DIR argument was not used.")
    endif()
    set(VENDOR_DIR "${osrf_testing_tools_cpp_DIR}/../../../share/osrf_testing_tools_cpp/vendor")
  endif()

  list(GET locations ${newest_valid_version_index} newest_valid_location)
  set(newest_valid_location "${VENDOR_DIR}/${newest_valid_location}")
  message(STATUS "building googletest from '${newest_valid_location}'...")

  list(GET md5s ${newest_valid_version_index} md5)

  set(add_external_project_file
    "${VENDOR_DIR}/google/googletest/googletest-external-project-add.cmake.in")

  osrf_testing_tools_cpp_extract_and_build_googletest(
    ${newest_valid_location} ${newest_valid_version} ${md5} ${add_external_project_file}
  )
  set(OSRF_TESTING_TOOLS_CPP_REQUIRE_GOOGLETEST_VERSION_SETUP ${newest_valid_version})
endfunction()
