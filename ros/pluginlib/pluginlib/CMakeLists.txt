cmake_minimum_required(VERSION 3.5)
project(pluginlib)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(class_loader REQUIRED)
find_package(rcutils REQUIRED)
find_package(rcpputils REQUIRED)
find_package(tinyxml2_vendor REQUIRED)
find_package(TinyXML2 REQUIRED)  # provided by tinyxml2 upstream, or tinyxml2_vendor

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
target_link_libraries(${PROJECT_NAME} INTERFACE
  ament_index_cpp::ament_index_cpp
  class_loader::class_loader
  rcutils::rcutils
  rcpputils::rcpputils
  tinyxml2::tinyxml2)

ament_export_dependencies(ament_index_cpp class_loader rcutils rcpputils tinyxml2_vendor TinyXML2)

ament_export_targets(export_${PROJECT_NAME})

install(
  TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME}
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  include_directories(include test/include)

  add_library(test_plugins SHARED ./test/test_plugins.cpp)
  target_compile_definitions(test_plugins PRIVATE "TEST_PLUGINLIB_FIXTURE_BUILDING_LIBRARY")
  target_link_libraries(test_plugins ${PROJECT_NAME})

  include("cmake/pluginlib_enable_plugin_testing.cmake")
  pluginlib_enable_plugin_testing(
    CMAKE_TARGET_VAR mock_install_target
    AMENT_PREFIX_PATH_VAR mock_install_path
    PACKAGE_NAME "test_pluginlib"
    PACKAGE_XML "test/test_package.xml"
    PLUGIN_CATEGORY "test_pluginlib"
    PLUGIN_DESCRIPTIONS "test/test_plugins.xml"
    PLUGIN_LIBRARIES test_plugins
  )

  ament_add_gtest(${PROJECT_NAME}_unique_ptr_test
    test/unique_ptr_test.cpp
    APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:test_plugins>"
    APPEND_ENV AMENT_PREFIX_PATH=${mock_install_path}
  )
  if(TARGET ${PROJECT_NAME}_unique_ptr_test)
    target_link_libraries(${PROJECT_NAME}_unique_ptr_test ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME}_unique_ptr_test "${mock_install_target}")
  endif()

  ament_add_gtest(${PROJECT_NAME}_utest
    test/utest.cpp
    APPEND_ENV AMENT_PREFIX_PATH=${mock_install_path}
  )
  if(TARGET ${PROJECT_NAME}_utest)
    # Used, but not linked to test ${PROJECT_NAME}'s exports:
    #   rcutils::rcutils
    target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME}_utest "${mock_install_target}")
  endif()

endif()

ament_package(
  CONFIG_EXTRAS "pluginlib-extras.cmake"
)
