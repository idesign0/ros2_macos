# Copyright 2019-2020 Open Source Robotics Foundation, Inc.
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

list(INSERT CMAKE_MODULE_PATH 0 "${tinyxml2_vendor_DIR}/Modules")

if(TinyXML2_FOUND AND NOT TARGET tinyxml2::tinyxml2)
  message(STATUS "TinyXML2 was already found when tinyxml2_vendor was included, adding missing imported target tinyxml2::tinyxml2")
  # TINYXML2_LIBRARY is composed of debug;<path\to\debug.lib>;optimized;<path\to\release.lib>
  # we have to extract the appropriate component based on the current configuration.
  list(LENGTH TINYXML2_LIBRARY TINYXML_LIBRARY_LIST_LENGTH)
  if(NOT ${TINYXML_LIBRARY_LIST_LENGTH} EQUAL 4)
    message(FATAL_ERROR "Unable to extract the library file path from ${TINYXML2_LIBRARY}")
  endif()
  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    list(GET TINYXML2_LIBRARY 0 ASSERT_DEBUG)
    if(NOT ${ASSERT_DEBUG} STREQUAL "debug")
      message(FATAL_ERROR "could not parse debug library path from ${TINYXML2_LIBRARY}")
    endif()
    list(GET TINYXML2_LIBRARY 1 TINYXML2_LIBRARY_PATH)
  else()
    list(GET TINYXML2_LIBRARY 2 ASSERT_OPTIMIZED)
    if(NOT ${ASSERT_OPTIMIZED} STREQUAL "optimized")
      message(FATAL_ERROR "could not parse library path from ${TINYXML2_LIBRARY}")
    endif()
    list(GET TINYXML2_LIBRARY 3 TINYXML2_LIBRARY_PATH)
  endif()
  if(NOT EXISTS ${TINYXML2_LIBRARY_PATH})
    message(FATAL_ERROR "library file path ${TINYXML2_LIBRARY_PATH} does not exist")
  endif()

  add_library(tinyxml2::tinyxml2 UNKNOWN IMPORTED)
  set_property(TARGET tinyxml2::tinyxml2 PROPERTY IMPORTED_LOCATION ${TINYXML2_LIBRARY_PATH})
  set_property(TARGET tinyxml2::tinyxml2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${TINYXML2_INCLUDE_DIR})
  list(APPEND TinyXML2_TARGETS tinyxml2::tinyxml2)
endif()
