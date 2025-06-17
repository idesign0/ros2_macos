# Copyright 2020 Open Source Robotics Foundation, Inc.
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

macro(_performance_test_fixture_find_memory_tools)
  if(NOT DEFINED _PERFORMANCE_TEST_FIXTURE_FIND_MEMORY_TOOLS)
    set(_PERFORMANCE_TEST_FIXTURE_FIND_MEMORY_TOOLS TRUE)
    find_package(osrf_testing_tools_cpp REQUIRED)

    get_target_property(
      _PERFORMANCE_TEST_FIXTURE_MEMORY_TOOLS_AVAILABLE
      osrf_testing_tools_cpp::memory_tools
      LIBRARY_PRELOAD_ENVIRONMENT_IS_AVAILABLE)

    if(NOT _PERFORMANCE_TEST_FIXTURE_MEMORY_TOOLS_AVAILABLE)
      if(AMENT_RUN_PERFORMANCE_TESTS)
        message(WARNING
          "'osrf_testing_tools_cpp' memory tools are not available, C++ tests "
          "using 'performance_test_fixture' can not be run and will be "
          "skipped.")
      endif()
    else()
      get_target_property(
        _PERFORMANCE_TEST_FIXTURE_MEMORY_TOOLS_ENV
        osrf_testing_tools_cpp::memory_tools
        LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE)
    endif()
  endif()
endmacro()

include("${performance_test_fixture_DIR}/add_performance_test.cmake")
