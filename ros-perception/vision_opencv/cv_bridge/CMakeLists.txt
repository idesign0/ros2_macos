cmake_minimum_required(VERSION 3.14)
project(cv_bridge)

# Dhruvs Note: for custom Boost-Python311 libs
# Set BOOST_ROOT relative to workspace root (parent of cv_bridge)
set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost-1.88.0" CACHE PATH "Custom Boost root" FORCE)
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/boost" CACHE PATH "Custom Boost includes" FORCE)
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib" CACHE PATH "Custom Boost libs" FORCE)
set(BOOST_DIR "${BOOST_ROOT}/stage/lib/cmake/Boost-1.88.0" CACHE PATH "Boost CMake config directory")

# Avoid searching system paths to pick up your custom Boost only
set(Boost_NO_SYSTEM_PATHS ON)

# Dhruvs Note: for custom Boost-Python311 libs

find_package(ament_cmake_ros REQUIRED)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra)
endif()

if(ANDROID)
  find_package(Boost REQUIRED)
  set(boost_python_target "")
else()
  find_package(Python3 REQUIRED COMPONENTS Development NumPy)
  find_package(Boost QUIET 
    PATHS
    ${BOOST_ROOT}
    ${BOOST_INCLUDEDIR}
    ${BOOST_LIBRARYDIR}
    ${BOOST_DIR}
    NO_DEFAULT_PATH
    )
  if(Boost_VERSION_STRING VERSION_LESS "1.67")
    # This is a bit of a hack to suppress a warning
    #   No header defined for python3; skipping header check
    # Which should only affect Boost versions < 1.67
    # Resolution for newer versions:
    #  https://gitlab.kitware.com/cmake/cmake/issues/16391
    set(_Boost_PYTHON3_HEADERS "boost/python.hpp")
    find_package(Boost REQUIRED COMPONENTS python3 
      PATHS
      ${BOOST_ROOT}
      ${BOOST_INCLUDEDIR}
      ${BOOST_LIBRARYDIR}
      ${BOOST_DIR}
      NO_DEFAULT_PATH
    )
    set(boost_python_target "Boost::python3")
  else()
    find_package(Boost REQUIRED COMPONENTS python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}
      PATHS
      ${BOOST_ROOT}
      ${BOOST_INCLUDEDIR}
      ${BOOST_LIBRARYDIR}
      ${BOOST_DIR}
      NO_DEFAULT_PATH
    )
    set(boost_python_target "Boost::python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
  endif()
endif()

find_package(rcpputils REQUIRED)
find_package(sensor_msgs REQUIRED)

find_package(OpenCV 4 QUIET
  COMPONENTS
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
  CONFIG
)
if(NOT OpenCV_FOUND)
  find_package(OpenCV 3 REQUIRED
    COMPONENTS
      opencv_core
      opencv_imgproc
      opencv_imgcodecs
    CONFIG
  )
endif()

if(NOT ANDROID)
  ament_python_install_package(${PROJECT_NAME}
    PACKAGE_DIR python/${PROJECT_NAME}
  )
endif()

add_subdirectory(src)

# cv_bridge_lib_dir is passed as APPEND_LIBRARY_DIRS for each ament_add_gtest call so
# the project library that they link against is on the library path.
# This is especially important on Windows.
# This is overwritten each loop, but which one it points to doesn't really matter.
set(cv_bridge_lib_dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

ament_export_dependencies(
  OpenCV
  sensor_msgs
)

ament_export_targets(export_${PROJECT_NAME})

# install the include folder
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
)

ament_package(
  CONFIG_EXTRAS "cmake/cv_bridge-extras.cmake.in"
)