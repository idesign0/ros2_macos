cmake_minimum_required(VERSION 4.0)

project(yaml_cpp_vendor)

option(FORCE_BUILD_VENDOR_PKG
  "Build yaml-cpp from source, even if system-installed package is available"
  OFF)

find_package(ament_cmake REQUIRED)

# Avoid DOWNLOAD_EXTRACT_TIMESTAMP warning for CMake >= 3.24
if (POLICY CMP0135)
  cmake_policy(SET CMP0135 OLD)
endif()

macro(build_yaml_cpp)
  set(extra_cmake_args)

  if(DEFINED CMAKE_BUILD_TYPE)
    list(APPEND extra_cmake_args -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
  endif()
  if(NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -w")
  endif()

  list(APPEND extra_cmake_args "-DYAML_CPP_BUILD_TESTS=OFF")
  list(APPEND extra_cmake_args "-DYAML_CPP_BUILD_TOOLS=OFF")
  list(APPEND extra_cmake_args "-DYAML_CPP_BUILD_CONTRIB=OFF")
  list(APPEND extra_cmake_args "-DYAML_BUILD_SHARED_LIBS=ON")
  list(APPEND extra_cmake_args "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
  list(APPEND extra_cmake_args "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

  if(WIN32 AND NOT ${CMAKE_VERBOSE_MAKEFILE})
    set(should_log ON)  # prevent warnings in Windows CI
  else()
    set(should_log OFF)
  endif()

  if(DEFINED CMAKE_TOOLCHAIN_FILE)
    list(APPEND extra_cmake_args "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
  endif()

  include(ExternalProject)
  ExternalProject_Add(yaml_cpp-0.7.0
    URL https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.7.0.tar.gz
    URL_MD5 74d646a3cc1b5d519829441db96744f0
    TIMEOUT 600
    LOG_CONFIGURE ${should_log}
    LOG_BUILD ${should_log}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/yaml_cpp_install
      ${extra_cmake_args}
      -Wno-dev
  )

  # The external project will install to the build folder, but we'll install that on make install.
  install(
    DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/yaml_cpp_install/
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/opt/yaml_cpp_vendor
    USE_SOURCE_PERMISSIONS
  )
endmacro()

# NO_CMAKE_PACKAGE_REGISTRY used to avoid finding the library downloaded in WORKSPACE B
# when building workspace A.
# This should only find a system installed yaml-cpp and thus the environment hook isn't needed.
find_package(yaml-cpp QUIET NO_CMAKE_PACKAGE_REGISTRY)
if(FORCE_BUILD_VENDOR_PKG OR NOT yaml-cpp_FOUND)
  build_yaml_cpp()

  if(WIN32)
    ament_environment_hooks(env_hook/yaml_cpp_vendor_library_path.bat)
    set(ENV_VAR_NAME "PATH")
    set(ENV_VAR_VALUE "opt\\yaml_cpp_vendor\\bin")
  else()
    ament_environment_hooks(env_hook/yaml_cpp_vendor_library_path.sh)
    if(APPLE)
      set(ENV_VAR_NAME "DYLD_LIBRARY_PATH")
    else()
      set(ENV_VAR_NAME "LD_LIBRARY_PATH")
    endif()
    set(ENV_VAR_VALUE "opt/yaml_cpp_vendor/lib")
  endif()
  ament_environment_hooks(env_hook/yaml_cpp_vendor_library_path.dsv.in)
else()
  message(STATUS "Found yaml-cpp ${yaml-cpp_VERSION} in path ${yaml-cpp_CONFIG}")
endif()

ament_package(
  CONFIG_EXTRAS "yaml_cpp_vendor-extras.cmake.in"
)
