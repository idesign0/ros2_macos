cmake_minimum_required(VERSION 3.20)
project(srdfdom LANGUAGES CXX)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Boost REQUIRED)
find_package(console_bridge REQUIRED)
find_package(urdfdom_headers REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(urdf REQUIRED)
find_package(tinyxml2_vendor REQUIRED)

# Made available by the vendor package
find_package(TinyXML2 REQUIRED)

# Set VERSION from package.xml
ament_package_xml()

add_library(${PROJECT_NAME} SHARED
  src/model.cpp
  src/srdf_writer.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION})

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_link_libraries(${PROJECT_NAME} PUBLIC tinyxml2::tinyxml2)

# Ament dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
  console_bridge::console_bridge
  urdf::urdf
  urdfdom_headers::urdfdom_headers
)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

install(PROGRAMS
  scripts/display_srdf
  DESTINATION lib/${PROJECT_NAME}
)

set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_compile_definitions(${PROJECT_NAME} PRIVATE "SRDFDOM_BUILDING_DLL")

ament_python_install_package(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(self_parser_test "test/test.py")

  add_definitions(-DTEST_RESOURCE_LOCATION="${CMAKE_SOURCE_DIR}/test/resources")
  execute_process(COMMAND bash -c "locale -a | grep -q ^en_US"
    RESULT_VARIABLE TEST_LOCALE
    OUTPUT_QUIET ERROR_QUIET)
  if(TEST_LOCALE)
    message(STATUS "Locale nl_NL not available. Locale test will not be meaningful.")
  endif()

  foreach(LOCALE IN ITEMS C nl_NL.UTF-8)
    ament_add_gtest(test_cpp_${LOCALE} test/test_parser.cpp ENV LC_ALL=${LOCALE})
    target_link_libraries(test_cpp_${LOCALE} ${PROJECT_NAME})
  endforeach()

  # ament_lint
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)
ament_export_dependencies(console_bridge)
ament_export_dependencies(urdfdom_headers)
ament_export_dependencies(urdf)
ament_export_dependencies(tinyxml2_vendor)
ament_export_dependencies(TinyXML2)
ament_package()
