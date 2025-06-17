# Copyright 2025 AIT - Austrian Institute of Technology GmbH
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

# Macro to extract GCC_MAJOR_VERSION and GCC_MINOR_VERSION
macro(extract_gcc_version)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(REPLACE "." ";" VERSION_LIST ${CMAKE_CXX_COMPILER_VERSION})
    list(GET VERSION_LIST 0 GCC_MAJOR_VERSION)
    list(GET VERSION_LIST 1 GCC_MINOR_VERSION)

    message(STATUS "Detected GCC Version: ${CMAKE_CXX_COMPILER_VERSION} (Major: ${GCC_MAJOR_VERSION}, Minor: ${GCC_MINOR_VERSION})")

    # Convert to a number to avoid string comparison issues
    if(GCC_MAJOR_VERSION MATCHES "^[0-9]+$")
      math(EXPR GCC_MAJOR_VERSION "${GCC_MAJOR_VERSION}")
    endif()
    if(GCC_MINOR_VERSION MATCHES "^[0-9]+$")
      math(EXPR GCC_MINOR_VERSION "${GCC_MINOR_VERSION}")
    endif()
  endif()
endmacro()

# set compiler options depending on detected compiler
macro(set_compiler_options)
  if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=conversion -Werror=unused-but-set-variable
                        -Werror=return-type -Werror=shadow -Werror=format
                        -Werror=missing-braces)
    message(STATUS "Compiler warnings enabled for ${CMAKE_CXX_COMPILER_ID}")

    # https://docs.ros.org/en/rolling/How-To-Guides/Ament-CMake-Documentation.html#compiler-and-linker-options
    if(NOT CMAKE_C_STANDARD)
      set(CMAKE_C_STANDARD 99)
    endif()
    if(NOT CMAKE_CXX_STANDARD)
      set(CMAKE_CXX_STANDARD 17)
    endif()

    # Extract major version if g++ is used
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      extract_gcc_version()
      if(DEFINED GCC_MAJOR_VERSION AND GCC_MAJOR_VERSION GREATER 10)
        # GCC 11 introduced -Werror=range-loop-construct
        add_compile_options(-Werror=range-loop-construct)
      endif()
    endif()
  endif()
endmacro()

# using this instead of visibility macros
# S1 from https://github.com/ros-controls/ros2_controllers/issues/1053
macro(export_windows_symbols)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endmacro()
