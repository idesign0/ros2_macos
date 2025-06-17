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

include(CMakeParseArguments)

################################################################################
# rti_lib_suffix()
################################################################################
function(rti_lib_suffix var)
    set(rti_lib_sfx         "")

    if("${CMAKE_BUILD_TYPE}" MATCHES "[dD]ebug" OR
        "${CMAKE_BUILD_TYPE}" MATCHES "RelWithDebInfo")
        string(APPEND rti_lib_sfx "d")
    endif()

    set(${var} "${rti_lib_sfx}" PARENT_SCOPE)
endfunction()

################################################################################
# rti_init_env()
################################################################################
function(rti_init_env)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Release"
            CACHE STRING "Default build type" FORCE)
    endif()

    set(connext_lib_sfx             "")

    if("${CMAKE_BUILD_TYPE}" MATCHES "[dD]ebug" OR
        "${CMAKE_BUILD_TYPE}" MATCHES "RelWithDebInfo")
        string(APPEND connext_lib_sfx       "d")
    endif()

    set(RTI_LIB_SUFFIX  ${connext_lib_sfx}
        CACHE STRING "Suffix for RTI libraries based on target build")
endfunction()

################################################################################
# _list_first_element(list_var match_prefix out_var)
################################################################################
function(_list_first_element list_var match_prefix out_var)
    file(GLOB ${list_var} "${match_prefix}")
    list(LENGTH ${list_var} ${list_var}_len)
    if(${${list_var}_len} GREATER 0)
        list(GET ${list_var} 0 first_el)
        set(${out_var} "${first_el}" PARENT_SCOPE)
    endif()
endfunction()

################################################################################
# rti_load_rtimehome()
################################################################################
function(rti_load_rtimehome)
    set(RTIMEHOME_USER  false)
    if(NOT DEFINED RTIMEHOME)
        if(NOT "$ENV{RTIMEHOME}" STREQUAL "")
            file(TO_CMAKE_PATH "$ENV{RTIMEHOME}" RTIMEHOME)
            set(RTIMEHOME_USER  true)
        else()
            rti_load_connextddsdir()

            if(CONNEXTDDS_DIR_FOUND)
                _list_first_element(rtime_installations
                    "${CONNEXTDDS_DIR}/rti_connext_dds_micro-*"
                    RTIMEHOME)
            endif()
        endif()
    else()
      set(RTIMEHOME_USER  true)
    endif()

    if("${RTIMEHOME}" STREQUAL "" OR
        NOT EXISTS "${RTIMEHOME}" OR
        NOT EXISTS "${RTIMEHOME}/CMakeLists.txt")
        set(RTIMEHOME_FOUND false)
        message(STATUS "Invalid RTIMEHOME = '${RTIMEHOME}'")
    else()
        set(RTIMEHOME_FOUND true)
        message(STATUS "Detected RTIMEHOME = '${RTIMEHOME}'")

        # Detect Micro version
        if(NOT DEFINED RTIME_VERSION)
          get_filename_component(rtimehome_base "${RTIMEHOME}" NAME)
          string(REGEX REPLACE
            "^rti_connext_dds_micro[_a-z]*-([0-9.a-z\\-_]+)$"
            "\\1"
            RTIME_VERSION
            "${rtimehome_base}")
          if("${RTIME_VERSION}" STREQUAL "")
            set(RTIME_VERSION "unknown")
          endif()
        endif()
    endif()

    set(RTIMEHOME "${RTIMEHOME}" PARENT_SCOPE)
    set(RTIMEHOME_FOUND "${RTIMEHOME_FOUND}" PARENT_SCOPE)
    set(RTIMEHOME_USER  "${RTIMEHOME_USER}" PARENT_SCOPE)
    set(RTIME_VERSION "${RTIME_VERSION}" PARENT_SCOPE)
endfunction()

################################################################################
# rti_build_connextmicro()
################################################################################
function(rti_build_connextmicro)
    set(extra_components ${ARGN})
    list(FIND extra_components "security_plugins" component_security)

    set(RTIMEHOME_FOUND false PARENT_SCOPE)
    set(RTIMEHOME_USER false PARENT_SCOPE)
    set(RTIConnextDDSMicro_FOUND false PARENT_SCOPE)
    set(RTIME_TARGETS "" PARENT_SCOPE)

    rti_load_rtimehome()

    if(NOT RTIME_TARGET_NAME)
      if(WIN32 OR CYGWIN)
          set(RTIME_TARGET_NAME       "Windows")
      elseif(APPLE)
          set(RTIME_TARGET_NAME       "Darwin")
      else()
          set(RTIME_TARGET_NAME       "Linux")
      endif()
    endif()
    set(RTIME_TARGET_NAME           "${RTIME_TARGET_NAME}")
    set(RTIME_DDS_ENABLE_APPGEN     false)
    set(RTIME_NO_SHARED_LIB         false)
    set(RTIME_EXCLUDE_DPSE          true)
    set(RTIME_EXCLUDE_CPP           true)
    if(${component_security} GREATER_EQUAL 0)
      set(RTIME_TRUST_INCLUDE_INTERFACES   true)
    endif()

    add_subdirectory(${RTIMEHOME} rtime)

    rti_lib_suffix(rti_lib_sfx)

    set(rtime_libraries
        rti_me${rti_lib_sfx}
        rti_me_netiosdm${rti_lib_sfx}
        rti_me_netioshmem${rti_lib_sfx}
        rti_me_whsm${rti_lib_sfx}
        rti_me_rhsm${rti_lib_sfx}
        rti_me_discdpde${rti_lib_sfx})

    if(TARGET rti_me_ddssecurity${rti_lib_sfx} AND
        TARGET rti_me_seccore${rti_lib_sfx})
      list(APPEND rtime_libraries
          rti_me_ddssecurity${rti_lib_sfx}
          rti_me_seccore${rti_lib_sfx})
      set(rtime_have_security     true)
    endif()

    set(RTIME_TARGETS)
    # define imported targets to match those that would be defined
    # by RTIFindConnextDDSMicro.cmake
    foreach(rtime_lib ${rtime_libraries})
        if("${rtime_lib}" STREQUAL "rti_me${rti_lib_sfx}")
          set(rtime_tgt     "c_api")
        else()
          string(REGEX REPLACE "^rti_me_" "" rtime_tgt "${rtime_lib}")
          if(NOT "${rti_lib_sfx}" STREQUAL "")
            string(REGEX REPLACE "${rti_lib_sfx}$" "" rtime_tgt "${rtime_tgt}")
          endif()
        endif()

        if(NOT TARGET RTIConnextDDSMicro::${rtime_tgt})
            # add_custom_target(RTIConnextDDSMicro::${rtime_tgt}
            #     DEPENDS ${rtime_tgt})
            add_library(RTIConnextDDSMicro::${rtime_tgt} SHARED IMPORTED)
            set_target_properties(RTIConnextDDSMicro::${rtime_tgt}
              PROPERTIES
                IMPORTED_LINK_INTERFACE_LIBRARIES
                  ${rtime_lib}
            )
        endif()
        list(APPEND RTIME_TARGETS RTIConnextDDSMicro::${rtime_tgt})
    endforeach()

    set(RTIME_TARGETS ${RTIME_TARGETS} PARENT_SCOPE)
    set(RTIMEHOME_FOUND true PARENT_SCOPE)
    set(RTIMEHOME_USER ${RTIMEHOME_USER} PARENT_SCOPE)
    set(RTIConnextDDSMicro_FOUND true PARENT_SCOPE)
    # Re-export RTIME_TARGET_NAME and RTIMEHOME in case they were
    # automatically detected.
    set(RTIME_TARGET_NAME ${RTIME_TARGET_NAME} PARENT_SCOPE)
    set(RTIMEHOME ${RTIMEHOME} PARENT_SCOPE)
endfunction()

################################################################################
# rti_find_connextmicro_lib(basedir rtime_lib)
################################################################################
function(rti_find_connextmicro_lib basedir rtime_lib)
    find_library(lib${rtime_lib}
        NAME
            ${rtime_lib}
        PATHS
            "${basedir}"
        NO_DEFAULT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

    if("${lib${rtime_lib}}" MATCHES NOTFOUND)
        message(STATUS "Library ${rtime_lib} not found in ${basedir}. RTI Connext DDS Micro will not be available")
        set(${rtime_lib}_FOUND false PARENT_SCOPE)
        return()
    endif()

    set(lib${rtime_lib} ${lib${rtime_lib}} PARENT_SCOPE)
    message(STATUS "Found library ${rtime_lib}: ${lib${rtime_lib}} (${lib${rtime_lib}-NOTFOUND})")
    set(${rtime_lib}_FOUND true PARENT_SCOPE)
endfunction()

################################################################################
# rti_find_connextmicro()
################################################################################
function(rti_find_connextmicro)
    set(extra_components ${ARGN})

    set(RTIMEHOME_FOUND             false PARENT_SCOPE)
    set(RTIMEHOME_USER              false PARENT_SCOPE)
    set(RTIConnextDDSMicro_FOUND    false PARENT_SCOPE)
    set(RTIME_TARGETS               "" PARENT_SCOPE)

    rti_lib_suffix(rti_lib_sfx)

    set(rtime_libraries
        rti_me${rti_lib_sfx}
        rti_me_netiosdm${rti_lib_sfx}
        rti_me_netioshmem${rti_lib_sfx}
        rti_me_whsm${rti_lib_sfx}
        rti_me_rhsm${rti_lib_sfx}
        rti_me_discdpde${rti_lib_sfx})

    list(FIND extra_components "security_plugins" component_security)
    if(${component_security} GREATER_EQUAL 0)
      list(APPEND rtime_libraries
        rti_me_ddssecurity${rti_lib_sfx}
        rti_me_seccore${rti_lib_sfx})
    endif()

    rti_load_rtimehome()

    set(RTIMEHOME_USER              ${RTIMEHOME_USER} PARENT_SCOPE)

    if(NOT RTIMEHOME_FOUND)
        message(STATUS "RTIMEHOME not found")
        if(RTIMEHOME_USER)
          message(WARNING
            "Invalid RTIMEHOME specified. "
            "RTI Connext DDS Micro will not be available: '${RTIMEHOME}'")
        endif()
        return()
    endif()

    set(RTIMEHOME_FOUND     true PARENT_SCOPE)
    set(RTIMEHOME           "${RTIMEHOME}" PARENT_SCOPE)

    if(NOT DEFINED RTIME_TARGET_NAME)
        _list_first_element(rtime_installations
            "${RTIMEHOME}/lib/*"
            rtime_target_dir)

        if("${rtime_target_dir}" STREQUAL "")
            message(STATUS
              "No RTI Connext DDS Micro libraries found under ${RTIMEHOME}/lib")
        else()
            get_filename_component(RTIME_TARGET_NAME
                "${rtime_target_dir}" NAME CACHE)
            message(STATUS
              "Automatically detected "
              "RTIME_TARGET_NAME = '${RTIME_TARGET_NAME}'")
        endif()
    endif()

    set(rtime_lib_dir       "${RTIMEHOME}/lib/${RTIME_TARGET_NAME}")
    set(rtime_inc_dir       "${RTIMEHOME}/include")

    set(location_property IMPORTED_LOCATION)
    if(WIN32)
        set(location_property IMPORTED_IMPLIB)
    endif()

    list(GET rtime_libraries 0 rtime_core)
    set(rtime_extra ${rtime_libraries})
    list(REMOVE_AT rtime_extra 0)

    rti_find_connextmicro_lib(${rtime_lib_dir} ${rtime_core})

    if(NOT ${rtime_core}_FOUND)
        message(STATUS "Required library not found: ${rtime_core}")
        return()
    endif()

    set(rtime_core_inc_dirs
        "${rtime_inc_dir}"
        "${RTIMEHOME}/src/reda/sequence")

    add_library(RTIConnextDDSMicro::c_api SHARED IMPORTED)
    set_target_properties(RTIConnextDDSMicro::c_api
        PROPERTIES
            IMPORTED_NO_SONAME TRUE
            ${location_property}
                "${lib${rtime_core}}"
            INTERFACE_INCLUDE_DIRECTORIES
                "${rtime_core_inc_dirs}")

    if(RTIME_TARGET_NAME MATCHES "Win")
      set_target_properties(RTIConnextDDSMicro::c_api
        PROPERTIES
            INTERFACE_COMPILE_DEFINITIONS
                RTIME_DLL_VARIABLE)
    endif()

    set(rtime_targets     RTIConnextDDSMicro::c_api)

    foreach(rtime_lib ${rtime_extra})
        rti_find_connextmicro_lib(${rtime_lib_dir} ${rtime_lib})

        if(NOT ${rtime_lib}_FOUND)
            message(STATUS "Required library not found: ${rtime_lib}")
            return()
        endif()

        string(REGEX REPLACE "^rti_me_" "" rtime_tgt "${rtime_lib}")
        if(NOT "${rti_lib_sfx}" STREQUAL "")
          string(REGEX REPLACE "${rti_lib_sfx}$" "" rtime_tgt "${rtime_tgt}")
        endif()

        add_library(RTIConnextDDSMicro::${rtime_tgt} SHARED IMPORTED)
        set_target_properties(RTIConnextDDSMicro::${rtime_tgt}
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${lib${rtime_lib}}"
                INTERFACE_INCLUDE_DIRECTORIES
                    "${rtime_inc_dir}"
                INTERFACE_LINK_LIBRARIES
                    RTIConnextDDSMicro::c_api)
        list(APPEND rtime_targets RTIConnextDDSMicro::${rtime_tgt})
    endforeach()

    message(STATUS "RTIME_TARGETS: ${rtime_targets}")

    set(RTIMEHOME_FOUND true PARENT_SCOPE)
    set(RTIMEHOME "${RTIMEHOME}" PARENT_SCOPE)
    set(RTIConnextDDSMicro_FOUND true PARENT_SCOPE)
    set(RTIME_VERSION "${RTIME_VERSION}" PARENT_SCOPE)
    set(RTIME_TARGETS ${rtime_targets} PARENT_SCOPE)
    set(RTIME_TARGET_NAME ${RTIME_TARGET_NAME} PARENT_SCOPE)
endfunction()

################################################################################
# rti_load_connextddsdir()
################################################################################
function(rti_load_connextddsdir)
    if(NOT DEFINED CONNEXTDDS_DIR)
        if(NOT "$ENV{CONNEXTDDS_DIR}" STREQUAL "")
            file(TO_CMAKE_PATH "$ENV{CONNEXTDDS_DIR}" connextdds_dir)
        elseif(DEFINED NDDSHOME)
            set(connextdds_dir          "${NDDSHOME}")
        elseif(NOT "$ENV{NDDSHOME}" STREQUAL "")
            file(TO_CMAKE_PATH "$ENV{NDDSHOME}" connextdds_dir)
        endif()
        if("${connextdds_dir}" STREQUAL "")
            message(STATUS "no CONNEXTDDS_DIR nor NDDSHOME specified")
        endif()
        set(CONNEXTDDS_DIR "${connextdds_dir}")
    endif()
    set(CONNEXTDDS_DIR "${CONNEXTDDS_DIR}"
        CACHE STRING
        "Installation directory for RTI Connext DDS Professional")
    if("${CONNEXTDDS_DIR}" STREQUAL "" OR
        NOT EXISTS "${CONNEXTDDS_DIR}")
        set(CONNEXTDDS_DIR_FOUND false)
        message(STATUS "Invalid CONNEXTDDS_DIR = '${CONNEXTDDS_DIR}'")
    else()
        set(CONNEXTDDS_DIR_FOUND true)
        message(STATUS "Detected CONNEXTDDS_DIR = '${CONNEXTDDS_DIR}'")
    endif()

    set(CONNEXTDDS_DIR "${CONNEXTDDS_DIR}" PARENT_SCOPE)
    set(CONNEXTDDS_DIR_FOUND "${CONNEXTDDS_DIR_FOUND}" PARENT_SCOPE)
endfunction()

################################################################################
# rti_guess_connextdds_arch()
# This function is copied from FindRTIConnextDDS.cmake@6.0.1 and it is used
# to avoid a bug (particularly on macOS) present in older versions of Connext
# (e.g. 5.3.1) when there are hidden files in `${CONNEXTDDS_DIR}/lib`
# (e.g. ".DS_Store").
################################################################################
function(rti_guess_connextdds_arch)
  if(NOT "$ENV{CONNEXTDDS_ARCH}" STREQUAL "")
    set(CONNEXTDDS_ARCH $ENV{CONNEXTDDS_ARCH} PARENT_SCOPE)
    return()
  endif()

  message(STATUS "CONNEXTDDS_ARCH not provided, trying to guess it...")

  # Guess the RTI Connext DDS architecture

  if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    string(REGEX REPLACE "^([0-9]+).*$" "\\1"
        major_version
        ${CMAKE_CXX_COMPILER_VERSION})
    set(version_compiler "${major_version}.0")

    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\1"
        kernel_version "${CMAKE_SYSTEM_VERSION}")

    set(guessed_architecture
        "x64Darwin${kernel_version}${version_compiler}")
  elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86")
      set(connextdds_host_arch "i86Win32")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
      set(connextdds_host_arch "x64Win64")
    else()
      message(FATAL_ERROR
        "${CMAKE_HOST_SYSTEM} is not supported as host architecture")
    endif()

    string(REGEX MATCH "[0-9][0-9][0-9][0-9]"
      vs_year
      "${CMAKE_GENERATOR}")

    set(guessed_architecture "${connextdds_host_arch}VS${vs_year}")
  elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    if(CMAKE_COMPILER_VERSION VERSION_EQUAL "4.6.3")
      set(kernel_version "3.x")
    else()
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\1"
        kernel_version
        "${CMAKE_SYSTEM_VERSION}")
    endif()

    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
      set(connextdds_host_arch "x64Linux")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686")
      set(connextdds_host_arch "i86Linux")
    endif()

    set(guessed_architecture
      "${connextdds_host_arch}${kernel_version}gcc${CMAKE_C_COMPILER_VERSION}")
  endif()

  if(EXISTS "${CONNEXTDDS_DIR}/lib/${guessed_architecture}")
    set(CONNEXTDDS_ARCH "${guessed_architecture}")
    message(STATUS
      "Guessed ${CONNEXTDDS_DIR}/lib/${guessed_architecture} exists")
  else()
    # If CONNEXTDDS_ARCH is unspecified, the module tries uses the first
    # architecture installed by looking under $CONNEXTDDS_DIR/lib that matches
    # the detected host architecture
    file(GLOB architectures_installed
    RELATIVE "${CONNEXTDDS_DIR}/lib"
    "${CONNEXTDDS_DIR}/lib/*")

    message(STATUS
      "Guessed CONNEXTDDS_ARCH ('${guessed_architecture}') not available.")
    message(STATUS
      "Pick first from ${CONNEXTDDS_DIR}/lib/[${architectures_installed}]")

    foreach(architecture_name ${architectures_installed})
      # Because the lib folder contains both target libraries and
      # Java JAR files, here we exclude the "java" in our algorithm
      # to guess the appropriate CONNEXTDDS_ARCH variable.
      # We also exclude any file that doesn't start with a character or number
      # since they are unlikely to be architecture names.
      if(architecture_name STREQUAL "java" OR
        NOT architecture_name MATCHES "[a-zA-Z0-9].*"
      )
        message(STATUS "ignored: ${architecture_name}")
        continue()
      elseif(architecture_name MATCHES ${CMAKE_HOST_SYSTEM_NAME} OR
        (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows" AND
          architecture_name MATCHES "Win")
      )
        if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
          # Get the installed Darwin
          set(CONNEXTDDS_ARCH "${architecture_name}")
        elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
          if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86" AND
            "${architecture_name}" MATCHES "Win32")
            # Get the x86Win32 architecture
            set(CONNEXTDDS_ARCH "${architecture_name}")
          elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64" AND
            "${architecture_name}" MATCHES "Win64")
            # Get the x64Win64 architecture
            set(CONNEXTDDS_ARCH "${architecture_name}")
          endif()
        elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
          if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686" AND
            "${architecture_name}" MATCHES "x86Linux")
            # Get the x86Linux architecture
            set(CONNEXTDDS_ARCH "${architecture_name}")
          elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64" AND
            "${architecture_name}" MATCHES "x64Linux")
            # Get the x64Linux architecture
            set(CONNEXTDDS_ARCH "${architecture_name}")
          endif()
        endif()
        if(NOT CONNEXTDDS_ARCH)
          message(STATUS
            "unsupported CMAKE_HOST_SYSTEM_NAME (${CMAKE_HOST_SYSTEM_NAME}) "
            "or CMAKE_HOST_SYSTEM_PROCESSOR (${CMAKE_HOST_SYSTEM_PROCESSOR}). "
            "Using architecture ${architecture_name} anyway.")
          set(CONNEXTDDS_ARCH "${architecture_name}")
        endif()
      else()
        message(STATUS "ignored foreign architecture: ${architecture_name}")
      endif()

      if(CONNEXTDDS_ARCH)
        break()
      endif()
    endforeach()
  endif()

  if(NOT CONNEXTDDS_ARCH)
    message(WARNING
      "CONNEXTDDS_ARCH not specified. Please set "
      "-DCONNEXTDDS_ARCH= to specify your RTI Connext DDS "
      " architecture")
  else()
      message(STATUS "Selected CONNEXTDDS_ARCH: ${CONNEXTDDS_ARCH}")
  endif()


  set(CONNEXTDDS_ARCH "${CONNEXTDDS_ARCH}" PARENT_SCOPE)
endfunction()

################################################################################
# rti_find_connextpro()
################################################################################
function(rti_find_connextpro)
    set(extra_components ${ARGN})

    rti_load_connextddsdir()

    if(NOT CONNEXTDDS_DIR_FOUND)
        set(RTIConnextDDS_FOUND false)
    else()
        # TODO(asorbini) Remove guessing logic after support for older Connext
        # (< 6.x) is dropped.
        if(NOT CONNEXTDDS_ARCH)
          rti_guess_connextdds_arch()
        endif()

        list(APPEND CMAKE_MODULE_PATH
            "${CONNEXTDDS_DIR}/resource/cmake")
        set(BUILD_SHARED_LIBS true)
        set(CONNEXTDDS_VERSION      "5.3.1")
        # Set RTICODEGEN_DIR to suppress warnings from FindRTIConnextDDS.cmake
        # in case the installation of Connext only contains runtime libraries.
        set(RTICODEGEN_DIR "${CONNEXTDDS_DIR}/bin")
        find_package(RTIConnextDDS  "${CONNEXTDDS_VERSION}"
            COMPONENTS     core ${extra_components})
    endif()

    if(RTIConnextDDS_FOUND AND NOT TARGET RTIConnextDDS::c_api)
      if("${CONNEXTDDS_DIR}" VERSION_GREATER_EQUAL "6.0.0")
        message(FATAL_ERROR
          "Expected imported targets to be created by "
          "FindRTIConnextDDS.cmake 6.x")
      endif()

      # define imported targets, since they are not defined by
      # FindRTIConnextDDS.cmake before 6.0.0

      if("${CMAKE_BUILD_TYPE}" MATCHES "[dD]ebug")
          set(release_type DEBUG)
      else()
          set(release_type RELEASE)
      endif()

      set(location_property IMPORTED_LOCATION)
      if(WIN32)
          set(location_property IMPORTED_IMPLIB)
      endif()

      add_library(RTIConnextDDS::c_api SHARED IMPORTED)
      list(GET CONNEXTDDS_C_API_LIBRARIES_${release_type}_SHARED 0 libnddsc)
      set(no_as_needed)
      set(as_needed)
      # Pass `--no-as-needed` to linker for "system" dependencies if we are
      # using `gcc`, as specified in the Connext 5.3.1 Platform Notes.
      if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(no_as_needed "-Wl,--no-as-needed")
        set(as_needed "-Wl,--as-needed")
      endif()
      set(c_api_libs
          ${CONNEXTDDS_C_API_LIBRARIES_${release_type}_SHARED}
          "${no_as_needed}"
          ${CONNEXTDDS_EXTERNAL_LIBS}
          "${as_needed}")
      list(REMOVE_AT c_api_libs 0)
      # Iterate over definitions list and detect any options not starting with
      # -D. Conside these options, and not definitions.
      set(ndds_compile_opts)
      set(ndds_defines)
      string(REPLACE " " ";" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
      foreach(ndds_def ${CONNEXTDDS_DEFINITIONS} ${CONNEXTDDS_DLL_EXPORT_MACRO})
          if(ndds_def MATCHES "^-D")
            string(REGEX REPLACE "^-D" "" ndds_def "${ndds_def}")
            list(APPEND ndds_defines "${ndds_def}")
          else()
            list(APPEND ndds_compile_opts "${ndds_def}")
          endif()
      endforeach()
      set_target_properties(RTIConnextDDS::c_api
              PROPERTIES
                ${location_property}
                  "${libnddsc}"
                IMPORTED_LINK_INTERFACE_LIBRARIES
                  "${c_api_libs}"
                INTERFACE_INCLUDE_DIRECTORIES
                  "${CONNEXTDDS_INCLUDE_DIRS}"
                INTERFACE_COMPILE_DEFINITIONS
                  "${ndds_defines}"
                INTERFACE_COMPILE_OPTIONS
                  "${ndds_compile_opts}"
                IMPORTED_NO_SONAME
                  true)
    endif()

    set(RTIConnextDDS_FOUND ${RTIConnextDDS_FOUND} PARENT_SCOPE)
    set(CONNEXTDDS_DIR      "${CONNEXTDDS_DIR}" PARENT_SCOPE)
    set(CONNEXTDDS_ARCH     "${CONNEXTDDS_ARCH}" PARENT_SCOPE)
    set(CONNEXTDDS_VERSION  "${RTICONNEXTDDS_VERSION}" PARENT_SCOPE)
endfunction()

