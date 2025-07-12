cmake_minimum_required(VERSION 3.5)
project(geodesy)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(angles REQUIRED)
find_package(geographic_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(unique_identifier_msgs REQUIRED)

include_directories(
  include
)

set(dependencies
  angles
  geographic_msgs
  geometry_msgs
  sensor_msgs
  unique_identifier_msgs
)

ament_python_install_package(${PROJECT_NAME} PACKAGE_DIR src/${PROJECT_NAME})

# build C++ coordinate conversion library
add_library(geoconv STATIC src/conv/utm_conversions.cpp)
target_link_libraries(geoconv PUBLIC
  ${geographic_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
  ${unique_identifier_msgs_TARGETS}
  angles::angles
  geographic_msgs::validation
  sensor_msgs::sensor_msgs_library
)

install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION include/${PROJECT_NAME}/)
install(TARGETS geoconv
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_export_dependencies(${dependencies})
ament_export_include_directories(include)
ament_export_libraries(geoconv)
ament_package()
