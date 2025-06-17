cmake_minimum_required(VERSION 3.16)
project(tl_expected
  HOMEPAGE_URL https://tl.tartanllama.xyz
  DESCRIPTION "C++11/14/17 std::expected with functional-style extensions"
  LANGUAGES CXX
)

find_package(ament_cmake REQUIRED)

add_library(tl_expected INTERFACE)
target_include_directories(tl_expected INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(tl_expected INTERFACE cxx_std_17)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS tl_expected
  EXPORT export_tl_expected
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

ament_export_targets(export_tl_expected HAS_LIBRARY_TARGET)
ament_package()
