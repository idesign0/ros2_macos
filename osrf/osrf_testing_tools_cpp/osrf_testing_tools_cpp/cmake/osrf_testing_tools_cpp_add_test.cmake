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

#
# Add a test to ctest.
#
# This macro will add the test to CTest via the `add_test()` cmake call, but
# it will pass the executable and arguments through another executable which
# can alter the environment variables for your test.
#
# :param testname: the name of the test
# :type testname: string
# :param COMMAND: the command including its arguments to invoke
# :type COMMAND: list of strings
# :param TIMEOUT: the test timeout in seconds, default: 60
# :type TIMEOUT: integer
# :param WORKING_DIRECTORY: the working directory for invoking the
#   command in, default: directory of the executable
#   see: https://cmake.org/cmake/help/latest/prop_test/WORKING_DIRECTORY.html
# :type WORKING_DIRECTORY: string
# :param ENV: list of env vars to set; listed as ``VAR=value``
# :type ENV: list of strings
# :param APPEND_ENV: list of env vars to append if already set, otherwise set;
#   listed as ``VAR=value``
# :type APPEND_ENV: list of strings
# :param APPEND_LIBRARY_DIRS: list of library dirs to append to the appropriate
#   OS specific env var, a la LD_LIBRARY_PATH
# :type APPEND_LIBRARY_DIRS: list of strings
#
# @public
#
function(osrf_testing_tools_cpp_add_test testname)
  cmake_parse_arguments(ARG
    ""
    "TIMEOUT;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;COMMAND;ENV"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "osrf_testing_tools_cpp_add_test() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()
  if(NOT ARG_COMMAND)
    message(FATAL_ERROR
      "osrf_testing_tools_cpp_add_test() must be invoked with the COMMAND argument")
  endif()
  if(NOT ARG_TIMEOUT)
    set(ARG_TIMEOUT 60)
  endif()
  if(NOT ARG_TIMEOUT GREATER 0)
    message(FATAL_ERROR "osrf_testing_tools_cpp_add_test() the TIMEOUT argument must be a "
      "valid number and greater than zero")
  endif()
  if(ARG_WORKING_DIRECTORY)
    set(WORKING_DIRECTORY_ARGS WORKING_DIRECTORY "ARG_WORKING_DIRECTORY")
  endif()

  # wrap command with run_test script to ensure test result generation
  set(test_runner_target osrf_testing_tools_cpp::test_runner)
  set(cmd_wrapper "$<TARGET_FILE:${test_runner_target}>")
  if(ARG_ENV)
    list(APPEND cmd_wrapper "--env" ${ARG_ENV})
  endif()
  if(ARG_APPEND_LIBRARY_DIRS)
    if(WIN32)
      set(_library_dirs_env_var "PATH")
    elseif(APPLE)
      set(_library_dirs_env_var "DYLD_LIBRARY_PATH")
    elseif(UNIX)
      set(_library_dirs_env_var "LD_LIBRARY_PATH")
    else()
      message(FATAL_ERROR "Unknown platform for environment variable to find libraries")
    endif()
    foreach(_dir ${ARG_APPEND_LIBRARY_DIRS})
      list(APPEND ARG_APPEND_ENV "${_library_dirs_env_var}=${_dir}")
    endforeach()
  endif()
  if(ARG_APPEND_ENV)
    list(APPEND cmd_wrapper "--append-env" ${ARG_APPEND_ENV})
  endif()
  list(APPEND cmd_wrapper "--" ${ARG_COMMAND})

  add_test(
    NAME "${testname}"
    COMMAND ${cmd_wrapper}
    ${WORKING_DIRECTORY_ARGS}
  )
  set_tests_properties(
    "${testname}"
    PROPERTIES TIMEOUT ${ARG_TIMEOUT}
  )
endfunction()
