cmake_minimum_required(VERSION 3.5)

project(rttest_examples)

find_package(rttest REQUIRED)

link_directories(${rttest_LIBRARY_DIR})
include_directories(${rttest_INCLUDE_DIRS})

add_executable(example_loop example_loop.c)
target_link_libraries(example_loop ${rttest_LIBRARIES})
