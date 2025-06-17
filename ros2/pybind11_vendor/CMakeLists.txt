cmake_minimum_required(VERSION 4.0)

project(pybind11_vendor)

find_package(ament_cmake REQUIRED)

option(FORCE_BUILD_VENDOR_PKG
  "Build pybind11 from source, even if system-installed package is available"
  OFF)

if(NOT FORCE_BUILD_VENDOR_PKG)
  find_package(pybind11 QUIET)
endif()

macro(build_pybind11)
  set(extra_cmake_args)

  # CMAKE_BUILD_TYPE was removed in ros2:pybind11_vendor#3 to eliminate warning
  # on Windows about unused variable. It is only used for tests, which we are
  # not building, and in pybind11Tools.cmake for Linux and Mac. In any case,
  # pybind11 is a headers-only class and does not require building at the time
  # of this vendor package, thus does not require passing in the variable here.
  # It will use the variable value in packages that depend on it for compiling.

  if(DEFINED CMAKE_TOOLCHAIN_FILE)
    list(APPEND extra_cmake_args "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    if(ANDROID)
      if(DEFINED ANDROID_ABI)
        list(APPEND extra_cmake_args "-DANDROID_ABI=${ANDROID_ABI}")
      endif()
      if(DEFINED ANDROID_CPP_FEATURES)
        list(APPEND extra_cmake_args "-DANDROID_CPP_FEATURES=${ANDROID_CPP_FEATURES}")
      endif()
      if(DEFINED ANDROID_FUNCTION_LEVEL_LINKING)
        list(APPEND extra_cmake_args "-DANDROID_FUNCTION_LEVEL_LINKING=${ANDROID_FUNCTION_LEVEL_LINKING}")
      endif()
      if(DEFINED ANDROID_NATIVE_API_LEVEL)
        list(APPEND extra_cmake_args "-DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
      endif()
      if(DEFINED ANDROID_NDK)
        list(APPEND extra_cmake_args "-DANDROID_NDK=${ANDROID_NDK}")
      endif()
      if(DEFINED ANDROID_STL)
        list(APPEND extra_cmake_args "-DANDROID_STL=${ANDROID_STL}")
      endif()
      if(DEFINED ANDROID_TOOLCHAIN_NAME)
        list(APPEND extra_cmake_args "-DANDROID_TOOLCHAIN_NAME=${ANDROID_TOOLCHAIN_NAME}")
      endif()
    endif()
  else()
    list(APPEND extra_cmake_args "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
  endif()
  list(APPEND extra_cmake_args "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

  # include(ExternalProject)
  # ExternalProject_Add(pybind11-2.9.1
  #   GIT_REPOSITORY https://github.com/pybind/pybind11.git
  #   GIT_TAG ffa346860b306c9bbfb341aed9c14c067751feb8  # v2.9.1
  #   GIT_CONFIG advice.detachedHead=false
  #   # Suppress git update due to https://gitlab.kitware.com/cmake/cmake/-/issues/16419
  #   # See https://github.com/ament/uncrustify_vendor/pull/22 for details
  #   UPDATE_COMMAND ""
  #   TIMEOUT 300
  #   CMAKE_ARGS
  #     -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_install
  #     -DPYBIND11_TEST=OFF
  #     -DCMAKE_INSTALL_INCLUDEDIR=include/${PROJECT_NAME}
  #     ${extra_cmake_args}
  #   # By default, pybind11 sets Py_DEBUG if the interpreter is a debug one (as
  #   # it is in Windows Debug).  Unfortunately, this conflicts with Py_DEBUG as
  #   # used by the CPython headers, and ultimately ends up causing multiple
  #   # definitions for Py_DEBUG, which MSVC complains about.  This patch switches
  #   # the internal pybind11 variable to be called PYBIND11_DEBUG, which avoids
  #   # the issue.
  #   PATCH_COMMAND
  #     ${CMAKE_COMMAND} -E chdir <SOURCE_DIR> git apply -p1 --ignore-space-change --whitespace=nowarn
  #       ${CMAKE_CURRENT_SOURCE_DIR}/pybind11-2.9.1-fix-windows-debug.patch
  # )

  find_package(pybind11 REQUIRED)
  # The external project will install to the build folder, but we'll install that on make install.
  install(
    DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_install/
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}
    USE_SOURCE_PERMISSIONS
  )
endmacro()

if(NOT pybind11_FOUND)
  build_pybind11()
endif()

ament_package()
