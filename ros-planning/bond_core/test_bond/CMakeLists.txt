cmake_minimum_required(VERSION 3.5)
project(test_bond)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
add_definitions(-DNOMINMAX)

find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/TestBond.srv"
  DEPENDENCIES std_msgs builtin_interfaces
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(bond REQUIRED)
  find_package(bondcpp REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(rclcpp_lifecycle REQUIRED)
  find_package(rosidl_cmake REQUIRED)
  find_package(Threads REQUIRED)

  if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(UUID REQUIRED uuid)
    # we have to find the absolute path to uuid as target_link_directories is not available before cmake 3.13
    find_library(uuid_ABS_PATH ${UUID_LIBRARIES} PATHS ${UUID_LIBRARY_DIRS})
  elseif(WIN32)
    set(uuid_ABS_PATH Rpcrt4.lib)
  endif()

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_callbacks_cpp test/test_callbacks_cpp.cpp)
  if(TARGET test_callbacks_cpp)
    target_include_directories(test_callbacks_cpp PRIVATE include)
    target_link_libraries(test_callbacks_cpp ${uuid_ABS_PATH})
    target_link_libraries(test_callbacks_cpp
      bondcpp::bondcpp
      rclcpp::rclcpp
      rclcpp_lifecycle::rclcpp_lifecycle)

    rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
    target_link_libraries(test_callbacks_cpp "${cpp_typesupport_target}")
  endif()

  #  ament_add_gtest(test_callbacks_cpp_exc test/test_callbacks_cpp_exc.cpp)
  #  if(TARGET test_callbacks_cpp_exc)
  #    target_include_directories(test_callbacks_cpp_exc PRIVATE include)
  #    target_link_libraries(test_callbacks_cpp_exc ${UUID_LIBS})
  #    ament_target_dependencies(test_callbacks_cpp_exc bondcpp rclcpp rclcpp_lifecycle)
  #    rosidl_target_interfaces(test_callbacks_cpp_exc
  #      ${PROJECT_NAME} "rosidl_typesupport_cpp")
  #  endif()
  #
  #  ament_add_gtest(exercise_bond_cpp_exc test/exercise_bond_cpp_exc.cpp)
  #  if(TARGET exercise_bond_cpp_exc)
  #    target_include_directories(exercise_bond_cpp_exc PRIVATE include)
  #    target_link_libraries(exercise_bond_cpp_exc ${UUID_LIBS})
  #    ament_target_dependencies(exercise_bond_cpp_exc bondcpp rclcpp rclcpp_lifecycle)
  #    rosidl_target_interfaces(exercise_bond_cpp_exc
  #      ${PROJECT_NAME} "rosidl_typesupport_cpp")
  #  endif()
endif()
ament_package()
