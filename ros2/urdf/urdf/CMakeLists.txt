cmake_minimum_required(VERSION 3.5)
project(urdf)

find_package(ament_cmake_ros REQUIRED)
find_package(pluginlib REQUIRED)
find_package(urdf_parser_plugin REQUIRED)
find_package(urdfdom REQUIRED)
find_package(urdfdom_headers REQUIRED)
find_package(tinyxml2_vendor REQUIRED)
find_package(TinyXML2 REQUIRED)

# Find version components
if(NOT urdfdom_headers_VERSION)
  set(urdfdom_headers_VERSION "0.0.0")
endif()
string(REGEX REPLACE "^([0-9]+).*" "\\1" URDFDOM_HEADERS_MAJOR_VERSION "${urdfdom_headers_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" URDFDOM_HEADERS_MINOR_VERSION "${urdfdom_headers_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" URDFDOM_HEADERS_REVISION_VERSION "${urdfdom_headers_VERSION}")
set(generated_install_path "${CMAKE_CURRENT_BINARY_DIR}/include")
set(generated_compat_header "${generated_install_path}/${PROJECT_NAME}/urdfdom_compatibility.h")
include_directories(${generated_install_path})
configure_file(urdfdom_compatibility.h.in "${generated_compat_header}" @ONLY)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

install(FILES ${generated_compat_header} DESTINATION include/${PROJECT_NAME}/${PROJECT_NAME})

add_library(${PROJECT_NAME} src/model.cpp)
target_include_directories(${PROJECT_NAME}
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)
ament_target_dependencies(${PROJECT_NAME}
  urdf_parser_plugin
  urdfdom
  urdfdom_headers
  pluginlib
)
target_link_libraries(${PROJECT_NAME}
  urdfdom::urdfdom_model
)

if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE "URDF_BUILDING_DLL")
endif()

if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

add_library(urdf_xml_parser SHARED
  src/urdf_plugin.cpp
)
target_link_libraries(urdf_xml_parser
  ${PROJECT_NAME}
)
ament_target_dependencies(urdf_xml_parser
  "pluginlib"
  "TinyXML2"
  "urdf_parser_plugin"
)

install(TARGETS urdf_xml_parser
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_cppcheck REQUIRED)
  find_package(ament_cmake_cpplint REQUIRED)
  find_package(ament_cmake_google_benchmark REQUIRED)
  find_package(ament_cmake_lint_cmake REQUIRED)
  find_package(ament_cmake_uncrustify REQUIRED)
  # This forces cppcheck to consider all files in this project to be C++,
  # including the headers which end with .h, which cppcheck would normally
  # consider to be C instead.
  ament_cppcheck(LANGUAGE "c++")
  ament_cpplint()
  ament_lint_cmake()
  ament_uncrustify(LANGUAGE "C++")

  pluginlib_enable_plugin_testing(
    CMAKE_TARGET_VAR mock_install_target
    AMENT_PREFIX_PATH_VAR mock_install_path
    PLUGIN_CATEGORY "urdf_parser"
    PLUGIN_DESCRIPTIONS "urdf_parser_description.xml"
    PLUGIN_LIBRARIES urdf_xml_parser
  )

  ament_add_google_benchmark(plugin_overhead
    test/benchmark_plugin_overhead.cpp
    APPEND_ENV AMENT_PREFIX_PATH="${mock_install_path}"
  )
  target_link_libraries(plugin_overhead
    urdf
  )
  add_dependencies(plugin_overhead "${mock_install_target}")
endif()

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies(pluginlib)
ament_export_dependencies(urdf_parser_plugin)
ament_export_dependencies(urdfdom)
ament_export_dependencies(urdfdom_headers)

pluginlib_export_plugin_description_file(urdf_parser_plugin "urdf_parser_description.xml")

ament_package()
