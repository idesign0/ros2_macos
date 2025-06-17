cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-transport11 VERSION 11.4.1)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake2 2.13 REQUIRED)
set(IGN_CMAKE_VER ${ignition-cmake2_VERSION_MAJOR})

#============================================================================
# Configure the project
#============================================================================
ign_configure_project(
  REPLACE_IGNITION_INCLUDE_PATH gz/transport
)

#============================================================================
# Set project-specific options
#============================================================================

# ignition-transport currently has no options that are unique to it

if (UNIX AND NOT APPLE)
  set (EXTRA_TEST_LIB_DEPS stdc++fs)
else()
  set (EXTRA_TEST_LIB_DEPS)
endif()

#============================================================================
# Search for project-specific dependencies
#============================================================================
message(STATUS "\n\n-- ====== Finding Dependencies ======")

#--------------------------------------
# Find Protobuf
ign_find_package(IgnProtobuf
                 REQUIRED
                 PRETTY Protobuf)

#--------------------------------------
# Find ZeroMQ
ign_find_package(ZeroMQ VERSION 4 REQUIRED PRIVATE)

if (UNIX AND NOT APPLE)
  execute_process(COMMAND lsb_release -cs
    OUTPUT_VARIABLE RELEASE_CODENAME
    RESULT_VARIABLE LSB_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if( NOT (${LSB_RESULT} STREQUAL "0"))
    message(WARNING "lsb_release executable not found. Disabling focal-specific workarounds")
  elseif (${RELEASE_CODENAME} STREQUAL "focal")
    set(UBUNTU_FOCAL 1)
  elseif (${RELEASE_CODENAME} STREQUAL "bullseye")
    set(DEBIAN_BULLSEYE 1)
  endif()
endif()

#--------------------------------------
# Find cppzmq
ign_find_package(CPPZMQ REQUIRED PRIVATE
  PKGCONFIG_IGNORE # NOTE: cppzmq does not seem to offer a pkg-config file
  PRETTY CppZMQ)

#--------------------------------------
# Find uuid
if (MSVC)
  message (STATUS "UUID: Using Windows RPC UuidCreate function\n")
else()
  ign_find_package(UUID REQUIRED)
endif()

#--------------------------------------
# Find ignition-utils
ign_find_package(ignition-utils1 REQUIRED COMPONENTS cli)
set(IGN_UTILS_VER ${ignition-utils1_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
ign_find_package(ignition-msgs8 REQUIRED)
set(IGN_MSGS_VER ${ignition-msgs8_VERSION_MAJOR})

#--------------------------------------
# Find ifaddrs
ign_find_package(IFADDRS QUIET)
if (IFADDRS_FOUND)
  set (HAVE_IFADDRS ON CACHE BOOL "HAVE IFADDRS" FORCE)
else ()
  set (HAVE_IFADDRS OFF CACHE BOOL "HAVE IFADDRS" FORCE)
endif()

#--------------------------------------
# Find if command is available. This is used to enable tests.
# Note that CLI files are installed regardless of whether the dependency is
# available during build time
find_program(HAVE_IGN_TOOLS ign)
set (IGN_TOOLS_VER 1)

#--------------------------------------
# Find SQLite3
ign_find_package(SQLite3
  VERSION 3.7.13
  REQUIRED_BY log
  PRIVATE_FOR log
  PRETTY sqlite3)


#============================================================================
# Configure the build
#============================================================================
ign_configure_build(QUIT_IF_BUILD_ERRORS
  COMPONENTS log parameters)

#============================================================================
# ign command line support
#============================================================================
add_subdirectory(conf)

#============================================================================
# Create package information
#============================================================================
ign_create_packages()

#============================================================================
# Configure documentation
#============================================================================
configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

ign_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md")
