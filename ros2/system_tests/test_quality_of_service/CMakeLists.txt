cmake_minimum_required(VERSION 3.5)

project(test_quality_of_service)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

find_package(ament_cmake REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(rcutils)
  find_package(rmw REQUIRED)
  find_package(std_msgs REQUIRED)

  # get the rmw implementations ahead of time
  find_package(rmw_implementation_cmake REQUIRED)
  get_available_rmw_implementations(rmw_implementations)
  foreach(rmw_implementation ${rmw_implementations})
    find_package("${rmw_implementation}" REQUIRED)
  endforeach()

  include_directories(include)

  add_library(${PROJECT_NAME}_support
    test/qos_test_node.cpp
    test/qos_test_publisher.cpp
    test/qos_test_subscriber.cpp
    test/qos_utilities.cpp
  )

  ament_find_gtest()
  target_include_directories(${PROJECT_NAME}_support PUBLIC "${GTEST_INCLUDE_DIRS}")
  target_link_libraries(${PROJECT_NAME}_support ${GTEST_LIBRARIES})
  ament_target_dependencies(${PROJECT_NAME}_support rclcpp rcutils std_msgs)

  function(add_custom_gtest target)
    ament_add_gtest(${target}${target_suffix} ${ARGN}
      ENV
        RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
        RMW_IMPLEMENTATION=${rmw_implementation}
    )
    target_compile_definitions(${target}${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    target_link_libraries(${target}${target_suffix} ${PROJECT_NAME}_support)
    ament_target_dependencies(${target}${target_suffix} rclcpp rcutils std_msgs)
  endfunction()

  macro(tests)
    add_custom_gtest(test_deadline test/test_deadline.cpp)
    add_custom_gtest(test_lifespan test/test_lifespan.cpp)
    add_custom_gtest(test_liveliness test/test_liveliness.cpp)
  endmacro()

  call_for_each_rmw_implementation(tests)
endif()

ament_package()
