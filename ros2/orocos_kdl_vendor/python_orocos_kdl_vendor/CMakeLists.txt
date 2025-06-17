cmake_policy(SET CMP0169 OLD)
cmake_minimum_required(VERSION 4.0)

project(python_orocos_kdl_vendor)

find_package(ament_cmake REQUIRED)

option(FORCE_BUILD_VENDOR_PKG
  "Build python_orocos_kdl from source, even if system-installed package is available"
  OFF)

# Avoid DOWNLOAD_EXTRACT_TIMESTAMP warning for CMake >= 3.24
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

if(NOT FORCE_BUILD_VENDOR_PKG)
  # Check if Python bindings are installed by trying to import from interpreter
  # Figure out Python3 debug/release before anything else can find_package it
  if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_package(python_cmake_module REQUIRED)
    find_package(PythonExtra REQUIRED)

    # Force FindPython3 to use the debug interpreter where ROS 2 expects it
    set(Python3_EXECUTABLE "${PYTHON_EXECUTABLE_DEBUG}")
  endif()
  find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
  execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import PyKDL"
    RESULT_VARIABLE pykdl_import_exit_code
    OUTPUT_QUIET
    ERROR_QUIET
  )
  if(pykdl_import_exit_code EQUAL 0)
    set(pykdl_FOUND 1)
  endif()
endif()

macro(build_pykdl)
  include(FetchContent)

  # This version must match orocos_kdl_vendor exactly
  fetchcontent_declare(
    python_orocos_kdl
    URL https://github.com/orocos/orocos_kinematics_dynamics/archive/507de66205e14b12c8c65f25eafc05c4dc66e21e.zip
    URL_MD5 3f547798ab4461b8247fb764435f3623
  )

  fetchcontent_getproperties(python_orocos_kdl)
  # TODO(sloretz) use FetchContent_MakeAvailable() when CMake 3.14 is the minimum
  if(NOT python_orocos_kdl_POPULATED)
    FetchContent_MakeAvailable(python_orocos_kdl)
    # Make python_orocos_kdl targets part of this project
    add_subdirectory(
      "${python_orocos_kdl_SOURCE_DIR}/python_orocos_kdl"
      ${python_orocos_kdl_BINARY_DIR})
  endif()

  find_package(ament_cmake_python REQUIRED)

  # Figure out where it installed the python module to
  get_directory_property(pykdl_install_dir
    DIRECTORY "${python_orocos_kdl_SOURCE_DIR}/python_orocos_kdl"
    DEFINITION PYTHON_SITE_PACKAGES_INSTALL_DIR)

  # Make an environment hook matching where it got installed
  set(_backup_PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR}")
  set(PYTHON_INSTALL_DIR "${pykdl_install_dir}")
  _ament_cmake_python_register_environment_hook()
  set(PYTHON_INSTALL_DIR "${_backup_PYTHON_INSTALL_DIR}")
endmacro()

if(NOT pykdl_FOUND)
  message(STATUS "Did not find PyKDL installation, building from source")
  build_pykdl()
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
