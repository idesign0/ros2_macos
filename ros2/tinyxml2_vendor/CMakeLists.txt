cmake_minimum_required(VERSION 4.0)

project(tinyxml2_vendor)

find_package(ament_cmake REQUIRED)
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
find_package(TinyXML2 REQUIRED)

install(DIRECTORY cmake DESTINATION share/${PROJECT_NAME})

ament_package(
  CONFIG_EXTRAS "tinyxml2_vendor-extras.cmake"	
)
