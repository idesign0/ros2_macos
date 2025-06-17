cmake_minimum_required(VERSION 3.16)
project(tcb_span
  LANGUAGES CXX
)

find_package(ament_cmake REQUIRED)

add_library(tcb_span INTERFACE)
target_include_directories(tcb_span INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(tcb_span INTERFACE cxx_std_17)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS tcb_span
  EXPORT export_tcb_span
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

ament_export_targets(export_tcb_span HAS_LIBRARY_TARGET)
ament_package()
