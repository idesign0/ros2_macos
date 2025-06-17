cmake_minimum_required(VERSION 3.8)
project(ros2_control_cmake)

# find dependencies
find_package(ament_cmake REQUIRED)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package(
  CONFIG_EXTRAS "ros2_control_cmake-extras.cmake"
)
