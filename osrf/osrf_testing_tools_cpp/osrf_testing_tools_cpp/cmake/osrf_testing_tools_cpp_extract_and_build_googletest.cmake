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

# Based on recommendation on how to integrate googletest into a CMake project:
#   https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project

set(__OSRF_TESTING_TOOLS_CPP_GOOGLETEST_VERSION_BUILT)

macro(osrf_testing_tools_cpp_extract_and_build_googletest
  GOOGLETEST_ARCHIVE_LOCATION
  GOOGLETEST_VERSION
  GOOGLETEST_MD5SUM
  GOOGLETEST_EXTERNAL_PROJECT_ADD_TEMPLATE
)
  # Make sure this only happens once
  if(__OSRF_TESTING_TOOLS_CPP_GOOGLETEST_VERSION_BUILT)
    set(error_msg "osrf_testing_tools_cpp_extract_and_build_googletest():")
    set(error_msg "${error_msg} Cannot build requested version '${GOOGLETEST_VERSION}'")
    set(error_msg "${error_msg} because version")
    set(error_msg "${error_msg} '${__OSRF_TESTING_TOOLS_CPP_GOOGLETEST_VERSION_BUILT}'")
    set(error_msg "${error_msg} has already been built.")
    message(FATAL_ERROR ${error_msg})
  endif()
  set(__OSRF_TESTING_TOOLS_CPP_GOOGLETEST_VERSION_BUILT ${GOOGLETEST_VERSION})

  # Extract and unpack googletest at configure time

  # Explicitly reset these variables otherwise configure_file will not use them
  set(GOOGLETEST_ARCHIVE_LOCATION ${GOOGLETEST_ARCHIVE_LOCATION})
  set(GOOGLETEST_VERSION ${GOOGLETEST_VERSION})
  set(GOOGLETEST_MD5SUM ${GOOGLETEST_MD5SUM})
  configure_file(
    "${GOOGLETEST_EXTERNAL_PROJECT_ADD_TEMPLATE}"
    googletest-${GOOGLETEST_VERSION}-extracted/CMakeLists.txt
    @ONLY
  )

  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-${GOOGLETEST_VERSION}-extracted)
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()

  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-${GOOGLETEST_VERSION}-extracted)
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  set(__prefix googletest-${GOOGLETEST_VERSION})
  if(UNIX AND NOT APPLE)
    # When building with asan (i.e. using colcon build --mixin asan-gcc),
    # asan itself provides a "fake" pthread that tricks the pthread
    # detection logic into thinking no link libraries are necessary
    # (see https://wiki.gentoo.org/wiki/AddressSanitizer/Problems#pthread_linking_issues
    # for some additional information).  To work around that, we unconditionally
    # add the -pthread flag for Linux machines so it will always work
    link_libraries("-pthread")
  endif()
  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(
    ${CMAKE_BINARY_DIR}/${__prefix}-extracted/${__prefix}-src
    ${CMAKE_BINARY_DIR}/${__prefix}-extracted/${__prefix}-build
    EXCLUDE_FROM_ALL
  )
  unset(__prefix)

  if(WIN32)
    # Googletest has some known warnings as errors on newer versions of VS, see:
    #   https://github.com/google/googletest/issues/1373
    # So disable offending warnings for now.
    if(TARGET gtest)  # Only in 1.7 and not 1.8
      target_compile_definitions(gtest PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING=1)
    endif()
    if(TARGET gtest_main)
      target_compile_definitions(gtest_main PUBLIC _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING=1)
    endif()
  endif()

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endmacro()
