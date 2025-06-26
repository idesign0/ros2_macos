cmake_minimum_required(VERSION 3.12)
project(py_binding_tools)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11_vendor REQUIRED)
find_package(pybind11 REQUIRED)

add_library(${PROJECT_NAME} SHARED
  src/ros_msg_typecasters.cpp
  src/initializer.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_link_libraries(${PROJECT_NAME} PUBLIC rclcpp::rclcpp ${geometry_msgs_TARGETS} pybind11::module)

ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

_ament_cmake_python_register_environment_hook()

pybind11_add_module(rclcpp src/rclcpp.cpp)
target_link_libraries(rclcpp PRIVATE ${PROJECT_NAME})
install(TARGETS rclcpp LIBRARY DESTINATION "${PYTHON_INSTALL_DIR}")

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(std_msgs REQUIRED)

  pybind11_add_module(${PROJECT_NAME}_test test/test.cpp)
  target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} ${std_msgs_TARGETS})
  install(TARGETS ${PROJECT_NAME}_test LIBRARY DESTINATION "${PYTHON_INSTALL_DIR}")

  ament_add_pytest_test(basic test/test_basic.py)
endif()

ament_export_dependencies(rclcpp)
ament_export_dependencies(geometry_msgs)
ament_package()
