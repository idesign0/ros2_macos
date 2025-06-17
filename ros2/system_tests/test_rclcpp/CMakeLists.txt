cmake_minimum_required(VERSION 3.5)

project(test_rclcpp)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

if(BUILD_TESTING)
  find_package(rclcpp REQUIRED)
  set(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS ${rclcpp_INCLUDE_DIRS})

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(launch_testing_ament_cmake REQUIRED)

  set(message_files
    "msg/UInt32.msg"
  )

  set(service_files
    "srv/AddTwoInts.srv"
  )

  rosidl_generate_interfaces(${PROJECT_NAME}
    ${message_files}
    ${service_files}
    SKIP_INSTALL
  )

  # get the rmw implementations ahead of time
  find_package(rmw_implementation_cmake REQUIRED)
  get_available_rmw_implementations(rmw_implementations)
  foreach(rmw_implementation ${rmw_implementations})
    find_package("${rmw_implementation}" REQUIRED)
  endforeach()

  macro(custom_gtest target)
    ament_add_gtest(${target}${target_suffix} ${ARGN}
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation})
    if(TARGET ${target}${target_suffix})
      target_compile_definitions(${target}${target_suffix}
        PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
      rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
      target_link_libraries(${target}${target_suffix} "${cpp_typesupport_target}")
      ament_target_dependencies(${target}${target_suffix}
        "rclcpp")
      target_include_directories(${target}${target_suffix} PUBLIC include)
    endif()
  endmacro()

  function(custom_executable target)
    add_executable(${target} ${ARGN})
    target_compile_definitions(${target}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
    target_link_libraries(${target} "${cpp_typesupport_target}")
    ament_target_dependencies(${target}
      "rclcpp")
  endfunction()

  function(custom_gtest_executable target)
    ament_add_gtest_executable(${target} ${ARGN})
    target_compile_definitions(${target}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
    target_link_libraries(${target} "${cpp_typesupport_target}")
    ament_target_dependencies(${target}
      "rclcpp")
  endfunction()

  macro(custom_launch_test_two_executables test_name executable1 executable2)
    cmake_parse_arguments(_ARG "" "ARGS1;ARGS2;RMW1;RMW2" "" ${ARGN})
    set(TEST_NAME "${test_name}")
    set(TEST_EXECUTABLE1 "$<TARGET_FILE:${executable1}>")
    set(TEST_EXECUTABLE1_ARGS "${_ARG_ARGS1}")
    set(TEST_EXECUTABLE1_NAME "${executable1}")
    set(TEST_RMW_IMPLEMENTATION1 "${_ARG_RMW1}")
    set(TEST_EXECUTABLE2 "$<TARGET_FILE:${executable2}>")
    set(TEST_EXECUTABLE2_ARGS "${_ARG_ARGS2}")
    set(TEST_EXECUTABLE2_NAME "${executable2}")
    set(TEST_RMW_IMPLEMENTATION2 "${_ARG_RMW2}")
    configure_file(
      test/test_two_executables.py.in
      ${test_name}${target_suffix}.py.configure
      @ONLY
    )
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}_$<CONFIG>.py"
      INPUT "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}.py.configure"
    )
    add_launch_test(
      "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}_$<CONFIG>.py"
      TARGET "${test_name}${target_suffix}"
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      ${_ARG_UNPARSED_ARGUMENTS}
    )
    if(TEST ${test_name}${target_suffix})
      set_tests_properties(${test_name}${target_suffix}
        PROPERTIES DEPENDS "${executable1}${target_suffix} ${executable2}${target_suffix}"
      )
    endif()
  endmacro()

  macro(custom_launch_n_nodes num_nodes)
    set(TEST_EXECUTABLE1 "$<TARGET_FILE:node_with_name>")
    set(TEST_EXECUTABLE2 "$<TARGET_FILE:node_check_names>")
    set(TEST_RMW_IMPLEMENTATION "${rmw_implementation}")
    set(TEST_NUM_NODES "${num_nodes}")
    configure_file(
      test/test_n_nodes.py.in
      test_n_nodes${target_suffix}.py.configure
      @ONLY
    )
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_n_nodes${target_suffix}_$<CONFIG>.py"
      INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_n_nodes${target_suffix}.py.configure"
    )
    add_launch_test(
      "${CMAKE_CURRENT_BINARY_DIR}/test_n_nodes${target_suffix}_$<CONFIG>.py"
      TARGET test_n_nodes${target_suffix}
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      ${_ARG_UNPARSED_ARGUMENTS}
    )
    if(TEST test_n_nodes${target_suffix})
      set_tests_properties(test_n_nodes${target_suffix}
        PROPERTIES DEPENDS "node_with_name${target_suffix} node_check_names${target_suffix}"
      )
    endif()
  endmacro()

  # Macro for tests that trigger the shutdown of an executable based on particular console output,
  # then check the output of the executable against different console output.
  macro(custom_launch_test_executable_output test_name executable)

    cmake_parse_arguments(_ARG "" "LAUNCH_TEST_TEMPLATE" "" ${ARGN})
    if(NOT _ARG_LAUNCH_TEST_TEMPLATE)
      set(_ARG_LAUNCH_TEST_TEMPLATE test/test_executable_output.py.in)
    endif()

    set(TEST_NAME "${test_name}")
    set(TEST_EXECUTABLE "$<TARGET_FILE:${executable}>")
    set(TEST_EXECUTABLE_NAME "${executable}")
    set(TEST_EXECUTABLE_TRIGGER_SHUTDOWN_OUTPUT
      "${CMAKE_CURRENT_SOURCE_DIR}/test/expected_outputs/${test_name}__trigger_shutdown")
    set(TEST_EXECUTABLE_EXPECTED_OUTPUT
      "${CMAKE_CURRENT_SOURCE_DIR}/test/expected_outputs/${test_name}__expected_output")
    configure_file(
      ${_ARG_LAUNCH_TEST_TEMPLATE}
      ${test_name}${target_suffix}.py.configure
      @ONLY
    )
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}_$<CONFIG>.py"
      INPUT "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}.py.configure"
    )
    add_launch_test(
      "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}_$<CONFIG>.py"
      TARGET ${test_name}${target_suffix}
      ${_ARG_UNPARSED_ARGUMENTS}
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
    )
    if(TEST ${test_name}${target_suffix})
      set_tests_properties(${test_name}${target_suffix}
        PROPERTIES DEPENDS "${executable}${target_suffix}"
      )
    endif()
  endmacro()

  macro(cross_rmw_tests)
    # test node names cross rmw
    if(rmw_implementation1 STREQUAL rmw_implementation2)
      set(target_suffix "__${rmw_implementation1}")
    else()
      set(target_suffix "__${rmw_implementation1}__${rmw_implementation2}")
    endif()

    set(rmw_implementation1_is_fastrtps FALSE)
    set(rmw_implementation2_is_fastrtps FALSE)
    if(rmw_implementation1 MATCHES "(.*)fastrtps(.*)")
      set(rmw_implementation1_is_fastrtps TRUE)
    endif()
    if(rmw_implementation2 MATCHES "(.*)fastrtps(.*)")
      set(rmw_implementation2_is_fastrtps TRUE)
    endif()

    set(rmw_implementation1_is_connext FALSE)
    set(rmw_implementation2_is_connext FALSE)
    if(rmw_implementation1 MATCHES "(.*)connext(.*)")
      set(rmw_implementation1_is_connext TRUE)
    endif()
    if(rmw_implementation2 MATCHES "(.*)connext(.*)")
      set(rmw_implementation2_is_connext TRUE)
    endif()

    set(rmw_implementation1_is_cyclonedds FALSE)
    set(rmw_implementation2_is_cyclonedds FALSE)
    if(rmw_implementation1 MATCHES "(.*)cyclonedds(.*)")
      set(rmw_implementation1_is_cyclonedds TRUE)
    endif()
    if(rmw_implementation2 MATCHES "(.*)cyclonedds(.*)")
      set(rmw_implementation2_is_cyclonedds TRUE)
    endif()


    # Whitelist cross-vendor tests
    if(NOT (rmw_implementation1 STREQUAL rmw_implementation2))
      # TODO(sloretz) enable connext/cyclone/fastrtps when all three use 1 participant per context
      if(
        (rmw_implementation1_is_fastrtps AND rmw_implementation2_is_cyclonedds) OR
        (rmw_implementation1_is_cyclonedds AND rmw_implementation2_is_fastrtps) OR
        (rmw_implementation1_is_fastrtps AND rmw_implementation2_is_fastrtps)
      )
        # Whitelisted cross-vendor tests
        set(_crt_SKIP_TEST ${SKIP_TEST})
      else()
        # Default skip cross-vendor tests
        set(_crt_SKIP_TEST "SKIP_TEST")
      endif()
    else()
      # Same vendor tests always allowed
      set(_crt_SKIP_TEST ${SKIP_TEST})
    endif()

    custom_launch_test_two_executables(test_node_name
      node_with_name node_name_list
      ARGS1 "${rmw_implementation1}" ARGS2 "node_with_name_${rmw_implementation1}"
      RMW1 ${rmw_implementation1} RMW2 ${rmw_implementation2}
      TIMEOUT 15
      ${_crt_SKIP_TEST})
  endmacro()

  macro(targets)
    custom_gtest(gtest_publisher
      "test/test_publisher.cpp"
      TIMEOUT 15)
    custom_gtest(gtest_avoid_ros_namespace_conventions_qos
      "test/test_avoid_ros_namespace_conventions_qos.cpp"
      TIMEOUT 15)
    custom_gtest(gtest_client_wait_for_service_shutdown
      "test/test_client_wait_for_service_shutdown.cpp"
      TIMEOUT 15)
    custom_gtest(gtest_executor
      "test/test_executor.cpp"
      TIMEOUT 60)
    custom_gtest(gtest_repeated_publisher_subscriber
      "test/test_repeated_publisher_subscriber.cpp"
      TIMEOUT 15)
    custom_gtest(gtest_spin
      "test/test_spin.cpp"
      TIMEOUT 30)
    custom_gtest(gtest_subscription
      "test/test_subscription.cpp"
      TIMEOUT 60)
    custom_gtest(gtest_multiple_service_calls
      "test/test_multiple_service_calls.cpp"
      TIMEOUT 60)
    custom_gtest(gtest_timer
      "test/test_timer.cpp"
      TIMEOUT 30)
    custom_gtest(gtest_timeout_subscriber
      "test/test_timeout_subscriber.cpp"
      TIMEOUT 30)
    custom_gtest(gtest_intra_process
      "test/test_intra_process.cpp"
      TIMEOUT 15)
    custom_gtest(gtest_multithreaded
      "test/test_multithreaded.cpp"
      TIMEOUT 90)
    custom_gtest(gtest_local_parameters
      "test/test_local_parameters.cpp"
      TIMEOUT 300)
    custom_gtest(gtest_services_in_constructor
      "test/test_services_in_constructor.cpp"
      TIMEOUT 30)
    custom_gtest(gtest_waitable
      "test/test_waitable.cpp"
      TIMEOUT 30)

    # Parameter tests single implementation
    custom_launch_test_two_executables(test_parameter_server_cpp
      test_parameters_server_cpp test_remote_parameters_cpp
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      TIMEOUT 60)

    # Service tests single implementation
    custom_launch_test_two_executables(test_services_cpp
      test_services_server_cpp test_services_client_cpp
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      TIMEOUT 60)

    custom_launch_test_two_executables(test_client_scope_cpp
      test_client_scope_server_cpp test_client_scope_client_cpp
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      TIMEOUT 60)

    custom_launch_test_two_executables(test_client_scope_consistency_cpp
      test_client_scope_consistency_server_cpp test_client_scope_consistency_client_cpp
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      TIMEOUT 60)

    custom_launch_n_nodes(10
      TIMEOUT 15)

    # Note (dhood): signal handler tests will be skipped on Windows because there is no opportunity
    # for signal handling once shutdown is triggered by launch_testing.
    set(SKIP_TEST "")
    if(WIN32)
      set(SKIP_TEST "SKIP_TEST")
    endif()
    # Test that a user-defined signal handler is called on interrupt:
    # after rclcpp::init has been called, but before rclcpp::shutdown has been called.
    custom_launch_test_executable_output(test_sigint_handler_before_shutdown
      test_sigint_handler
      TIMEOUT 30
      ${SKIP_TEST})

    # Test that a user-defined signal handler is restored after rclcpp::init and rclcpp::shutdown
    # have been called.
    custom_launch_test_executable_output(test_sigint_handler_after_shutdown
      test_sigint_handler
      TIMEOUT 30
      ${SKIP_TEST})

    # Test that a user-defined signal handler is called on interrupt:
    # after rclcpp::init has been called, but before rclcpp::shutdown has been called.
    custom_launch_test_executable_output(test_sigterm_handler_before_shutdown
      test_sigterm_handler
      LAUNCH_TEST_TEMPLATE test/test_sigterm.py.in
      TIMEOUT 30
      ${SKIP_TEST})

    # Test that a user-defined signal handler is restored after rclcpp::init and rclcpp::shutdown
    # have been called.
    custom_launch_test_executable_output(test_sigterm_handler_after_shutdown
      test_sigterm_handler
      LAUNCH_TEST_TEMPLATE test/test_sigterm.py.in
      TIMEOUT 30
      ${SKIP_TEST})

    # Test node names
    set(rmw_implementation1 "${rmw_implementation}")
    foreach(rmw_implementation2 ${rmw_implementations})
      cross_rmw_tests()
    endforeach()
  endmacro()

  set(append_library_dirs "${CMAKE_CURRENT_BINARY_DIR}")
  if(WIN32)
    set(append_library_dirs "${append_library_dirs}/$<CONFIG>")
  endif()

  # Test node names
  add_executable(node_with_name "test/node_with_name.cpp")
  ament_target_dependencies(node_with_name
    "rclcpp")
  add_executable(node_name_list "test/node_name_list.cpp")
  ament_target_dependencies(node_name_list
    "rclcpp")
  add_executable(node_check_names "test/node_check_names.cpp")
  ament_target_dependencies(node_check_names
    "rclcpp")

  call_for_each_rmw_implementation(targets)

  custom_executable(test_sigint_handler "test/test_sigint_handler.cpp")
  custom_executable(test_sigterm_handler "test/test_sigterm_handler.cpp")

  # Parameter tests single implementation
  custom_executable(test_parameters_server_cpp "test/test_parameters_server.cpp")
  custom_gtest_executable(test_remote_parameters_cpp "test/test_remote_parameters.cpp")

  # Service tests single implementation
  custom_executable(test_services_server_cpp "test/test_services_server.cpp")
  custom_gtest_executable(test_services_client_cpp "test/test_services_client.cpp")

  custom_executable(test_client_scope_server_cpp "test/test_client_scope_server.cpp")
  custom_gtest_executable(test_client_scope_client_cpp "test/test_client_scope_client.cpp")

  custom_executable(test_client_scope_consistency_server_cpp "test/test_client_scope_consistency_server.cpp")
  custom_gtest_executable(test_client_scope_consistency_client_cpp "test/test_client_scope_consistency_client.cpp")
endif()  # BUILD_TESTING

# TODO should not install anything
ament_package()

if(TEST cppcheck)
  # cppcheck test is created in hook in ament_package()
  set_tests_properties(cppcheck PROPERTIES TIMEOUT 400)
endif()
