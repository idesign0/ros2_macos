# Test logic for version filtering when selecting googletest version.
add_test(
  NAME "test_osrf_testing_tools_cpp_filter_versions_cmake"
  COMMAND
    ${CMAKE_COMMAND} -P
    "${CMAKE_SOURCE_DIR}/test/cmake/test_osrf_testing_tools_cpp_filter_versions.cmake"
)