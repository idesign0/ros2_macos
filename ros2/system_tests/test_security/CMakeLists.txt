cmake_minimum_required(VERSION 3.7)

project(test_security)

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

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

option(SECURITY "Activate security" OFF)

if(BUILD_TESTING)
  # No security test on services for now
  find_package(ament_cmake REQUIRED)
  find_package(test_msgs REQUIRED)

  # Test only a couple of message types to avoid taking too much test time
  set(message_files "msg/Empty.msg;msg/UnboundedSequences.msg")

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(launch_testing_ament_cmake REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)

  # get the rmw implementations ahead of time
  find_package(rmw_implementation_cmake REQUIRED)
  get_available_rmw_implementations(rmw_implementations)
  foreach(rmw_implementation ${rmw_implementations})
    find_package("${rmw_implementation}" REQUIRED)
  endforeach()

  function(custom_executable target)
    add_executable(${target} ${ARGN})
    ament_target_dependencies(${target}
      "rclcpp" "test_msgs")
  endfunction()

  function(custom_security_test_c target)
    ament_add_gtest(
      "${target}${target_suffix}" ${ARGN}
      TIMEOUT 10
      APPEND_LIBRARY_DIRS "${append_library_dirs}"
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
      ROS_SECURITY_KEYSTORE=${KEYSTORE_DIRECTORY_NATIVE_PATH}
      PATH="${TEST_PATH}"
    )
    if(TARGET ${target}${target_suffix})
      target_link_libraries(${target}${target_suffix}
        ${_AMENT_EXPORT_ABSOLUTE_LIBRARIES}
        ${_AMENT_EXPORT_LIBRARY_TARGETS})
      ament_target_dependencies(${target}${target_suffix}
        "rcl" "osrf_testing_tools_cpp")
      set_tests_properties(
        ${target}${target_suffix}
        PROPERTIES REQUIRED_FILES "$<TARGET_FILE:${target}${target_suffix}>"
        FIXTURES_REQUIRED "sros_artifacts"
      )
    endif()
  endfunction()

  macro(security_tests)
    set(suffix "__${rmw_implementation}")
    set(PUBLISHER_RMW ${rmw_implementation})
    set(SUBSCRIBER_RMW ${rmw_implementation})
    # Not testing across client libraries for now
    set(TEST_PUBLISHER_RCL "rclcpp")
    set(TEST_SUBSCRIBER_RCL "rclcpp")
    set(TEST_PUBLISHER_EXECUTABLE "$<TARGET_FILE:test_secure_publisher_cpp>")
    set(TEST_SUBSCRIBER_EXECUTABLE "$<TARGET_FILE:test_secure_subscriber_cpp>")

    # Test suite for communication without security
    set(non_secure_comm_PUBLISHER_ROS_SECURITY_ENABLE_LIST "false;false;false;true;false")
    set(non_secure_comm_SUBSCRIBER_ROS_SECURITY_ENABLE_LIST "false;false;false;false;true")
    set(non_secure_comm_PUBLISHER_ROS_SECURITY_STRATEGY_LIST "Enforce;garbage;garbage;Permissive;Garbage")
    set(non_secure_comm_SUBSCRIBER_ROS_SECURITY_STRATEGY_LIST "Enforce;Permissive;Garbage;Garbage;Permissive")
    set(non_secure_comm_PUBLISHER_ROS_SECURITY_KEYSTORE_LIST "garbage;WHATEVER;${KEYSTORE_DIRECTORY_NATIVE_PATH};garbage;garbage")
    set(SUBSCRIBER_ROS_SECURITY_KEYSTORE_LIST "${KEYSTORE_DIRECTORY_NATIVE_PATH};WHATEVER;garbage;garbage;garbage")

    # Test suite for secured communication
    set(secure_comm_PUBLISHER_ROS_SECURITY_ENABLE_LIST "true;true;true;true")
    set(secure_comm_SUBSCRIBER_ROS_SECURITY_ENABLE_LIST "true;true;true;true")
    set(secure_comm_PUBLISHER_ROS_SECURITY_STRATEGY_LIST "Enforce;Enforce;Permissive;Permissive")
    set(secure_comm_SUBSCRIBER_ROS_SECURITY_STRATEGY_LIST "Enforce;Permissive;Enforce;Permissive")

    # Test suite for one node with security and the second without
    set(not_connecting_PUBLISHER_ROS_SECURITY_ENABLE_LIST "false;true")
    set(not_connecting_SUBSCRIBER_ROS_SECURITY_ENABLE_LIST "true;false")
    set(not_connecting_PUBLISHER_ROS_SECURITY_STRATEGY_LIST "Permissive;Enforce")
    set(not_connecting_SUBSCRIBER_ROS_SECURITY_STRATEGY_LIST "Enforce;Permissive")

    list(LENGTH non_secure_comm_PUBLISHER_ROS_SECURITY_ENABLE_LIST n_non_secure_tests)
    list(LENGTH secure_comm_PUBLISHER_ROS_SECURITY_ENABLE_LIST n_secure_communication_tests)
    list(LENGTH not_connecting_PUBLISHER_ROS_SECURITY_ENABLE_LIST n_not_connecting_tests)

    foreach(message_file ${message_files})
      get_filename_component(TEST_MESSAGE_TYPE "${message_file}" NAME_WE)

      set(index 0)
      # configure all non secure communication tests
      set(SUBSCRIBER_SHOULD_TIMEOUT "false")
      while(index LESS ${n_non_secure_tests})
        # here we define all the variables needed for security template expansion
        list(GET non_secure_comm_PUBLISHER_ROS_SECURITY_ENABLE_LIST ${index} PUBLISHER_ROS_SECURITY_ENABLE)
        list(GET SUBSCRIBER_ROS_SECURITY_ENABLE_LIST ${index} SUBSCRIBER_ROS_SECURITY_ENABLE)
        list(GET PUBLISHER_ROS_SECURITY_STRATEGY_LIST ${index} PUBLISHER_ROS_SECURITY_STRATEGY)
        list(GET SUBSCRIBER_ROS_SECURITY_STRATEGY_LIST ${index} SUBSCRIBER_ROS_SECURITY_STRATEGY)
        list(GET PUBLISHER_ROS_SECURITY_KEYSTORE_LIST ${index} PUBLISHER_ROS_SECURITY_KEYSTORE)
        list(GET SUBSCRIBER_ROS_SECURITY_KEYSTORE_LIST ${index} SUBSCRIBER_ROS_SECURITY_KEYSTORE)

        set(test_suffix "__${TEST_MESSAGE_TYPE}${suffix}__non_secure_comm_${index}")
        configure_file(
          test/test_secure_publisher_subscriber.py.in
          test_secure_publisher_subscriber${test_suffix}.py.configured
          @ONLY
        )
        file(GENERATE
          OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}_$<CONFIG>.py"
          INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}.py.configured"
        )
        math(EXPR index "${index} + 1")

        add_launch_test(
          "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}_$<CONFIG>.py"
          TARGET test_secure_publisher_subscriber${test_suffix}
          APPEND_LIBRARY_DIRS "${append_library_dirs}"
          ENV
          PATH="${TEST_PATH}"
          TIMEOUT 20
        )
        if(TEST test_secure_publisher_subscriber${test_suffix})
          set_tests_properties(
            test_secure_publisher_subscriber${test_suffix}
            PROPERTIES DEPENDS "test_secure_publisher_cpp__${rmw_implementation};test_secure_subscriber_cpp__${rmw_implementation}"
            FIXTURES_REQUIRED "sros_artifacts"
          )
        endif()
      endwhile()

      set(index 0)
      set(SUBSCRIBER_SHOULD_TIMEOUT "false")
      set(PUBLISHER_ROS_SECURITY_KEYSTORE "${KEYSTORE_DIRECTORY_NATIVE_PATH}")
      set(SUBSCRIBER_ROS_SECURITY_KEYSTORE "${KEYSTORE_DIRECTORY_NATIVE_PATH}")
      # configure all secure communication tests
      while(index LESS ${n_secure_communication_tests})
        # here we define all the variables needed for security template expansion
        list(GET secure_comm_PUBLISHER_ROS_SECURITY_ENABLE_LIST ${index} PUBLISHER_ROS_SECURITY_ENABLE)
        list(GET secure_comm_SUBSCRIBER_ROS_SECURITY_ENABLE_LIST ${index} SUBSCRIBER_ROS_SECURITY_ENABLE)
        list(GET secure_comm_PUBLISHER_ROS_SECURITY_STRATEGY_LIST ${index} PUBLISHER_ROS_SECURITY_STRATEGY)
        list(GET secure_comm_SUBSCRIBER_ROS_SECURITY_STRATEGY_LIST ${index} SUBSCRIBER_ROS_SECURITY_STRATEGY)

        set(test_suffix "__${TEST_MESSAGE_TYPE}${suffix}__secure_comm_${index}")
        configure_file(
          test/test_secure_publisher_subscriber.py.in
          test_secure_publisher_subscriber${test_suffix}.py.configured
          @ONLY
        )
        file(GENERATE
          OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}_$<CONFIG>.py"
          INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}.py.configured"
        )
        math(EXPR index "${index} + 1")

        add_launch_test(
          "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}_$<CONFIG>.py"
          TARGET test_secure_publisher_subscriber${test_suffix}
          APPEND_LIBRARY_DIRS "${append_library_dirs}"
          ENV
          PATH="${TEST_PATH}"
          TIMEOUT 20
        )
        if(TEST test_secure_publisher_subscriber${test_suffix})
          set_tests_properties(
            test_secure_publisher_subscriber${test_suffix}
            PROPERTIES DEPENDS "test_secure_publisher_cpp__${rmw_implementation};test_secure_subscriber_cpp__${rmw_implementation}"
            FIXTURES_REQUIRED "sros_artifacts"
          )
        endif()
      endwhile()

      set(index 0)
      set(PUBLISHER_ROS_SECURITY_KEYSTORE "${KEYSTORE_DIRECTORY_NATIVE_PATH}")
      set(SUBSCRIBER_ROS_SECURITY_KEYSTORE "${KEYSTORE_DIRECTORY_NATIVE_PATH}")
      set(SUBSCRIBER_SHOULD_TIMEOUT "true")
      # configure all not connecting tests
      while(index LESS ${n_not_connecting_tests})
        # here we define all the variables needed for security template expansion
        list(GET not_connecting_PUBLISHER_ROS_SECURITY_ENABLE_LIST ${index} PUBLISHER_ROS_SECURITY_ENABLE)
        list(GET not_connecting_SUBSCRIBER_ROS_SECURITY_ENABLE_LIST ${index} SUBSCRIBER_ROS_SECURITY_ENABLE)
        list(GET not_connecting_PUBLISHER_ROS_SECURITY_STRATEGY_LIST ${index} PUBLISHER_ROS_SECURITY_STRATEGY)
        list(GET not_connecting_SUBSCRIBER_ROS_SECURITY_STRATEGY_LIST ${index} SUBSCRIBER_ROS_SECURITY_STRATEGY)

        set(test_suffix "__${TEST_MESSAGE_TYPE}${suffix}__secure_not_connecting_${index}")
        configure_file(
          test/test_secure_publisher_subscriber.py.in
          test_secure_publisher_subscriber${test_suffix}.py.configured
          @ONLY
        )
        file(GENERATE
          OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}_$<CONFIG>.py"
          INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}.py.configured"
        )
        math(EXPR index "${index} + 1")

        add_launch_test(
          "${CMAKE_CURRENT_BINARY_DIR}/test_secure_publisher_subscriber${test_suffix}_$<CONFIG>.py"
          TARGET test_secure_publisher_subscriber${test_suffix}
          APPEND_LIBRARY_DIRS "${append_library_dirs}"
          ENV
          PATH="${TEST_PATH}"
          TIMEOUT 20
        )
        if(TEST test_secure_publisher_subscriber${test_suffix})
          set_tests_properties(
            test_secure_publisher_subscriber${test_suffix}
            PROPERTIES DEPENDS "test_secure_publisher_cpp__${rmw_implementation};test_secure_subscriber_cpp__${rmw_implementation}"
            FIXTURES_REQUIRED "sros_artifacts"
          )
        endif()
      endwhile()
    endforeach()
  endmacro()

  macro(targets)
    set(ENV_PATH "$ENV{PATH}")
    file(TO_CMAKE_PATH "${ENV_PATH}" ENV_PATH)
    set(TEST_PATH "${ENV_PATH}")
    if(rmw_implementation MATCHES "rmw_connext(.*)")
      # Connext 5.3.1 needs RTI's OpenSSL binaries (based on EOL 1.0.2) to be
      # on the PATH at runtime as the system version of OpenSSL is not supported
      set(RTI_BIN_PATH "$ENV{RTI_OPENSSL_BIN}")
      file(TO_CMAKE_PATH "${RTI_BIN_PATH}" RTI_BIN_PATH)
      set(TEST_PATH "${RTI_BIN_PATH};${ENV_PATH}")
    endif()

    if(NOT WIN32)
      string(REPLACE ";" ":" TEST_PATH "${TEST_PATH}")
    endif()

    # TODO(jacobperron) Disable Connext on Windows until we fix issue with CI
    # TODO(asorbini): Remove exceptions once ros2/rmw_connext is deprecated.
    if(
      (rmw_implementation STREQUAL "rmw_connext_cpp" AND NOT WIN32) OR
      (rmw_implementation STREQUAL "rmw_connext_dynamic_cpp" AND NOT WIN32) OR
      (rmw_implementation STREQUAL "rmw_connextdds" AND NOT WIN32) OR
      rmw_implementation STREQUAL "rmw_fastrtps_cpp" OR
      rmw_implementation STREQUAL "rmw_fastrtps_dynamic_cpp" OR
      rmw_implementation STREQUAL "rmw_cyclonedds_cpp"
    )
      custom_security_test_c(test_security_nodes_c
        "test/test_invalid_secure_node_creation_c.cpp")
      security_tests()
    endif()
  endmacro()

  if(SECURITY)
    # executables secure publisher / subscriber
    custom_executable(test_secure_publisher_cpp
      "test/test_secure_publisher.cpp")
    custom_executable(test_secure_subscriber_cpp
      "test/test_secure_subscriber.cpp")

    set(append_library_dirs "${CMAKE_CURRENT_BINARY_DIR}")
    if(WIN32)
      set(append_library_dirs "${append_library_dirs}/$<CONFIG>")
      string(REPLACE "\\" "\\\\" RTI_OPENSSL_LIBS "$ENV{RTI_OPENSSL_LIBS}")
      set(append_library_dirs "${append_library_dirs};${RTI_OPENSSL_LIBS}")
    # TODO(mikaelarguedas) uncomment this once Connext supports OpenSSL 1.1.1
    # in the meantime use RTI_OPENSSL_LIBS
    # elseif(APPLE)
    #   # connext need the openssl libraries to be on the DYLD_LIBRARY_PATH at runtime
    #   # as SIP strips these variables on recent MacOS we populate them here
    #   # see https://github.com/ros2/ros2/issues/409
    #   execute_process(
    #     COMMAND "brew" "--prefix" "openssl"
    #     RESULT_VARIABLE _retcode
    #     OUTPUT_VARIABLE _out_var
    #     ERROR_VARIABLE _error_var
    #   )
    #   if(NOT _retcode EQUAL 0)
    #     message(FATAL_ERROR "command 'brew --prefix openssl' failed with error code '${_retcode}' and error message '${_error_var}'")
    #   endif()
    #   string(STRIP "${_out_var}" _out_var)
    #   set(openssl_lib_path "${_out_var}/lib")
    #   file(TO_NATIVE_PATH "${openssl_lib_path}" openssl_lib_path)
    #   set(append_library_dirs "${append_library_dirs};${openssl_lib_path}")
    else()
      set(append_library_dirs "${append_library_dirs}:$ENV{RTI_OPENSSL_LIBS}")
    endif()

    # finding gtest once in the highest scope
    # prevents finding it repeatedly in each local scope
    ament_find_gtest()

    set(KEYSTORE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test/test_security_files")
    if(WIN32)
      string(REPLACE "/" "\\\\" KEYSTORE_DIRECTORY_NATIVE_PATH "${KEYSTORE_DIRECTORY}")
    else()
      set(KEYSTORE_DIRECTORY_NATIVE_PATH "${KEYSTORE_DIRECTORY}")
    endif()

    #
    # CTest Fixtures
    #
    # These fixtures are set up as needed at the beginning of the test run to
    # support any selected tests which require them.
    #
    # * sros_artifacts: This fixture generates SROS2 security artifacts needed
    #   for the tests in this package.
    #

    find_program(ROS2_EXECUTABLE ros2)

    add_test(NAME sros_artifacts
      COMMAND ${CMAKE_COMMAND}
        "-DROS2_EXECUTABLE=${ROS2_EXECUTABLE}"
        "-DKEYSTORE_DIRECTORY=${KEYSTORE_DIRECTORY}"
        "-DKEYSTORE_DIRECTORY_NATIVE_PATH=${KEYSTORE_DIRECTORY_NATIVE_PATH}"
        "-P${CMAKE_CURRENT_SOURCE_DIR}/test/sros_artifacts.cmake"
    )
    set_tests_properties(sros_artifacts PROPERTIES
      FIXTURES_SETUP sros_artifacts
    )

    call_for_each_rmw_implementation(targets)
  endif()
endif()  # BUILD_TESTING

ament_auto_package()
