cmake_minimum_required(VERSION 4.0)
project(tinyxml_vendor)

option(FORCE_BUILD_VENDOR_PKG
  "Build TinyXML from source, even if system-installed package is available"
  OFF)

find_package(ament_cmake REQUIRED)

if(NOT FORCE_BUILD_VENDOR_PKG)
  list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
  find_package(TinyXML)
endif()


if (NOT TinyXML_FOUND)
  set(extra_cmake_args)

  if(DEFINED CMAKE_BUILD_TYPE)
    list(APPEND extra_cmake_args "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
  endif()

  include(ExternalProject)
  ExternalProject_Add(tinyxml-2.6.2
    PREFIX tinyxml-2.6.2
    URL https://downloads.sourceforge.net/project/tinyxml/tinyxml/2.6.2/tinyxml_2_6_2.tar.gz
    URL_MD5 c1b864c96804a10526540c664ade67f0
    CMAKE_ARGS
      -DBUILD_SHARED_LIBS=ON
      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/tinyxml_install
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      ${extra_cmake_args}
    INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/tinyxml_install"
    PATCH_COMMAND
    ${CMAKE_COMMAND} -E chdir <SOURCE_DIR> git apply -p1 --ignore-space-change --whitespace=nowarn ${CMAKE_CURRENT_SOURCE_DIR}/enforce-use-stl.patch
    COMMAND
    ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml_cmakelists.txt" <SOURCE_DIR>/CMakeLists.txt
    )

  # The external project will install to the build folder, but we'll install that on make install.
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tinyxml_install/ DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
  message(STATUS "Found TinyXML. Using TinyXML from system.")
endif()

install(DIRECTORY cmake DESTINATION share/${PROJECT_NAME})

ament_package(
  CONFIG_EXTRAS "tinyxml_vendor-extras.cmake"
)
