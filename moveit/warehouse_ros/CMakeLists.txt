cmake_minimum_required(VERSION 3.16.3)
project(warehouse_ros)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(warehouse_ros SHARED
  src/database_loader.cpp
  src/transform_collection.cpp
)
target_link_libraries(warehouse_ros PUBLIC OpenSSL::Crypto)
set_target_properties(warehouse_ros PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_link_libraries(warehouse_ros PUBLIC
  rclcpp::rclcpp
  ${std_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
  pluginlib::pluginlib
  tf2::tf2
  tf2_ros::tf2_ros
  ${tf2_geometry_msgs_TARGETS}
  ${Boost_LIBRARIES}
)
target_include_directories(warehouse_ros
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

add_executable(warehouse_test_dbloader src/test_dbloader.cpp)
target_link_libraries(warehouse_test_dbloader PUBLIC warehouse_ros)

install(
  TARGETS warehouse_ros
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  TARGETS warehouse_test_dbloader
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(
  rclcpp
  std_msgs
  geometry_msgs
  pluginlib
  tf2
  tf2_ros
  tf2_geometry_msgs
  Boost
  OpenSSL)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
