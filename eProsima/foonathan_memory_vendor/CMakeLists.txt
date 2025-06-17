cmake_minimum_required(VERSION 4.0)
project(foonathan_memory_vendor VERSION "1.3.2")

option(FOONATHAN_MEMORY_FORCE_VENDORED_BUILD
        "Force foonathan memory vendor package build even if it is already installed in the system" OFF)

if(NOT FOONATHAN_MEMORY_FORCE_VENDORED_BUILD)
  execute_process(COMMAND
      ${CMAKE_COMMAND}
      --find-package
      -DNAME=foonathan_memory
      -DCOMPILER_ID=$<IF:$<BOOL:${WIN32}>,MSVC,GNU>
      -DLANGUAGE=CXX
      -DMODE=EXIST
      -DCMAKE_FIND_DEBUG_MODE=ON
    ERROR_QUIET
    RESULT_VARIABLE _EXIT_CODE)
endif()

if(NOT _EXIT_CODE EQUAL 0)
  ###############################################################################
  # Default shared libraries
  ###############################################################################
  # Global flag to cause add_library() to create shared libraries if on.
  # If set to true, this will cause all libraries to be built shared
  # unless the library was explicitly added as a static library.
  option(BUILD_SHARED_LIBS "Create shared libraries by default" OFF)

  # Option to build memory examples
  option(BUILD_MEMORY_EXAMPLES "Build memory examples" OFF)
  # Option to build memory tests
  option(BUILD_MEMORY_TESTS "Build memory tests" OFF)
  # Option to build memory tools
  option(BUILD_MEMORY_TOOLS "Build memory tools" ON)
  # Option for position independence
  option(CMAKE_POSITION_INDEPENDENT_CODE "Enable position independence" ON)

  # Validate option dependency
  if((NOT BUILD_MEMORY_TOOLS) AND (BUILD_MEMORY_EXAMPLES OR BUILD_MEMORY_TESTS))
    message(FATAL_ERROR
      "BUILD_MEMORY_TOOLS is required when BUILD_MEMORY_EXAMPLES"
      " or BUILD_MEMORY_TESTS are set")
  endif()

  # Propagate BUILD_SHARED_LIBS
  list(APPEND extra_cmake_args -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})

  # Propagate CMAKE_POSITION_INDEPENDENT_CODE
  list(APPEND extra_cmake_args -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE})

  if(DEFINED CMAKE_BUILD_TYPE)
    list(APPEND extra_cmake_args -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
  endif()

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
        list(APPEND extra_cmake_args
          "-DANDROID_FUNCTION_LEVEL_LINKING=${ANDROID_FUNCTION_LEVEL_LINKING}")
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
    if(QNX)
      list(APPEND extra_cmake_args "-DCMAKE_SYSTEM_PROCESSOR=${CPUVARDIR}")
    endif()
  endif()
  include(ExternalProject)

  if(INSTALLER_PLATFORM)
    set(PATCH_COMMAND_STR PATCH_COMMAND
      git apply ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.patch &&
      sed -i -e "s/INSTALLER_PLATFORM/${INSTALLER_PLATFORM}/g" CMakeLists.txt)
  endif()

  externalproject_add(foo_mem-ext
  GIT_REPOSITORY https://github.com/foonathan/memory.git
  GIT_TAG v0.7-3
  TIMEOUT 600
  # Avoid the update (git pull) and so the recompilation of foonathan_memory library each time.
  UPDATE_COMMAND ""
  CMAKE_ARGS
    -DFOONATHAN_MEMORY_BUILD_EXAMPLES=${BUILD_MEMORY_EXAMPLES}
    -DFOONATHAN_MEMORY_BUILD_TESTS=${BUILD_MEMORY_TESTS}
    -DFOONATHAN_MEMORY_BUILD_TOOLS=${BUILD_MEMORY_TOOLS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/foo_mem_ext_prj_install
    ${extra_cmake_args}
    -Wno-dev
    ${PATCH_COMMAND_STR}
  )

  # The external project will install to the build folder, but we'll install that on make install.
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foo_mem_ext_prj_install/
  DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
  message(STATUS "Found foonathan_memory")
endif()

configure_file(foonathan_memory_vendorConfig.cmake.in
  "${PROJECT_BINARY_DIR}/foonathan_memory_vendorConfig.cmake" @ONLY)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/foonathan_memory_vendorConfig-version.cmake"
  COMPATIBILITY AnyNewerVersion)

find_package(ament_cmake_test QUIET)
if(BUILD_TESTING)
  find_package(ament_cmake_copyright QUIET)
  find_package(ament_cmake_lint_cmake QUIET)
  find_package(ament_cmake_xmllint QUIET)
  if(ament_cmake_copyright_FOUND)
    ament_copyright()
  endif()
  if(ament_cmake_lint_cmake_FOUND)
    ament_lint_cmake()
  endif()
  if(ament_cmake_xmllint_FOUND)
    ament_xmllint()
  endif()
endif()

install(FILES
  package.xml
  DESTINATION share/${PROJECT_NAME})

install(FILES
  "${PROJECT_BINARY_DIR}/foonathan_memory_vendorConfig.cmake"
  "${PROJECT_BINARY_DIR}/foonathan_memory_vendorConfig-version.cmake"
  DESTINATION share/${PROJECT_NAME}/cmake)
