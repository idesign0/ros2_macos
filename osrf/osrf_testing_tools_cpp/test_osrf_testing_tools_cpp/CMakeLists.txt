cmake_minimum_required(VERSION 3.5)

project(test_osrf_testing_tools_cpp)

# Default to C++11
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
  find_package(osrf_testing_tools_cpp REQUIRED)
  osrf_testing_tools_cpp_require_googletest(VERSION_GTE 1.8)  # ensures target gtest_main exists
  get_target_property(memory_tools_available
    osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_IS_AVAILABLE)

  if(memory_tools_available)
    add_executable(test_example_memory_tools_gtest test/test_example_memory_tools.cpp)
    target_link_libraries(test_example_memory_tools_gtest
      gtest_main
      osrf_testing_tools_cpp::memory_tools
    )
    get_target_property(extra_env_vars
      osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE)
    osrf_testing_tools_cpp_add_test(test_example_memory_tools
      COMMAND "$<TARGET_FILE:test_example_memory_tools_gtest>"
      ENV ${extra_env_vars}
    )

    add_executable(test_is_not_working_gtest test/test_is_not_working.cpp)
    target_link_libraries(test_is_not_working_gtest
      gtest_main
      osrf_testing_tools_cpp::memory_tools
    )
    osrf_testing_tools_cpp_add_test(test_is_not_working_gtest
      COMMAND "$<TARGET_FILE:test_is_not_working_gtest>"
    )
  endif()
endif()
