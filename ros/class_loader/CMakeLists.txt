cmake_minimum_required(VERSION 3.5)

project(class_loader)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CLASS_LOADER_IGNORE_AMENT FALSE CACHE BOOL
  "Do not use ament when building this package.")
if(NOT CLASS_LOADER_IGNORE_AMENT)
  find_package(ament_cmake)
  find_package(ament_cmake_ros REQUIRED)
  set(explicit_library_type "")
else()
  set(explicit_library_type "SHARED")
endif()

find_package(console_bridge_vendor REQUIRED) # Provides console_bridge 0.4.0 on platforms without it.
find_package(console_bridge REQUIRED)

find_package(rcpputils REQUIRED)

if(ament_cmake_FOUND)
  ament_export_dependencies(console_bridge)
  ament_export_dependencies(rcpputils)
endif()

set(${PROJECT_NAME}_SRCS
  src/class_loader.cpp
  src/class_loader_core.cpp
  src/meta_object.cpp
  src/multi_library_class_loader.cpp
)
add_library(${PROJECT_NAME} ${explicit_library_type} ${${PROJECT_NAME}_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

if(ament_cmake_FOUND)
  target_link_libraries(${PROJECT_NAME}
    console_bridge::console_bridge
    rcpputils::rcpputils)
  ament_export_targets(${PROJECT_NAME})
else()
  target_include_directories(${PROJECT_NAME}
    PUBLIC ${console_bridge_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${console_bridge_LIBRARIES})
endif()
if(WIN32)
  # Causes the visibility macros to use dllexport rather than dllimport
  # which is appropriate when building the dll but not consuming it.
  target_compile_definitions(${PROJECT_NAME} PRIVATE "CLASS_LOADER_BUILDING_DLL")
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto QUIET)
  if(ament_lint_auto_FOUND)
    # TODO(mikaelarguedas) add copyright test once the linter accepts /** block comments
    list(APPEND AMENT_LINT_AUTO_EXCLUDE
      ament_cmake_copyright
    )
    ament_lint_auto_find_test_dependencies()

    # Files in test/fviz_case_study do have compatible copyrights
    find_package(ament_cmake_copyright)
    ament_copyright(
      TESTNAME copyright_check_tests_only
      "test/fviz_case_study"
    )
  endif()
  add_subdirectory(test)
endif()

if(ament_cmake_FOUND)
  ament_package(
    CONFIG_EXTRAS "class_loader-extras.cmake"
  )
else()
  set(TARGET_NAME ${PROJECT_NAME})
  set(PKGCONFIG_LIBS
    ${console_bridge_LIBRARIES}
  )

  # Prepare and install necessary files to support finding of the library
  # using pkg-config
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    @ONLY)

  install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION lib/pkgconfig)
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME})

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
