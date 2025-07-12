cmake_minimum_required(VERSION 3.5)
project(bondcpp)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(bond REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(smclib REQUIRED)
find_package(Threads REQUIRED)

if(UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(UUID REQUIRED uuid)
  # we have to find the absolute path to uuid as target_link_directories is not available before cmake 3.13
  find_library(uuid_ABS_PATH ${UUID_LIBRARIES} PATHS ${UUID_LIBRARY_DIRS})
elseif(WIN32)
  set(uuid_ABS_PATH Rpcrt4.lib)
endif()

add_library(${PROJECT_NAME}
  SHARED
  src/bond.cpp
  src/BondSM_sm.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  ${UUID_INCLUDE_DIRS})
if(WIN32)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE "BONDCPP_BUILDING_LIBRARY" "NOMINMAX")
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${bond_TARGETS}
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  smclib::smclib_header
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${uuid_ABS_PATH})

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME})

ament_export_dependencies(bond rclcpp rclcpp_lifecycle smclib)
ament_export_include_directories(include/${PROJECT_NAME})
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()
ament_package()
