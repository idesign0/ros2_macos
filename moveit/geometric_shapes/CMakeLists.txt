cmake_minimum_required(VERSION 3.5)
project(geometric_shapes)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Set compile options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(PROJECT_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wwrite-strings
    -Wunreachable-code
    -Wpointer-arith
    -Wredundant-decls
    -Wno-unused-parameter
  )
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  message("${PROJECT_NAME}: You did not request a specific build type: Choosing 'Release' for maximum performance")
  set(CMAKE_BUILD_TYPE Release)
endif()


# find *absolute* paths to ASSIMP_LIBRARIES
# Both, pkg-config and assimp's cmake-config don't provide an absolute library path.
# For, pkg-config the path is in ASSIMP_PC_LIBRARY_DIRS, for cmake in ASSIMP_LIBRARY_DIRS.
find_library(assimp_ABS_LIBRARIES NAMES ${assimp_LIBRARIES} assimp HINTS ${assimp_LIBRARY_DIRS} ${assimp_PC_LIBRARY_DIRS})
set(assimp_LIBRARIES "${assimp_ABS_LIBRARIES}")

# These need to be in this order to find header files
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)

find_package(ament_cmake REQUIRED)
find_package(assimp REQUIRED)
find_package(console_bridge REQUIRED)
find_package(console_bridge_vendor REQUIRED)
find_package(eigen_stl_containers REQUIRED)
find_package(geometry_msgs REQUIRED)
# Enforce the system package version on Ubuntu jammy and noble which is also used by libfcl-dev
# The version is fixed to prevent ABI conflicts with ros-octomap
find_package(octomap 1.9.7...<1.10.0 REQUIRED)
find_package(QHULL REQUIRED)
find_package(random_numbers REQUIRED)
find_package(rclcpp REQUIRED)
find_package(resource_retriever REQUIRED)
find_package(shape_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(fcl REQUIRED)

set(THIS_PACKAGE_EXPORT_DEPENDS
  eigen3_cmake_module
  Eigen3
  Boost
  console_bridge
  console_bridge_vendor
  eigen_stl_containers
  geometry_msgs
  octomap
  random_numbers
  rclcpp
  resource_retriever
  shape_msgs
  visualization_msgs
)

# Set VERSION from package.xml
ament_package_xml()

add_library(${PROJECT_NAME} SHARED
  src/aabb.cpp
  src/bodies.cpp
  src/body_operations.cpp
  src/mesh_operations.cpp
  src/obb.cpp
  src/shape_extents.cpp
  src/shape_operations.cpp
  src/shape_to_marker.cpp
  src/shapes.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT_NAME}_VERSION} WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_compile_options(${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
# Private libraries that are not transitively needed by downstream projects
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    assimp::assimp
    fcl
    ${QHULL_LIBRARIES}
  PUBLIC
    ${geometry_msgs_TARGETS}
    ${shape_msgs_TARGETS}
    ${visualization_msgs_TARGETS}
    ${console_bridge_TARGETS}
    eigen_stl_containers::eigen_stl_containers
    random_numbers::random_numbers
    rclcpp::rclcpp
    resource_retriever::resource_retriever
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${QHULL_INCLUDE_DIRS})

if(BUILD_TESTING)
  # Unit tests
  add_subdirectory(test)

  # ament_lint
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_EXPORT_DEPENDS})

ament_package()
