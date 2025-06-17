cmake_minimum_required(VERSION 4.0)

project(libcurl_vendor)

option(FORCE_BUILD_VENDOR_PKG
  "Build libcurl from source, even if system-installed package is available"
  OFF)

find_package(ament_cmake REQUIRED)

# Avoid DOWNLOAD_EXTRACT_TIMESTAMP warning for CMake >= 3.24
if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

macro(build_libcurl)
  set(extra_cxx_flags)
  set(extra_c_flags)
  if(NOT WIN32)
    list(APPEND extra_cxx_flags "-std=c++14 -w")
    list(APPEND extra_c_flags "-w")
  endif()

  set(PKGCONFIG_FOR_OPENSSL)
  if(APPLE)
    set(PKGCONFIG_FOR_OPENSSL "/usr/local/opt/openssl/lib/pkgconfig")
  endif()

  if(WIN32 AND NOT ${CMAKE_VERBOSE_MAKEFILE})
    set(should_log ON)  # prevent warnings in Windows CI
  else()
    set(should_log OFF)
  endif()

  include(ExternalProject)
  if(WIN32)
    ExternalProject_Add(curl-7.81.0
      URL https://github.com/curl/curl/releases/download/curl-7_81_0/curl-7.81.0.tar.gz
      URL_MD5 9e5e81fc7657eea8dc66672768082c46
      LOG_CONFIGURE ${should_log}
      LOG_BUILD ${should_log}
      CMAKE_ARGS
        -DENABLE_MANUAL=OFF
        -DBUILD_TESTING=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/libcurl_install
        -Wno-dev
      TIMEOUT 600
    )
  else()
    ExternalProject_Add(curl-7.81.0
      URL https://github.com/curl/curl/releases/download/curl-7_81_0/curl-7.81.0.tar.gz
      URL_MD5 9e5e81fc7657eea8dc66672768082c46
      CONFIGURE_COMMAND
        <SOURCE_DIR>/configure
        CFLAGS=${extra_c_flags}
        CXXFLAGS=${extra_cxx_flags}
        PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${PKGCONFIG_FOR_OPENSSL}
        --prefix=${CMAKE_CURRENT_BINARY_DIR}/libcurl_install
        --with-ssl
        --without-libgsasl
        --without-libpsl
        --without-libidn2
      BUILD_COMMAND $(MAKE)
      INSTALL_COMMAND $(MAKE) install
      TIMEOUT 600
    )
  endif()

  # The external project will install to the build folder, but we'll install that on make install.
  install(
    DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/libcurl_install/
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/opt/libcurl_vendor
  )
endmacro()

find_package(CURL QUIET)

if(FORCE_BUILD_VENDOR_PKG OR NOT CURL_FOUND)
  # System curl not found, build one locally.
  build_libcurl()

  if(WIN32)
    ament_environment_hooks(env_hook/libcurl_vendor_library_path.bat)
    set(ENV_VAR_NAME "PATH")
    set(ENV_VAR_VALUE "opt\\libcurl_vendor\\bin")
  else()
    ament_environment_hooks(env_hook/libcurl_vendor_library_path.sh)
    if(APPLE)
      set(ENV_VAR_NAME "DYLD_LIBRARY_PATH")
    else()
      set(ENV_VAR_NAME "LD_LIBRARY_PATH")
    endif()
    set(ENV_VAR_VALUE "opt/libcurl_vendor/lib")
  endif()
  ament_environment_hooks(env_hook/libcurl_vendor_library_path.dsv.in)
  set(CURL_FOUND FALSE)
endif()

ament_package(
  CONFIG_EXTRAS "libcurl_vendor-extras.cmake.in"
)
