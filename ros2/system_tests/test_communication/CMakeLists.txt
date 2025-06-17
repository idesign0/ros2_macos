cmake_minimum_required(VERSION 3.5)

project(test_communication)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND MSVC)
  # /bigobj is needed to avoid error C1128:
  #   https://msdn.microsoft.com/en-us/library/8578y171.aspx
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

option(SKIP_SINGLE_RMW_TESTS
  "Skip tests involving only a single RMW implementation" OFF)
option(SKIP_MULTI_RMW_TESTS
  "Skip tests involving only multiple RMW implementations" OFF)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

if(BUILD_TESTING)
  find_package(ament_cmake REQUIRED)
  find_package(test_msgs REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)

  ament_index_get_resource(interface_files "rosidl_interfaces" "test_msgs")
  string(REPLACE "\n" ";" interface_files "${interface_files}")

  set(message_files "")
  set(service_files "")
  set(action_files "")
  foreach(interface_file ${interface_files})
    get_filename_component(interface_ns "${interface_file}" DIRECTORY)
    get_filename_component(interface_ns "${interface_ns}" NAME)
    string_ends_with("${interface_file}" ".msg" is_message)
    if(is_message AND interface_ns STREQUAL "msg")
      list(APPEND message_files "${interface_file}")
      continue()
    endif()
    string_ends_with("${interface_file}" ".srv" is_service)
    if(is_service AND interface_ns STREQUAL "srv")
      list(APPEND service_files "${interface_file}")
      continue()
    endif()
    string_ends_with("${interface_file}" ".idl" is_action)
    if(is_action AND interface_ns STREQUAL "action")
      list(APPEND action_files "${interface_file}")
      continue()
    endif()
  endforeach()

  set(other_message_files
    "msg/FieldsWithSameType.msg"
    "msg/UInt32.msg"
  )
  rosidl_generate_interfaces(${PROJECT_NAME}
    ${other_message_files}
    DEPENDENCIES test_msgs
    SKIP_INSTALL
  )

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(launch_testing_ament_cmake REQUIRED)

  # Provides PYTHON_EXECUTABLE_DEBUG
  find_package(python_cmake_module REQUIRED)
  find_package(PythonExtra REQUIRED)

  # get the rmw implementations ahead of time
  find_package(rmw_implementation_cmake REQUIRED)
  get_available_rmw_implementations(rmw_implementations2)
  foreach(rmw_implementation ${rmw_implementations2})
    find_package("${rmw_implementation}" REQUIRED)
  endforeach()

  function(custom_test target with_message_argument)
    if(with_message_argument)
      # adding test for each message type
      foreach(message_file ${message_files})
        get_filename_component(TEST_MESSAGE_TYPE "${message_file}" NAME_WE)
        ament_add_test(
          "${target}${target_suffix}__${TEST_MESSAGE_TYPE}"
          COMMAND "$<TARGET_FILE:${target}>" "${TEST_MESSAGE_TYPE}"
          TIMEOUT 15
          GENERATE_RESULT_FOR_RETURN_CODE_ZERO
          APPEND_LIBRARY_DIRS "${append_library_dirs}"
          ENV
          RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
          RMW_IMPLEMENTATION=${rmw_implementation})
        set_tests_properties(
          "${target}${target_suffix}__${TEST_MESSAGE_TYPE}"
          PROPERTIES REQUIRED_FILES "$<TARGET_FILE:${target}>"
        )
      endforeach()
    else()
      ament_add_test(
        "${target}${target_suffix}"
        COMMAND "$<TARGET_FILE:${target}>"
        TIMEOUT 15
        GENERATE_RESULT_FOR_RETURN_CODE_ZERO
        APPEND_LIBRARY_DIRS "${append_library_dirs}"
        ENV
        RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
        RMW_IMPLEMENTATION=${rmw_implementation})
      set_tests_properties(
        "${target}${target_suffix}"
        PROPERTIES REQUIRED_FILES "$<TARGET_FILE:${target}>"
      )
    endif()
  endfunction()

  function(custom_executable target)
    add_executable(${target} ${ARGN})
    rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
    target_link_libraries(${target} "${cpp_typesupport_target}")
    ament_target_dependencies(${target}
      "rclcpp"
      "rclcpp_action"
      "test_msgs"
    )
  endfunction()

  function(custom_test_c target)
    ament_add_gtest(
      "${target}${target_suffix}" ${ARGN}
      TIMEOUT 90
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation})
    if(TARGET ${target}${target_suffix})
      rosidl_get_typesupport_target(c_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_c")
      target_link_libraries(${target}${target_suffix} "${c_typesupport_target}")
      ament_target_dependencies(${target}${target_suffix}
        "osrf_testing_tools_cpp" "rcl")
      set_tests_properties(
        ${target}${target_suffix}
        PROPERTIES REQUIRED_FILES "$<TARGET_FILE:${target}${target_suffix}>"
      )
    endif()
  endfunction()

  macro(multi_targets)
    # test publish / subscribe messages
    if(rmw_implementation1 STREQUAL rmw_implementation2)
      set(suffix "${suffix}__${rmw_implementation1}")
    else()
      set(suffix "${suffix}__${rmw_implementation1}__${rmw_implementation2}")
    endif()

    set(SKIP_TEST "")

    # TODO(wjwwood): Connext and Fast-RTPS do not currently communicate over pub/sub
    set(rmw_implementation1_is_fastrtps FALSE)
    set(rmw_implementation2_is_fastrtps FALSE)
    if(rmw_implementation1 MATCHES "(.*)fastrtps(.*)")
      set(rmw_implementation1_is_fastrtps TRUE)
    endif()
    if(rmw_implementation2 MATCHES "(.*)fastrtps(.*)")
      set(rmw_implementation2_is_fastrtps TRUE)
    endif()
    if(
      WIN32 AND
      NOT "${rmw_implementation1_is_fastrtps}" STREQUAL "${rmw_implementation2_is_fastrtps}"
    )
      set(SKIP_TEST "SKIP_TEST")
    endif()

    set(rmw_implementation1_is_connext FALSE)
    if(rmw_implementation1 MATCHES "(.*)connext(.*)")
      set(rmw_implementation1_is_connext TRUE)
    endif()
    set(rmw_implementation2_is_connext FALSE)
    if(rmw_implementation2 MATCHES "(.*)connext(.*)")
      set(rmw_implementation2_is_connext TRUE)
    endif()

    # TODO(asorbini) Skip tests between rmw_connext_cpp and rmw_connextdds
    # since they are not meant to coexist (and used incompatible typecodes by default).
    set(rmw_implementation1_is_connextdds FALSE)
    if(rmw_implementation1 MATCHES "(.*)connextdds")
      set(rmw_implementation1_is_connextdds TRUE)
    endif()
    set(rmw_implementation2_is_connextdds FALSE)
    if(rmw_implementation2 MATCHES "(.*)connextdds")
      set(rmw_implementation2_is_connextdds TRUE)
    endif()

    set(rmw_implementation1_is_connext_cpp FALSE)
    if(rmw_implementation1 MATCHES "(.*)connext(.*)_cpp")
      set(rmw_implementation1_is_connext_cpp TRUE)
    endif()
    set(rmw_implementation2_is_connext_cpp FALSE)
    if(rmw_implementation2 MATCHES "(.*)connext(.*)_cpp")
      set(rmw_implementation2_is_connext_cpp TRUE)
    endif()

    if((rmw_implementation1_is_connextdds AND rmw_implementation2_is_connext_cpp) OR
      (rmw_implementation1_is_connext_cpp AND rmw_implementation2_is_connextdds)
    )
      set(SKIP_TEST "SKIP_TEST")
    endif()

    set(rmw_implementation1_is_cyclonedds FALSE)
    set(rmw_implementation2_is_cyclonedds FALSE)
    if(rmw_implementation1 MATCHES "(.*)cyclonedds(.*)")
      set(rmw_implementation1_is_cyclonedds TRUE)
    endif()
    if(rmw_implementation2 MATCHES "(.*)cyclonedds(.*)")
      set(rmw_implementation2_is_cyclonedds TRUE)
    endif()

    set(PUBLISHER_RMW ${rmw_implementation1})
    set(SUBSCRIBER_RMW ${rmw_implementation2})
    set(TEST_MESSAGE_TYPES "")
    foreach(message_file ${message_files})
      get_filename_component(message_type "${message_file}" NAME_WE)
      # TODO(dirk-thomas) WStrings published by FastRTPS can't be received
      # correctly by Connext on macOS
      if(
        "${message_type}" STREQUAL "WStrings" AND
        rmw_implementation1_is_fastrtps AND
        rmw_implementation2_is_connext AND
        APPLE
      )
        continue()
      endif()
      # TODO(dirk-thomas) Connext and CycloneDDS don't interoperate for WString
      if(
        "${message_type}" STREQUAL "WStrings" AND
        (
          (rmw_implementation1_is_connext AND rmw_implementation2_is_cyclonedds) OR
          (rmw_implementation1_is_cyclonedds AND rmw_implementation2_is_connext)
        )
      )
        continue()
      endif()
      list(APPEND TEST_MESSAGE_TYPES "${message_type}")
    endforeach()
    configure_file(
      test/test_publisher_subscriber.py.in
      test_publisher_subscriber${suffix}.py.configured
      @ONLY
    )
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_publisher_subscriber${suffix}_$<CONFIG>.py"
      INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_publisher_subscriber${suffix}.py.configured"
    )

    list(LENGTH TEST_MESSAGE_TYPES length)
    math(EXPR timeout "${length} * 15")
    add_launch_test(
      "${CMAKE_CURRENT_BINARY_DIR}/test_publisher_subscriber${suffix}_$<CONFIG>.py"
      TARGET test_publisher_subscriber${suffix}
      PYTHON_EXECUTABLE "${_PYTHON_EXECUTABLE}"
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      TIMEOUT ${timeout}
      ${SKIP_TEST})
    if(TEST test_publisher_subscriber${suffix})
      set_tests_properties(
        test_publisher_subscriber${suffix}
        PROPERTIES DEPENDS "test_publisher_cpp__${rmw_implementation1};test_subscriber_cpp__${rmw_implementation2}"
      )
    endif()

    # test requester / replier
    set(SKIP_TEST "")

    # TODO(mikaelarguedas) Simpler way to blacklist specific tests (e.g. regex matching)
    # TODO different vendors can't talk to each other right now
    # TODO(asorbini): Remove Connext exceptions once ros2/rmw_connext is deprecated.
    if(NOT rmw_implementation1 STREQUAL rmw_implementation2 OR
      (client_library1 STREQUAL "rclpy" AND rmw_implementation1 MATCHES "rmw_connext_dynamic_cpp") OR
      (client_library2 STREQUAL "rclpy" AND rmw_implementation2 MATCHES "rmw_connext_dynamic_cpp")
    )
      set(SKIP_TEST "SKIP_TEST")
    endif()

    set(REQUESTER_RMW ${rmw_implementation1})
    set(REPLIER_RMW ${rmw_implementation2})
    set(TEST_SERVICE_TYPES "")
    foreach(service_file ${service_files})
      get_filename_component(service_type "${service_file}" NAME_WE)
      list(APPEND TEST_SERVICE_TYPES "${service_type}")
    endforeach()
    configure_file(
      test/test_requester_replier.py.in
      test_requester_replier${suffix}.py.configured
      @ONLY
    )
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_requester_replier${suffix}_$<CONFIG>.py"
      INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_requester_replier${suffix}.py.configured"
    )

    list(LENGTH TEST_SERVICE_TYPES length)
    math(EXPR timeout "${length} * 30")
    add_launch_test(
      "${CMAKE_CURRENT_BINARY_DIR}/test_requester_replier${suffix}_$<CONFIG>.py"
      TARGET test_requester_replier${suffix}
      PYTHON_EXECUTABLE "${_PYTHON_EXECUTABLE}"
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      TIMEOUT ${timeout}
      ${SKIP_TEST})
    if(TEST test_requester_replier${suffix})
      set_tests_properties(
        test_requester_replier${suffix}
        PROPERTIES DEPENDS
          "test_requester_cpp__${rmw_implementation1};test_replier_cpp__${rmw_implementation2}"
      )
    endif()

    # test action client / server
    # Note: taking same exclusions as services since actions use services too
    # set(SKIP_TEST "")

    set(ACTION_CLIENT_RMW ${rmw_implementation1})
    set(ACTION_SERVER_RMW ${rmw_implementation2})
    set(TEST_ACTION_TYPES "")
    foreach(action_file ${action_files})
      get_filename_component(action_type "${action_file}" NAME_WE)
      list(APPEND TEST_ACTION_TYPES "${action_type}")
    endforeach()
    configure_file(
      test/test_action_client_server.py.in
      test_action_client_server${suffix}.py.configured
      @ONLY
    )
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_action_client_server${suffix}_$<CONFIG>.py"
      INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_action_client_server${suffix}.py.configured"
    )

    list(LENGTH TEST_ACTION_TYPES length)
    math(EXPR timeout "${length} * 30")
    add_launch_test(
      "${CMAKE_CURRENT_BINARY_DIR}/test_action_client_server${suffix}_$<CONFIG>.py"
      TARGET test_action_client_server${suffix}
      PYTHON_EXECUTABLE "${_PYTHON_EXECUTABLE}"
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      TIMEOUT ${timeout}
      ${SKIP_TEST})
    if(TEST test_action_client_server${suffix})
      set_tests_properties(
        test_action_client_server${suffix}
        PROPERTIES DEPENDS
          "test_action_client_cpp__${rmw_implementation1};test_action_server_cpp__${rmw_implementation2}"
      )
    endif()
  endmacro()

  macro(configure_template _client_library1 _client_library2)
    set(_PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE}")
    set(_client_library1 "${_client_library1}")
    set(_client_library2 "${_client_library2}")
    set(TEST_PUBLISHER_RCL "${_client_library1}")
    set(TEST_SUBSCRIBER_RCL "${_client_library2}")
    set(TEST_REQUESTER_RCL "${_client_library1}")
    set(TEST_REPLIER_RCL "${_client_library2}")
    set(TEST_ACTION_CLIENT_RCL "${_client_library1}")
    set(TEST_ACTION_SERVER_RCL "${_client_library2}")

    if(_client_library1 STREQUAL _client_library2)
      set(suffix "__${_client_library1}")
    else()
      set(suffix "__${_client_library1}__${_client_library2}")
    endif()

    if(_client_library1 STREQUAL "rclpy" OR _client_library2 STREQUAL "rclpy")
      if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
          set(_PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE_DEBUG}")
        endif()
      endif()
    endif()

    if(_client_library1 STREQUAL "rclpy")
      set(TEST_PUBLISHER_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/test/publisher_py.py")
      set(TEST_REQUESTER_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/test/requester_py.py")
      set(TEST_ACTION_CLIENT_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/test/action_client_py.py")
    elseif(_client_library1 STREQUAL "rclcpp")
      set(TEST_PUBLISHER_EXECUTABLE "$<TARGET_FILE:test_publisher_cpp>")
      set(TEST_REQUESTER_EXECUTABLE "$<TARGET_FILE:test_requester_cpp>")
      set(TEST_ACTION_CLIENT_EXECUTABLE "$<TARGET_FILE:test_action_client_cpp>")
    endif()

    if(_client_library2 STREQUAL "rclpy")
      set(TEST_SUBSCRIBER_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/test/subscriber_py.py")
      set(TEST_REPLIER_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/test/replier_py.py")
      set(TEST_ACTION_SERVER_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/test/action_server_py.py")
    elseif(_client_library2 STREQUAL "rclcpp")
      set(TEST_SUBSCRIBER_EXECUTABLE "$<TARGET_FILE:test_subscriber_cpp>")
      set(TEST_REPLIER_EXECUTABLE "$<TARGET_FILE:test_replier_cpp>")
      set(TEST_ACTION_SERVER_EXECUTABLE "$<TARGET_FILE:test_action_server_cpp>")
    endif()
  endmacro()


  macro(multi_client_libraries)
    # TODO(mikaelarguedas) Aggregate available rcl from ament index ?
    set(client_libraries "")
    list(APPEND client_libraries "rclpy")
    list(APPEND client_libraries "rclcpp")
    foreach(client_library1 ${client_libraries})
      foreach(client_library2 ${client_libraries})
        configure_template("${client_library1}" "${client_library2}")
        multi_targets()
      endforeach()
    endforeach()
  endmacro()

  add_library(subscribe_types STATIC
    "test/subscribe_array_types.cpp"
    "test/subscribe_basic_types.cpp"
    "test/subscribe_string_types.cpp")
  ament_target_dependencies(subscribe_types
    "rclcpp"
    "test_msgs")

  # publisher combined with a subscriber
  custom_executable(test_publisher_subscriber_cpp
    "test/test_publisher_subscriber.cpp")
  target_link_libraries(test_publisher_subscriber_cpp subscribe_types)
  # subcription valid data
  custom_executable(test_subscription_valid_data_cpp
    "test/test_subscription_valid_data.cpp")
  # executables publisher / subscriber
  custom_executable(test_publisher_cpp
    "test/test_publisher.cpp")
  custom_executable(test_subscriber_cpp
    "test/test_subscriber.cpp")
  target_link_libraries(test_subscriber_cpp subscribe_types)
  # executables requester / replier
  custom_executable(test_requester_cpp
    "test/test_requester.cpp")
  custom_executable(test_replier_cpp
    "test/test_replier.cpp")
  # executables action client / server
  custom_executable(test_action_client_cpp
    "test/test_action_client.cpp")
  custom_executable(test_action_server_cpp
    "test/test_action_server.cpp")

  macro(targets)
    if(NOT SKIP_SINGLE_RMW_TESTS)
      custom_test_c(test_messages_c
        "test/test_messages_c.cpp")

      # publisher combined with a subscriber
      custom_test(test_publisher_subscriber_cpp TRUE)
      # subcription valid data
      custom_test(test_subscription_valid_data_cpp FALSE)
    endif()

    set(rmw_implementation1 "${rmw_implementation}")
    set(target_suffix1 "${target_suffix}")

    foreach(rmw_implementation2 ${rmw_implementations2})
      if(
        rmw_implementation1 STREQUAL rmw_implementation2 AND
        NOT SKIP_SINGLE_RMW_TESTS OR
        NOT rmw_implementation1 STREQUAL rmw_implementation2 AND
        NOT SKIP_MULTI_RMW_TESTS
      )
        multi_client_libraries()
      endif()
    endforeach()
  endmacro()

  set(append_library_dirs "${CMAKE_CURRENT_BINARY_DIR}")
  if(WIN32)
    set(append_library_dirs "${append_library_dirs}/$<CONFIG>")
  endif()

  macro(serialize)
    # TODO(asorbini): Remove this exception once ros2/rmw_connext is deprecated.
    set(SKIP_TEST "")
    if(${rmw_implementation} STREQUAL "rmw_connext_dynamic_cpp")
      message(STATUS "skipping serialize tests for ${rmw_implementation}")
      set(SKIP_TEST "SKIP_TEST")
    endif()

    set(serialize_target_name "test_serialize${target}${target_suffix}")
    ament_add_gtest(
      ${serialize_target_name} test/test_message_serialization.cpp
      TIMEOUT 30
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      ${SKIP_TEST}
    )
    if(TARGET ${serialize_target_name})
      add_dependencies(${serialize_target_name} ${PROJECT_NAME})
      ament_target_dependencies(${serialize_target_name}
        "rmw"
        "rosidl_typesupport_c"
        "rosidl_typesupport_cpp"
        "test_msgs"
        ${rmw_implementation}
      )
      set_tests_properties(
        ${serialize_target_name}
        PROPERTIES REQUIRED_FILES "$<TARGET_FILE:${serialize_target_name}>"
      )
    endif()
  endmacro()

  macro(pub_sub_serialized)
    # TODO(asorbini): Remove this exception once ros2/rmw_connext is deprecated.
    set(SKIP_TEST "")
    if(${rmw_implementation} STREQUAL "rmw_connext_dynamic_cpp")
      message(STATUS "skipping serialize tests for ${rmw_implementation}")
      set(SKIP_TEST "SKIP_TEST")
    endif()

    set(target_name "test_publisher_subscriber_serialized${target}${target_suffix}")
    ament_add_gtest(
      ${target_name} test/test_publisher_subscriber_serialized.cpp
      TIMEOUT 30
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      ${SKIP_TEST}
    )
    if(TARGET ${target_name})
      add_dependencies(${target_name} ${PROJECT_NAME})
      ament_target_dependencies(${target_name}
        "rclcpp"
        "rclcpp_action"
        "rmw"
        "rosidl_typesupport_c"
        "rosidl_typesupport_cpp"
        "test_msgs"
        ${rmw_implementation}
      )
      set_tests_properties(
        ${target_name}
        PROPERTIES REQUIRED_FILES "$<TARGET_FILE:${target_name}>"
      )
    endif()
  endmacro()

  # finding gtest once in the highest scope
  # prevents finding it repeatedly in each local scope
  ament_find_gtest()

  call_for_each_rmw_implementation(targets)
  if(NOT SKIP_SINGLE_RMW_TESTS)
    call_for_each_rmw_implementation(serialize)
    call_for_each_rmw_implementation(pub_sub_serialized)
  endif()
endif()  # BUILD_TESTING

ament_auto_package()
