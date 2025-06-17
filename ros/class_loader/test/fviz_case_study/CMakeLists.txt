add_library(${PROJECT_NAME}_Test_Fviz EXCLUDE_FROM_ALL SHARED fviz.cpp)
if(WIN32)
  # Causes the visibility macros to use dllexport rather than dllimport
  # which is appropriate when building the dll but not consuming it.
  target_compile_definitions(${PROJECT_NAME}_Test_Fviz PRIVATE "FVIZ_BUILDING_DLL")
endif()

# This "fviz_main" is just for testing locally, for the test see "fviz_test" below.
add_executable(${PROJECT_NAME}_Test_Fviz_Main EXCLUDE_FROM_ALL fviz_main.cpp)
if(ament_cmake_FOUND)
  target_include_directories(${PROJECT_NAME}_Test_Fviz_Main
    PUBLIC "../include")
  ament_target_dependencies(${PROJECT_NAME}_Test_Fviz_Main "console_bridge")
else()
  target_include_directories(${PROJECT_NAME}_Test_Fviz_Main
    PUBLIC "../include" ${console_bridge_INCLUDE_DIRS})
endif()
target_link_libraries(${PROJECT_NAME}_Test_Fviz_Main
  ${PROJECT_NAME} ${PROJECT_NAME}_Test_Fviz)

add_library(${PROJECT_NAME}_Test_FvizDefaultPlugin
  EXCLUDE_FROM_ALL SHARED fviz_default_plugin.cpp)
target_include_directories(${PROJECT_NAME}_Test_FvizDefaultPlugin
  PUBLIC "../include" ${console_bridge_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_Test_FvizDefaultPlugin
  ${PROJECT_NAME} ${PROJECT_NAME}_Test_Fviz)
class_loader_hide_library_symbols(${PROJECT_NAME}_Test_FvizDefaultPlugin)

if(WIN32)
  set(append_library_dirs "$<TARGET_FILE_DIR:${PROJECT_NAME}>;$<TARGET_FILE_DIR:${PROJECT_NAME}_TestPlugins1>")
else()
  set(append_library_dirs "${CMAKE_CURRENT_BINARY_DIR}")
endif()

ament_add_gtest(${PROJECT_NAME}_fviz_test fviz_test.cpp
  APPEND_LIBRARY_DIRS "${append_library_dirs}"
)
if(TARGET ${PROJECT_NAME}_fviz_test)
  if(ament_cmake_FOUND)
    target_include_directories(${PROJECT_NAME}_fviz_test
      PUBLIC "../include")
    ament_target_dependencies(${PROJECT_NAME}_fviz_test "console_bridge")
  else()
    target_include_directories(${PROJECT_NAME}_fviz_test
      PUBLIC "../include" ${console_bridge_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_fviz_test)
  endif()
  target_link_libraries(${PROJECT_NAME}_fviz_test
    ${PROJECT_NAME} ${PROJECT_NAME}_Test_Fviz)
  if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME}_fviz_test pthread)
  endif()
  add_dependencies(${PROJECT_NAME}_fviz_test
    ${PROJECT_NAME}_Test_FvizDefaultPlugin ${PROJECT_NAME}_Test_Fviz_Main)
endif()
