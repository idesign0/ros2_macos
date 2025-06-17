cmake_minimum_required(VERSION 3.5)

project(map_msgs)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

set(msg_files
  "msg/OccupancyGridUpdate.msg"
  "msg/PointCloud2Update.msg"
  "msg/ProjectedMapInfo.msg"
  "msg/ProjectedMap.msg"
)
set(srv_files
  "srv/GetMapROI.srv"
  "srv/GetPointMapROI.srv"
  "srv/GetPointMap.srv"
  "srv/ProjectedMapsInfo.srv"
  "srv/SaveMap.srv"
  "srv/SetMapProjections.srv"
)
rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  ${srv_files}
  DEPENDENCIES nav_msgs sensor_msgs std_msgs
  ADD_LINTER_TESTS
)

ament_export_dependencies(rosidl_default_runtime)

ament_package()
