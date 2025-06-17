cmake_minimum_required(VERSION 3.5)
project(libstatistics_collector)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Enable strict compiler flags if possible.
include(CheckCXXCompilerFlag)
set(FLAGS -pedantic -Wno-long-long -Wall -Wextra -Wcast-qual -Wformat -Wwrite-strings -Wcast-align
  -Wno-error=cast-align -Wmissing-declarations -Wconversion)
foreach(FLAG ${FLAGS})
  check_cxx_compiler_flag(${FLAG} R${FLAG})
  if(${R${FLAG}})
    set(WARNING_CXX_FLAGS "${WARNING_CXX_FLAGS} ${FLAG}")
  endif()
endforeach()

if(WIN32)
  add_definitions(-DNOMINMAX)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rcl REQUIRED)
find_package(rcpputils REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(statistics_msgs REQUIRED)

add_library(${PROJECT_NAME}
  src/libstatistics_collector/collector/collector.cpp
  src/libstatistics_collector/collector/generate_statistics_message.cpp
  src/libstatistics_collector/moving_average_statistics/moving_average.cpp
  src/libstatistics_collector/moving_average_statistics/types.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE "LIBSTATISTICS_COLLECTOR_BUILDING_LIBRARY")

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

ament_target_dependencies(${PROJECT_NAME}
  "builtin_interfaces"
  "rcl"
  "rcpputils"
  "statistics_msgs"
)

install(
  TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies("builtin_interfaces" "rcl" "rcpputils" "rosidl_default_runtime" "statistics_msgs")

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(performance_test_fixture REQUIRED)
  find_package(std_msgs REQUIRED)

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_collector
    test/collector/test_collector.cpp)
  target_link_libraries(test_collector ${PROJECT_NAME})
  ament_target_dependencies(test_collector "rcpputils")

  ament_add_gtest(test_moving_average_statistics
    test/moving_average_statistics/test_moving_average_statistics.cpp)
  target_link_libraries(test_moving_average_statistics ${PROJECT_NAME})
  ament_target_dependencies(test_moving_average_statistics "rcpputils")

  ament_add_gtest(test_received_message_period
    test/topic_statistics_collector/test_received_message_period.cpp)
  target_link_libraries(test_received_message_period ${PROJECT_NAME})
  ament_target_dependencies(test_received_message_period "rcl" "rcpputils")

  ament_add_gtest(test_received_message_age
    test/topic_statistics_collector/test_received_message_age.cpp)
  target_link_libraries(test_received_message_age ${PROJECT_NAME})
  ament_target_dependencies(test_received_message_age "rcl" "rcpputils")

  rosidl_generate_interfaces(libstatistics_collector_test_msgs
    "test/msg/DummyMessage.msg"
    "test/msg/DummyCustomHeaderMessage.msg"
    DEPENDENCIES "std_msgs"
    SKIP_INSTALL)

  # To enable use of dummy_message.hpp in test_received_message_age
  rosidl_get_typesupport_target(cpp_typesupport_target libstatistics_collector_test_msgs "rosidl_typesupport_cpp")
  target_link_libraries(test_received_message_age "${cpp_typesupport_target}")

  add_performance_test(benchmark_iterative test/benchmark/benchmark_iterative.cpp)
  if(TARGET benchmark_iterative)
    target_link_libraries(benchmark_iterative ${PROJECT_NAME})
  endif()
endif()

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

ament_package()
