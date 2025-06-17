cmake_minimum_required(VERSION 3.5)

project(test_cli)

find_package(ament_cmake_auto REQUIRED)

if(BUILD_TESTING)
  # Provides PYTHON_EXECUTABLE_DEBUG
  find_package(python_cmake_module REQUIRED)
  find_package(PythonExtra REQUIRED)
  set(_PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE}")
  if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(_PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE_DEBUG}")
    endif()
  endif()

  # Default to C++14
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
  endif()
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()

  find_package(ament_cmake REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(rclcpp REQUIRED)

  ament_lint_auto_find_test_dependencies()

  add_executable(initial_params_rclcpp
    test/initial_params.cpp)
  ament_target_dependencies(initial_params_rclcpp
    "rclcpp")

  ament_add_pytest_test(test_params_yaml
    test/test_params_yaml.py
    PYTHON_EXECUTABLE "${_PYTHON_EXECUTABLE}"
    ENV
      INITIAL_PARAMS_RCLCPP=$<TARGET_FILE:initial_params_rclcpp>
      INITIAL_PARAMS_RCLPY=${CMAKE_CURRENT_LIST_DIR}/test/initial_params.py
    # https://github.com/ros2/system_tests/issues/365
    SKIP_TEST
  )
  if(TEST test_params_yaml)
    set_tests_properties(test_params_yaml
      PROPERTIES DEPENDS
        initial_params_rclcpp)
  endif()
endif()

ament_auto_package()
