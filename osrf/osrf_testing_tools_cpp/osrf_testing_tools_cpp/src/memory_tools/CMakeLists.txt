# Integrate backward-cpp according to:
#   https://github.com/bombela/backward-cpp/tree/v1.5#modifying-cmake_module_path

# Note, I had to use Backward_DIR since they provide a config file rather than a module...
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bombela/backward-cpp")
set(Backward_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bombela/backward-cpp")
set(FPHSA_NAME_MISMATCHED TRUE)
find_package(Backward REQUIRED)
unset(FPHSA_NAME_MISMATCHED)

add_library(memory_tools SHARED
  custom_memory_functions.cpp
  implementation_monitoring_override.cpp
  initialize.cpp
  is_working.cpp
  memory_tools_service.cpp
  monitoring.cpp
  register_hooks.cpp
  stack_trace.cpp
  testing_helpers.cpp
  verbosity.cpp
)

target_include_directories(memory_tools
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(memory_tools PRIVATE Backward::Backward)

if(CMAKE_DL_LIBS)
  target_link_libraries(memory_tools PUBLIC ${CMAKE_DL_LIBS})
endif()

target_compile_definitions(memory_tools
  PRIVATE "OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_BUILDING_DLL")

add_library(memory_tools_interpose SHARED
  memory_tools.cpp
)
target_link_libraries(memory_tools_interpose memory_tools)

option(OSRF_TESTING_TOOLS_CPP_DISABLE_MEMORY_TOOLS
  "Disable environment configuration for memory tools"
  OFF)

# Memory tools doesn't work on Windows right now.
# TODO(wjwwood): If on aarch64, so not set the preload environment variables, until fixed.
# see: https://github.com/osrf/osrf_testing_tools_cpp/issues/3
if(WIN32 OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  set(OSRF_TESTING_TOOLS_CPP_DISABLE_MEMORY_TOOLS ON)
endif()

if (NOT OSRF_TESTING_TOOLS_CPP_DISABLE_MEMORY_TOOLS)
  set(memory_tools_is_available TRUE)
  if(APPLE)
    list(APPEND memory_tools_extra_test_env
      DYLD_INSERT_LIBRARIES=$<TARGET_FILE:memory_tools_interpose>)
  elseif(UNIX)
    list(APPEND memory_tools_extra_test_env
      LD_PRELOAD=$<TARGET_FILE:memory_tools_interpose>)
  endif()
endif()

install(TARGETS memory_tools
  EXPORT memory_tools
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT memory_tools
  DESTINATION share/${PROJECT_NAME}/cmake
  NAMESPACE "${PROJECT_NAME}::"
  FILE "memory_toolsExport.cmake"
)

install(TARGETS memory_tools_interpose
  EXPORT memory_tools_interpose
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT memory_tools_interpose
  DESTINATION share/${PROJECT_NAME}/cmake
  NAMESPACE "${PROJECT_NAME}::"
  FILE "memory_tools_interposeExport.cmake"
)

set(memory_tools_extra_test_env "${memory_tools_extra_test_env}" PARENT_SCOPE)
set(memory_tools_is_available "${memory_tools_is_available}" PARENT_SCOPE)
set(memory_tools_src_dir_internal_testing_only
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>" PARENT_SCOPE)
