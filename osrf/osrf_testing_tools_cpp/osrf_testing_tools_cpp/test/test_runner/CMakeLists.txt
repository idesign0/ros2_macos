# Test parsing of environment variables
add_executable(test_parse_environment_variable test_parse_environment_variable.cpp)
# gtest_main is found by osrf_testing_tools_cpp_require_googletest(), called in main CMakeLists.txt
target_link_libraries(test_parse_environment_variable gtest_main)
add_test(
  NAME "test_parse_environment_variable"
  COMMAND "$<TARGET_FILE:test_parse_environment_variable>"
)

# Test the test_runner's ability to influence environment variables in tests
add_executable(assert_env_vars assert_env_vars.cpp)
# gtest_main is found by osrf_testing_tools_cpp_require_googletest(), called in main CMakeLists.txt
target_link_libraries(assert_env_vars gtest_main)

# The use of `:` in path-like env var's is converted to `;` by `assert_env_vars`.
add_test(
  NAME "test_test_runner"
  COMMAND
    "$<TARGET_FILE:test_runner>"
    --env
      FOO=bar
      PING=pong
    --append-env
      FOO=baz
    --
    "$<TARGET_FILE:assert_env_vars>"
    --env
      FOO=bar:baz
      PING=pong
)

# # This test can be uncommented to make sure a bad return code is propogated by test_runner.
# add_test(
#   NAME "test_test_runner_fails"
#   COMMAND
#     "$<TARGET_FILE:test_runner>"
#     --env
#       FOO=bar
#       PING=pong
#     --append-env
#       FOO=baz
#     --
#     python3 -c "import sys; print('exiting with code 1'); sys.exit(1)"
# )
