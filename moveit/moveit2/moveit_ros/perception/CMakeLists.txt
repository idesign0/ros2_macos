cmake_minimum_required(VERSION 3.22)
project(moveit_ros_perception)

# Dhruvs Note: different comiler for perception packages
# Manually tell CMake how to find OpenMP
set(OpenMP_C_FLAGS "-fopenmp")
set(OpenMP_CXX_FLAGS "-fopenmp")
set(OpenMP_C_LIB_NAMES "omp")
set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/llvm" ${CMAKE_PREFIX_PATH})
find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
  message(STATUS "OpenMP found and enabled")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  include_directories(SYSTEM "/opt/homebrew/opt/llvm/include")
  link_directories("/opt/homebrew/opt/llvm/lib")
else()
  message(FATAL_ERROR "OpenMP NOT found")
endif()
# Dhruvs Note: different comiler for perception packages

# Common cmake code applied to all moveit packages
find_package(moveit_common REQUIRED)
moveit_package()

option(WITH_OPENGL "Build the parts that depend on OpenGL" ON)

if(WITH_OPENGL)
  # Prefer newer vendor-specific OpenGL library
  if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
  endif()
  find_package(OpenGL REQUIRED)
#  find_package(GLEW REQUIRED)

  set(gl_LIBS ${gl_LIBS} ${OPENGL_LIBRARIES})
  if(APPLE)
    # Dhruvs Note: Direct Path set for GLEW and GLUT on macOS
    # Manually set GLEW and GLUT paths for Homebrew on macOS
    set(GLEW_INCLUDE_DIR "/opt/homebrew/opt/glew/include")
    set(GLEW_LIBRARY "/opt/homebrew/opt/glew/lib/libGLEW.dylib")
    set(GLUT_INCLUDE_DIR "/opt/homebrew/opt/freeglut/include")
    set(GLUT_LIBRARY "/opt/homebrew/lib/libglut.dylib")
    # find_package(FreeGLUT REQUIRED)
    set(GLEW_LIBRARIES ${GLEW_LIBRARY})
    set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
    set(GLUT_LIBRARIES ${GLUT_LIBRARY})
    set(GLUT_INCLUDE_DIRS ${GLUT_INCLUDE_DIR})

    set(SYSTEM_GL_INCLUDE_DIR ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
    set(SYSTEM_GL_LIBRARIES ${GLEW_LIBRARIES} ${GLUT_LIBRARIES})
    # Dhruvs Note: different comiler for perception packages

  else()
    find_package(GLUT REQUIRED)
    if(WIN32)
      set(SYSTEM_GL_LIBRARIES GLEW::glew GLUT::GLUT)
    else()
      set(SYSTEM_GL_LIBRARIES ${GLEW_LIBRARIES} GLUT::GLUT)
    endif()
  endif()
  set(perception_GL_INCLUDE_DIRS "mesh_filter/include" "depth_image_octomap_updater/include")
  set(SYSTEM_GL_INCLUDE_DIRS ${GLEW_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
endif()

if(APPLE)
  find_package(X11 REQUIRED)
endif()

find_package(ament_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(moveit_core REQUIRED)
find_package(rclcpp REQUIRED)
find_package(urdf REQUIRED)
find_package(message_filters REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(pluginlib REQUIRED)
find_package(image_transport REQUIRED)
find_package(object_recognition_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(moveit_ros_occupancy_map_monitor REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(OpenCV)

# Finds Boost Components
include(ConfigExtras.cmake)

set(THIS_PACKAGE_INCLUDE_DIRS
  depth_image_octomap_updater/include
  lazy_free_space_updater/include
  mesh_filter/include
  point_containment_filter/include
  pointcloud_octomap_updater/include
  semantic_world/include
  ${perception_GL_INCLUDE_DIRS}
)

set(THIS_PACKAGE_LIBRARIES
  moveit_depth_image_octomap_updater
  moveit_depth_image_octomap_updater_core
  moveit_lazy_free_space_updater
  moveit_mesh_filter
  moveit_point_containment_filter
  moveit_pointcloud_octomap_updater
  moveit_pointcloud_octomap_updater_core
  moveit_semantic_world
)

set(THIS_PACKAGE_INCLUDE_DEPENDS
  image_transport
  moveit_core
  moveit_msgs
  moveit_ros_occupancy_map_monitor
  object_recognition_msgs
  rclcpp
  sensor_msgs
  tf2_geometry_msgs
  Eigen3
)

include_directories(${THIS_PACKAGE_INCLUDE_DIRS})
include_directories(SYSTEM
  ${Boost_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${SYSTEM_GL_INCLUDE_DIR}
  ${X11_INCLUDE_DIR}
)

add_subdirectory(lazy_free_space_updater)
add_subdirectory(point_containment_filter)
add_subdirectory(pointcloud_octomap_updater)
if(WITH_OPENGL)
  add_subdirectory(mesh_filter)
  add_subdirectory(depth_image_octomap_updater)
endif()

add_subdirectory(semantic_world)

install(
  TARGETS ${THIS_PACKAGE_LIBRARIES}
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})

pluginlib_export_plugin_description_file(moveit_ros_occupancy_map_monitor "pointcloud_octomap_updater_plugin_description.xml")
pluginlib_export_plugin_description_file(moveit_ros_occupancy_map_monitor "depth_image_octomap_updater_plugin_description.xml")

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  # These don't pass yet, disable them for now
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  set(ament_cmake_uncrustify_FOUND TRUE)

  # Run all lint tests in package.xml except those listed above
  ament_lint_auto_find_test_dependencies()
endif()

ament_package(CONFIG_EXTRAS ConfigExtras.cmake)
