^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_communication
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.4 (2023-11-21)
-------------------

0.12.3 (2022-04-05)
-------------------
* Split test_subscriber into multiple compilation units. (`#500 <https://github.com/ros2/system_tests/issues/500>`_)
* Contributors: Chris Lalancette

0.12.2 (2022-03-28)
-------------------
* Add test_msgs dependency (`#497 <https://github.com/ros2/system_tests/issues/497>`_)
* Contributors: Shane Loretz

0.12.1 (2022-01-13)
-------------------

0.12.0 (2021-11-18)
-------------------
* Update python nodes SIGINT handling (`#490 <https://github.com/ros2/system_tests/issues/490>`_)
* Updated maintainers (`#489 <https://github.com/ros2/system_tests/issues/489>`_)
* Fix deprecated subscriber callback warnings (`#483 <https://github.com/ros2/system_tests/issues/483>`_)
* Add tests for BoundedPlainSequences (`#481 <https://github.com/ros2/system_tests/issues/481>`_)
* Use rosidl_get_typesupport_target() (`#480 <https://github.com/ros2/system_tests/issues/480>`_)
* Use rcpputils/scope_exit.hpp instead of rclcpp/scope_exit.hpp (`#479 <https://github.com/ros2/system_tests/issues/479>`_)
* Add changelogs (`#473 <https://github.com/ros2/system_tests/issues/473>`_)
* Contributors: Abrar Rahman Protyasha, Aditya Pande, Christophe Bedard, Ivan Santiago Paunovic, Shane Loretz

0.11.1 (2021-04-26)
-------------------

0.11.0 (2021-04-06)
-------------------

0.10.0 (2021-03-18)
-------------------
* Add support for rmw_connextdds. (`#463 <https://github.com/ros2/system_tests/issues/463>`_)
* Kill off the ros2 daemon before running tests. (`#460 <https://github.com/ros2/system_tests/pull/460>`_)
* Remove Opensplice from test_communication. (`#460 <https://github.com/ros2/system_tests/pull/460>`_)
* Make TestMessageSerialization robust to missed messages. (`#456 <https://github.com/ros2/system_tests/issues/456>`_)
* Add corresponding rclcpp::shutdown. (`#455 <https://github.com/ros2/system_tests/issues/455>`_)
* Update maintainers. (`#450 <https://github.com/ros2/system_tests/issues/450>`_)
* Contributors: Andrea Sorbini, Chris Lalancette, Jacob Perron, Stephen Brawner

0.9.1 (2020-07-06)
------------------

0.9.0 (2020-06-04)
------------------
* Avoid new deprecations. (`#426 <https://github.com/ros2/system_tests/issues/426>`_)
  * Avoid new deprecations
  * Avoid more deprecations
* Use serilaized message in callback. (`#427 <https://github.com/ros2/system_tests/issues/427>`_)
* Fix CMake warning about using uninitialized variables. (`#425 <https://github.com/ros2/system_tests/issues/425>`_)
* Disabled Connext-CycloneDDS WString tests. (`#421 <https://github.com/ros2/system_tests/issues/421>`_)
* Rename rosidl_generator_c namespace to rosidl_runtime_c. (`#416 <https://github.com/ros2/system_tests/issues/416>`_)
* Add options to selectively ignore single/multi RMW tests. (`#403 <https://github.com/ros2/system_tests/issues/403>`_)
* Code style only: wrap after open parenthesis if not in one line. (`#397 <https://github.com/ros2/system_tests/issues/397>`_)
* Remove ready_fn, and one self.proc_info. (`#391 <https://github.com/ros2/system_tests/issues/391>`_)
* Clean up bounded sequences check. (`#393 <https://github.com/ros2/system_tests/issues/393>`_)
  * Clean up bounded sequences check
  1. Use a macro to extract repeated logic
  2. Check the sequence length so we get a test error message instead of segfaulting.
  * Add missing include
  * Check strings better
  Also, minor changes for clarity
  * Rename EXPECT_RCSTR_EQ -> EXPECT_ROSIDLC_STREQ
* Contributors: Dan Rose, Dirk Thomas, Karsten Knese, Peter Baughman, William Woodall

0.8.0 (2019-11-20)
------------------
* 0.8.0
* Revert "Skip rclcpp__rclpy__rmw_connext_cpp__rmw_fastrtps_cpp tests. (`#382 <https://github.com/ros2/system_tests/issues/382>`_)". (`#383 <https://github.com/ros2/system_tests/issues/383>`_)
  This reverts commit bde886a231ba262b2d8c1e81513b0c8f85e1f3bb.
* Skip rclcpp__rclpy__rmw_connext_cpp__rmw_fastrtps_cpp tests. (`#382 <https://github.com/ros2/system_tests/issues/382>`_)
  These tests fail consistently due to an assertion in Fast-RTPS which was
  recently introduced.
* Fix condition to not skip FastRTPS to FastRTPS pub/sub tests. (`#377 <https://github.com/ros2/system_tests/issues/377>`_)
  * Fix condition to not skip FastRTPS to FastRTPS pub/sub tests
  * Fix and simplify comparison logic
  * Skip WStrings on macOS from FastRTPS to Connext
* Support Arrays.srv in communication tests. (`#376 <https://github.com/ros2/system_tests/issues/376>`_)
* Contributors: Dirk Thomas, Jacob Perron, Michael Carroll, Scott K Logan

0.7.1 (2019-05-29)
------------------

0.7.0 (2019-05-20)
------------------
* Fix memory leaks in test_communication tests. (`#368 <https://github.com/ros2/system_tests/issues/368>`_)
  Fix memory leaks detected by AddressSanitizer in
  test_message_serialization and test_messages_c tests.
* Handle launch_testing assertExitCodes correctly. (`#367 <https://github.com/ros2/system_tests/issues/367>`_)
* Fix deprecation warnings. (`#364 <https://github.com/ros2/system_tests/issues/364>`_)
* Make test_subscriber_cpp always fail gracefully. (`#363 <https://github.com/ros2/system_tests/issues/363>`_)
  * Make test_subscriber_cpp always fail gracefully.
  * Use fprintf to stderr instead of std::cerr.
* Changes to avoid deprecated API's. (`#361 <https://github.com/ros2/system_tests/issues/361>`_)
  * Changes to avoid deprecated API's
  * Review comments
* Corrected publish calls with shared_ptr signature. (`#348 <https://github.com/ros2/system_tests/issues/348>`_)
  * Corrected publish calls with shared_ptr signature
  * Updated with PR comments
  * Correct linter failure
* Fix issues with C messages test. (`#355 <https://github.com/ros2/system_tests/issues/355>`_)
  * Initialize BasicTypes field of Array
  Otherwise, tests may fail if garbage values are used.
  * Correct the number of BoundedSequences messages
  * Add C message tests for Constants.msg, Defaults.msg, and Empty.msg
* Skip cross vendor testing of OpenSplice for WStrings. (`#354 <https://github.com/ros2/system_tests/issues/354>`_)
* Update to be compatible with latest QoS changes. (`#349 <https://github.com/ros2/system_tests/issues/349>`_)
* Add WString tests. (`#353 <https://github.com/ros2/system_tests/issues/353>`_)
* API updates for RMW preallocation work. (`#352 <https://github.com/ros2/system_tests/issues/352>`_)
* Use new interface definitions. (`#350 <https://github.com/ros2/system_tests/issues/350>`_)
  * Replace Primitives with BasicTypes
  * Replace StaticArrayPrimitives with Arrays
  * Replace BoundedArrayPrimitives with BoundedSequences
  * Replace DynamicArrayPrimitives with UnboundedSequences
  * Replace nested sequence and array message types with MultiNested
  * Update FieldsWithSameType.msg
  * Update test_message_serialization.cpp
  * Fix compile issues
  * Add tests for Constants.msg and Defaults.msg
  * Update expected buffer length for serialized message
  * Add test for Strings.msg for C
  * Cleanup
  * Update test_security package to use new interface definitions
  * Use BasicTypes for checking message serialization size
  It seems that BoundedSequences does not have a consistent size between tests.
* Migrate launch tests to new launch_testing features & API. (`#340 <https://github.com/ros2/system_tests/issues/340>`_)
  * Update after launch_testing features becoming legacy.
  * Migrate test_rclcpp tests to new launch_testing API.
  * Migrate test_communication tests to new launch_testing API.
  * Migrate test_security tests to new launch_testing API.
  * Migrate test_cli_remapping tests to new launch_testing API.
  * Stop using injected attributes in launch tests.
  * Bump test_rclcpp tests timeout to please CI.
  * Fix PATH in test_security tests.
  * Bump test_security tests timeout to please CI.
  * Address peer review comments.
  * Please flake8 on test_cli_remapping.
* Update call to async_send_goal. (`#346 <https://github.com/ros2/system_tests/issues/346>`_)
  * Update call to async_send_goal
  It now takes an options struct that contains a reference to the feedback callback.
  * Use action client method for requesting goal result
  Otherwise, an exception is thrown since there was no result callback provided when sending the goal.
* Rename action state transitions. (`#342 <https://github.com/ros2/system_tests/issues/342>`_)
  * Rename action state transitions
  Now using active verbs as described in the design doc:
  http://design.ros2.org/articles/actions.html#goal-states
  Connects to `ros2/rcl#399 <https://github.com/ros2/rcl/issues/399>`_.
* Merge pull request `#339 <https://github.com/ros2/system_tests/issues/339>`_ from ros2/`ivanpauno/ros2#658 <https://github.com/ivanpauno/ros2/issues/658>`_
  Using ament_target_dependencies where possible
* Used ament_target_directories where possible in test_communication CMakeLists
* Refactor test generation. (`#336 <https://github.com/ros2/system_tests/issues/336>`_)
* Update char type mapping, update to use separated action types. (`#315 <https://github.com/ros2/system_tests/issues/315>`_)
  * Update char type mapping
  * Match renamed action types
  * Use correct term
  * Change char type values
  * Make the build pass for now
  * Update action API
  * Update action API
* Add communication tests for Python Actions. (`#333 <https://github.com/ros2/system_tests/issues/333>`_)
* Add launch along with launch_testing as test dependencies. (`#334 <https://github.com/ros2/system_tests/issues/334>`_)
* Drops legacy launch API usage. (`#328 <https://github.com/ros2/system_tests/issues/328>`_)
  * Drops legacy launch API usage.
  * Fixes style issues.
  * Drops more legacy launch API use cases.
  * Adds launch_testing as test_security dependency.
  * Applies misc fixes after Windows triaging.
  * Applies more fixes after Windows triaging.
  * Disables test_rclcpp cross vendor tests on Windows.
* Add test for test_msgs/NestedMessage.action. (`#330 <https://github.com/ros2/system_tests/issues/330>`_)
  * Add test for test_msgs/NestedMessage.action
  * Fixes for cpplint and uncrustify
* Pass context to wait set. (`#324 <https://github.com/ros2/system_tests/issues/324>`_)
* Contributors: Dirk Thomas, Jacob Perron, M. M, Michael Carroll, Michel Hidalgo, Prajakta Gokhale, Shane Loretz, William Woodall, ivanpauno

0.6.0 (2018-12-14)
------------------
* Disable any cross-vendor communication tests for Fast-RTPS. (`#322 <https://github.com/ros2/system_tests/issues/322>`_)
  * Disable any cross-vendor communication tests for Fast-RTPS.
  Builds are actually failing for all cross-vendor tests involving
  rmw_fastrtps_cpp not just those between Connext and Fast-RTPS.
  * Address linter feedback.
* Disable cross vendor tests for pub/sub fastrtps/connext. (`#320 <https://github.com/ros2/system_tests/issues/320>`_)
  * Disable cross vendor tests for pub/sub fastrtps/connext
  * Only skip tests on Windows.
* Add Fibonacci test for actions. (`#316 <https://github.com/ros2/system_tests/issues/316>`_)
  * Add Fibonacci test for actions
  * Fixup test creation
  * Remove debug code
  * Action tests depend on action client and server
  * Static cast to get rid of warning
* Refactor to support init options and context. (`#313 <https://github.com/ros2/system_tests/issues/313>`_)
  * Refactor to support init options and context
  * Fix security tests
  * Pass context to timer api
  * Avoid custom main just for init/shutdown
  * Avoid terminate in ~thread on exceptions
  * Update expected output
  * Add missing fini in test fixture
  * Fixup pub/sub test fixture
* Only consider .msg files with a msg namespace. (`#310 <https://github.com/ros2/system_tests/issues/310>`_)
* Add new fixtures. (`#312 <https://github.com/ros2/system_tests/issues/312>`_)
  * Add new fixtures
  * Fix copy paste error
* Update rcl_wait_set_add_guard_condition() call. (`#311 <https://github.com/ros2/system_tests/issues/311>`_)
  Now the function takes an optional output index argument.
* Merge pull request `#307 <https://github.com/ros2/system_tests/issues/307>`_ from ros2/array-terminology
  rename dynamic array to sequence
* Rename files
* Rename dynamic array to sequence
* Use new error handling API from rcutils. (`#306 <https://github.com/ros2/system_tests/issues/306>`_)
  * Use new error handling API from rcutils
  * Fix some more cases where the new error handling API is used
* Merge pull request `#303 <https://github.com/ros2/system_tests/issues/303>`_ from ros2/hidmic/namespace-messages-with-subfolder
  Handles msg files with the same name in different subfolders
* Prevents tests from being generated for action messages and services.
* Handles msg files with the same name in different subfolders.
* Add new test message type DynamicArrayStaticArrayPrimitivesNested for communcation tests. (`#302 <https://github.com/ros2/system_tests/issues/302>`_)
* Use add_compile_options instead of setting only cxx flags
* Use consolidated rcl_wait_set_clear(). (`#292 <https://github.com/ros2/system_tests/issues/292>`_)
* Remove unused builtin_interfaces dependency. (`#285 <https://github.com/ros2/system_tests/issues/285>`_)
* Only test serialization on fastrtps and connext. (`#284 <https://github.com/ros2/system_tests/issues/284>`_)
  * Only test serialization on fastrtps and connext
  * Use skip_test
  * Reset skip test
* Expose cdr. (`#267 <https://github.com/ros2/system_tests/issues/267>`_)
  * Change to new rclcpp subscription api
  * Uncrustify
  * Add serialization tests
  * Linters
  * Add pub_sub test for raw callbacks
  * Address review comments
  * Warn unused
  * Raw->serialized
  * Use size_t. (`#283 <https://github.com/ros2/system_tests/issues/283>`_)
  * Raw->serialized
  * Use size_t
* Use debug python executable on windows. (`#281 <https://github.com/ros2/system_tests/issues/281>`_)
  * Use debug python executable on windows
  * Get python debug executable from pythonextra
* Migrate launch -> launch.legacy. (`#273 <https://github.com/ros2/system_tests/issues/273>`_)
* Account for null-terminator character. (`#269 <https://github.com/ros2/system_tests/issues/269>`_)
  * Account for null-terminator character
  * Modify tmpstr to be able to represent any size_t value
* Use call_async. (`#257 <https://github.com/ros2/system_tests/issues/257>`_)
* [test_communication] Unique namespaces. (`#256 <https://github.com/ros2/system_tests/issues/256>`_)
  * Add namespace to pubsub tests
  * Add namespace to service tests
  * Uncrustify
  * Use UTC time rather than datetime
  * Single quotes
  * Make arguments mandatory like in C++
* Skip python service tests only for connext dynamic. (`#249 <https://github.com/ros2/system_tests/issues/249>`_)
  * Enable python services for all but connext dynamic
  * Remove changes from 248
  * Up to 20 spins
  * Revert "remove changes from 248"
  This reverts commit 77fc9f4b5e488533dfc8e079178ed17e2f8c288f.
* Reenable service tests: rclcpp requester rclpy replier on Windows. (`#248 <https://github.com/ros2/system_tests/issues/248>`_)
* Contributors: Alexis Pojomovsky, Dirk Thomas, Jacob Perron, Karsten Knese, Michel Hidalgo, Mikael Arguedas, Shane Loretz, Steven! Ragnarök, William Woodall

0.4.0 (2017-12-08)
------------------
* Update for rclcpp namespace removals. (`#255 <https://github.com/ros2/system_tests/issues/255>`_)
  * Remove subscription:: namespace
  * Remove client:: namespace
  * Remove service:: namespace
  * Remove parameter_client:: namespace
  * Remove parameter_service:: namespace
  * Remove timer:: namespace
  * Remove node:: namespace
  * Remove event:: namespace
  * Remove utilities:: namespace
* Merge pull request `#252 <https://github.com/ros2/system_tests/issues/252>`_ from ros2/check_if_test_exists_before_adding_properties
  check if test exists before adding properties
* Check if test exists before adding properties
* Cmake 3.10 compatibility: pass absolute path to file(GENERATE) function. (`#251 <https://github.com/ros2/system_tests/issues/251>`_)
* Wait for service before calling it. (`#244 <https://github.com/ros2/system_tests/issues/244>`_)
  * Wait for service before calling it
  * Wait for a maximum of 15 seconds
  * Refactor to make sure cleanup happens
* Find gtest before macro invocation so that its not find during each macro invocation. (`#246 <https://github.com/ros2/system_tests/issues/246>`_)
* Merge pull request `#245 <https://github.com/ros2/system_tests/issues/245>`_ from ros2/ament_cmake_pytest
  use ament_cmake_pytest instead of ament_cmake_nose
* Use ament_cmake_pytest instead of ament_cmake_nose
* Typo
* Restore bigobj. (`#241 <https://github.com/ros2/system_tests/issues/241>`_)
  * [test_communication] restore bigobj
  * [test_security] restore bigobj
  * Make it explicit that bigobj is needed only in debug mode
* 240 fixups
* Replaces "std::cout<<" with "printf". (`#240 <https://github.com/ros2/system_tests/issues/240>`_)
  * [test_communication]replace uses of iostream
  * [test_rclcpp] remove use of std::cout except flushing
  * Missed some
  * We use float duration not double
  * Remove now unused include
* Merge pull request `#230 <https://github.com/ros2/system_tests/issues/230>`_ from ros2/test_connext_secure
  Test connext secure
* Removing /bigobj flag on windows. (`#239 <https://github.com/ros2/system_tests/issues/239>`_)
* Move security tests in different package
  generate new security files with latest sros2 generation script
* Merge pull request `#236 <https://github.com/ros2/system_tests/issues/236>`_ from ros2/optimize_test_publisher_subscriber
  Minimize the number of calls to message.__repr_\_()
* Minimize the number of calls to message.__repr_\_()
* Merge pull request `#233 <https://github.com/ros2/system_tests/issues/233>`_ from ros2/uncrustify_master
  update style to match latest uncrustify
* N need to tweak python path now that messages come from test_msgs. (`#232 <https://github.com/ros2/system_tests/issues/232>`_)
* Update style to match latest uncrustify
* 0.0.3
* Test msgs. (`#223 <https://github.com/ros2/system_tests/issues/223>`_)
  * Use messages from test_msgs
  * Update tests to use messages from new package
  * Delete unused message files
  * Update service tests as well
  * Revert spurious changes
  * Remove todo but dont change compile options because this package will keep generating it's own messages
  * No need to install isnterfaces anymore
  * Rename message field for DynamicArrayPrimitivesNested
  * Remove spurious line change
  * Iterate over interface files to built list of services and messages
* Update test_messages_c.cpp. (`#226 <https://github.com/ros2/system_tests/issues/226>`_)
  Array initialized with 2 while 3 elements filled, increased size.
* Call rclcpp::shutdown in all tests. (`#225 <https://github.com/ros2/system_tests/issues/225>`_)
* Commenting out unused import for flake8 compliance
* Merge pull request `#222 <https://github.com/ros2/system_tests/issues/222>`_ from ros2/enable_array_tests_opensplice
  reenable array tests with OpenSplice
* Reenable array tests with OpenSplice
* Ensure nodes have called rclcpp::shutdown before exiting. (`#220 <https://github.com/ros2/system_tests/issues/220>`_)
* Use unbuffered Python in launch files. (`#218 <https://github.com/ros2/system_tests/issues/218>`_)
  * Use unbuffered Python in launch files
  * Use unbuffered Python in secure pubsub launch file
* Testing array longers than 101. (`#216 <https://github.com/ros2/system_tests/issues/216>`_)
* Use _WIN32 everywhere. (`#213 <https://github.com/ros2/system_tests/issues/213>`_)
* 0.0.2
* C memleak testing. (`#211 <https://github.com/ros2/system_tests/issues/211>`_)
  * Added nested message that always breaks because of the bug
  * Added C++ code for DynamicArrayPrimitivesNested message
  * Fixed style and publisher/subscriber (combo) test case
  * Expose core dumpes on complex messages
  * Dont run other tests to save debugging time
  * More fixtures, looks like a string array alignment issue
  * Newline at end of file
  * Move include to the right place
  * Add comment about current failing tests
  * Remove debug prints
  * Restore/reenable all tests
  * That was actually pretty readable with vertical space
  * Use all messages fron the fixtures rather the only the first one
  * Linters
  * What's cool with functions is that you can call them rather than copy-n-paste code
* Destroy node before shutdown. (`#210 <https://github.com/ros2/system_tests/issues/210>`_)
* Use CMAKE_X_STANDARD and check compiler rather than platform
* Add option for security tests. (`#208 <https://github.com/ros2/system_tests/issues/208>`_)
* Adding security tests. (`#204 <https://github.com/ros2/system_tests/issues/204>`_)
  * WIP: add security tests
  * Keys, certs and crap used for testing
  * Switching to a multi process test because of https://github.com/eProsima/Fast-RTPS/issues/106
  * Test failing / throwing cases
  * Test only for fastrtps for now
  * Lint
  * Unnused var name
  * WIP
  * Test all message type for regression checking. Also disable should throw examples that will be implemented in a single process in C
  * Update certs/key files
  * Move tests with invalid node creation to single process
  * Add not connecting tests with timer, remove unused args, simplify template logic
  * Remove now useless topic_name parameters
  * Leverage VALID_SECURE_ROOT
  * More cleanup
  * Update copyright year
  * Remove debug prints
  * Remove unused variables
  * Add generated from notice to all test python templates
  * Removing variables is great, code that compiles is better
  * Check for test target existence
  * Rename test suite to match what is being tested
  * Rename security environment variables
  * Trailing whitespace
* Destroy node before shutdown. (`#207 <https://github.com/ros2/system_tests/issues/207>`_)
* Merge pull request `#205 <https://github.com/ros2/system_tests/issues/205>`_ from ros2/move_time
  remove unnecessary usage of RCL_S_TO_NS
* Remove unnecessary usage of RCL_S_TO_NS
* Remove unnecessary topic name check. (`#203 <https://github.com/ros2/system_tests/issues/203>`_)
  * Remove incorrect and unnecessary topic name check
  * Up timeout for slow test
* Set_tests_properties for correct requester replier executable. (`#202 <https://github.com/ros2/system_tests/issues/202>`_)
* Support addition of node namespace in rclcpp API. (`#196 <https://github.com/ros2/system_tests/issues/196>`_)
* Merge pull request `#199 <https://github.com/ros2/system_tests/issues/199>`_ from ros2/use_explicit_kwargs
  use explicit kwargs
* Use explicit kwargs
* Add missing exec dep on builtin_interfaces. (`#198 <https://github.com/ros2/system_tests/issues/198>`_)
  * Add missing exec dep on builtin_interfaces
  * Alphabetically is better
* Fix deps. (`#192 <https://github.com/ros2/system_tests/issues/192>`_)
  * Every day I'm reshuffling
  * Auto
* Install msgs and fixtures for use by other packages. (`#190 <https://github.com/ros2/system_tests/issues/190>`_)
  * Install msgs and fixtures for use by other packages
  * Reshuffle depends
  * Reshuffle depends
* Use -Wpedantic. (`#189 <https://github.com/ros2/system_tests/issues/189>`_)
  * Add pedantic flag
  * Fix pedantic warning
  * Fix C4456 warning
  * Reduce scope of wait_sets
  * Reduce scope rather than renaming variable
* Comply with flake8 + flake-import-order. (`#188 <https://github.com/ros2/system_tests/issues/188>`_)
* Merge pull request `#187 <https://github.com/ros2/system_tests/issues/187>`_ from ros2/use_rmw_impl
  use rmw implementation
* Remove usage of RCLPY_IMPLEMENTATION
* Use rmw implementation
* Merge pull request `#186 <https://github.com/ros2/system_tests/issues/186>`_ from ros2/typesupport_c_reloaded
  use rosidl_typesupport_c
* Use rosidl_typesupport_c
* Replace deprecated <CONFIGURATION> with <CONFIG>
* Use new rclcpp::literals namespace + constness issue fix. (`#178 <https://github.com/ros2/system_tests/issues/178>`_)
  * Use new rclcpp::literals namespace
  * Test_subscription.cpp: fix missing 'const'
  wait_for_future() required a non-const reference but
  at the callers are using user-defined literals such as 10_s,
  which aren't lvalue.
  * Add NOLINT to 'using namespace rclcpp::literals'
  * Use std::chrono_literals
* C++14. (`#181 <https://github.com/ros2/system_tests/issues/181>`_)
* Rclpy tests match rclcpp timing. (`#183 <https://github.com/ros2/system_tests/issues/183>`_)
* Merge pull request `#180 <https://github.com/ros2/system_tests/issues/180>`_ from ros2/typesupport_reloaded
  append build space to library path
* Test loong strings for services. (`#179 <https://github.com/ros2/system_tests/issues/179>`_)
* Append build space to library path
* Mark blacklisted tests as skipped. (`#177 <https://github.com/ros2/system_tests/issues/177>`_)
  * Skip opensplice failing tests
  * Use new SKIP_TEST arg rather than hacking templates
  * Lint cmake
  * Remove unnecessary args
  * Use _SKIP_TEST variable everywhere
  * Rename _SKIP_TEST to SKIP_TEST
  * Indent cmake
* Test python services. (`#175 <https://github.com/ros2/system_tests/issues/175>`_)
  * Extend service template to test python services
  * Trailing whitespace
  * Skipping tests raising SkipTest
  * Remove SKIP_TEST for non nose tests
  * Add bracket because linter doesnt understand multiline conditions
* Remove unnecessary ament_index_build_path. (`#174 <https://github.com/ros2/system_tests/issues/174>`_)
* Use generator for target file location. (`#173 <https://github.com/ros2/system_tests/issues/173>`_)
  * Use generator for target file location
  * Remove unused variable
* Add a bunch of tests for rcl and rosidl_generator_c messages. (`#122 <https://github.com/ros2/system_tests/issues/122>`_)
  * Rcl tests for rosidl_generator_c and c type support
  * Add test source file
  * Don't need assignn
  * Don't ignore fastrtps
  * Test all message types
  * Init messages with default values
  * Increase test timeout
  * Update fixtures
  * Reuse primitive message verify function
  * No need for executables here
  * Add waitset
  * Increase string length
  * Proper graph guard condition
* Merge pull request `#172 <https://github.com/ros2/system_tests/issues/172>`_ from ros2/fix_pyflakes
  fix pyflakes
* Fix pyflakes
* Test cross RCL communication. (`#152 <https://github.com/ros2/system_tests/issues/152>`_)
  * Unify templates and configure them in a macro
  * Remove unnecessary logic
  * Reenable single process tests
  * Refactor template parameters
  * Reenable service testing across rmw
  * String compare
  * Wrap blacklist tests condition
  * Clean comments
  * Reenable failing connext_dynamic StaticArrayNested test
  * Remove env variable check
  * Rename rcl variable to client_library(ies)
  * Rename macro
* Merge pull request `#171 <https://github.com/ros2/system_tests/issues/171>`_ from ros2/rosidl_target_interfaces_add_dependency
  remove obsolete add_dependencies
* Remove obsolete add_dependencies
* Support local graph changes in Connext. (`#164 <https://github.com/ros2/system_tests/issues/164>`_)
  * Remove blocks and workarounds on service tests
  * Remove no longer needed sleep
  * Remove blocks and workarounds on new service test
  * Replace busy wait with graph event wait
  * Use new non-busy wait
  * [style] uncrustify and cpplint
  * Increase timeout for test_services
  timeout was 30s, but it is consistently taking
  34s for me
  * Update wait_for_subscriber to also wait for it to be gone
  * Deduplicate code and allow retried publishing
  * Increase timeout for test_rclcpp/test_subscription to 60s
  * Comment cleanup
  * Fix typo
* Fixed tests after pull request `ros2/rclcpp#261 <https://github.com/ros2/rclcpp/issues/261>`_. (`#170 <https://github.com/ros2/system_tests/issues/170>`_)
* Merge pull request `#168 <https://github.com/ros2/system_tests/issues/168>`_ from ros2/looong_strings
  tests strings > 256
* Tests strings > 256
* Merge pull request `#166 <https://github.com/ros2/system_tests/issues/166>`_ from ros2/fix_cpplint
  comply with stricter cpplint rules
* Comply with stricter cpplint rules
* Increase max spin count to handle fastrtps different spin behaviour
* Enable fastrtps python tests
* Ascii fixture. (`#161 <https://github.com/ros2/system_tests/issues/161>`_)
  * Use ASCII value for char
  * Homogenize BoundedArrayPrimitives fixtures
* Merge pull request `#148 <https://github.com/ros2/system_tests/issues/148>`_ from ros2/remove_noop
  remove noops
* Remove noops
* Merge pull request `#147 <https://github.com/ros2/system_tests/issues/147>`_ from ros2/fix_more_tests
  remove duplicates of test_subscription_valid_data_cpp, fix skipped tests on Windows
* Remove duplicates of test_subscription_valid_data_cpp, fix skipped tests on Windows
* Merge pull request `#146 <https://github.com/ros2/system_tests/issues/146>`_ from ros2/revert_test_requester_timing
  revert test requester timing
* Revert test requester timing
* Update schema url
* Merge pull request `#145 <https://github.com/ros2/system_tests/issues/145>`_ from ros2/sleep_if_not_wait_for_service
  use sleep if wait_for_service throws
* Use sleep if wait_for_service throws
* Add schema to manifest files
* Merge pull request `#142 <https://github.com/ros2/system_tests/issues/142>`_ from ros2/bounded_vector
  add tests for bounded vectors
* Merge pull request `#144 <https://github.com/ros2/system_tests/issues/144>`_ from ros2/update_test_times
  update test times
* Support bounded vectors
* Add communication tests for bounded arrays
* Update test times
* Use wait_for_service to make Service tests less flaky. (`#132 <https://github.com/ros2/system_tests/issues/132>`_)
  * Use wait_for_service to make tests less flaky
  * Realign timeouts
  * Avoid using wait_for_service with fastrtps
  this can be undone once fastrtps supports wait_for_service
  * [test_communication] avoid wait_for_service with fastrtps
  it can be undone once fastrtps supports wait_for_service
  * Add test to ensure wait_for_service wakes after shutdown/sigint
* Windows python debug. (`#138 <https://github.com/ros2/system_tests/issues/138>`_)
  * Pass python interpreter to nose test
  * Rename interpreter to executable
  * Rename PYTHON_DBG_EXECUTABLE to PYTHON_EXECUTABLE_DEBUG
  * Cmake3.5 remove variable expansion
* Add tests for all message_files. (`#125 <https://github.com/ros2/system_tests/issues/125>`_)
  * Add tests for all messages
  * Do not run opensplice failing test
  * Use cmake3.5 syntax, fixed rmw_implementation variable
* Dont assert type support during import. (`#141 <https://github.com/ros2/system_tests/issues/141>`_)
* Wrap complex condition
* Merge pull request `#136 <https://github.com/ros2/system_tests/issues/136>`_ from ros2/cmake35
  require CMake 3.5
* Remove trailing spaces from comparisons, obsolete quotes and explicit variable expansion
* Require CMake 3.5
* Linting
* Fix string comparison cmake
* Merge pull request `#121 <https://github.com/ros2/system_tests/issues/121>`_ from ros2/add_rclpy_talker_listener_to_test_communication
  add tests for rclpy talker listener
* Check the rmw id matches in cross-vendor tests. (`#126 <https://github.com/ros2/system_tests/issues/126>`_)
* Rclpy from install folder
* Revert cpp tests addition, handled by `#125 <https://github.com/ros2/system_tests/issues/125>`_
* Cleanup
* Added todo for rmw blacklist in cmake
* Reenable cpp tests
* Use camelcase format for message names
* Already enforced by argparse
* Move testing in callback to match cpp tests
* Extend default duration to allow missed messages
* Removed artefacts from poor rebase, blacklist failing opensplice dynamicarrayprimitives
* Disable non python tests for testing on the farm
* Test python for all message types
* Add libs for windows
* Fix assert condition and pep8
* Use utf-8 compatible values for char testing
* Enable test for all msg files
* Testing values cross rcl communication
* Multiple rmw_implementation
* Macro
* Use fixture, test received message
* Add python message fixtures
* Add comment for system path inserts
* Add rclpy directory to system path
* Add rclpy as test dependency
* Homogenize fiels assignment for testing
* Fixed byte/char array assignation
* Added back char and byte now that somehow fixed on python generator side
  Conflicts:
  test_communication/test/message_fixtures.hpp
* Working towards testing python communication along cpp one
  Conflicts:
  test_communication/test/message_fixtures.hpp
* Merge pull request `#119 <https://github.com/ros2/system_tests/issues/119>`_ from ros2/fix_tests
  fix generation of tests with multiple executables
* Disable cross vendor services for FastRTPS
* Disable tests failing due to OpenSplice bug
* Merge pull request `#128 <https://github.com/ros2/system_tests/issues/128>`_ from ros2/curly_brackets
  fix use of brackets
* Fix brackets: see if Clang or Windows complains
* Remove RCL_ASSERT_RMW_ID_MATCHES for multi target tests
* Fix generation of tests with multiple executables
* Merge pull request `#127 <https://github.com/ros2/system_tests/issues/127>`_ from ros2/float_fixture_values
  use values that fit in a float for testing
* Use values that fit in a float for testing
* Fix spelling in comment
* Merge pull request `#120 <https://github.com/ros2/system_tests/issues/120>`_ from dhood/test-linking-runtime
  Ensure using correct rmw implementation in tests
* Use RCL_ASSERT_RMW_ID_MATCHES to ensure correct rmw implementation is being used
* Move message registration
* Merge pull request `#118 <https://github.com/ros2/system_tests/issues/118>`_ from ros2/rclcpp219
  extend test to cover another case
* Merge pull request `#117 <https://github.com/ros2/system_tests/issues/117>`_ from ros2/msg_with_fields_with_same_type
  add message which has fields with the same non-primitive type
* Extend test to cover `ros2/rclcpp#219 <https://github.com/ros2/rclcpp/issues/219>`_
* Add message which has fields with the same non-primitive type
* Merge pull request `#115 <https://github.com/ros2/system_tests/issues/115>`_ from ros2/ctest_build_testing
  use CTest BUILD_TESTING
* Get only C++ typesupport implementations. (`#114 <https://github.com/ros2/system_tests/issues/114>`_)
  * Get only C++ typesupport implementations
  * Add busy_wait_for_subscriber to make publisher test unflaky
* Use CTest BUILD_TESTING
* Use rcl. (`#113 <https://github.com/ros2/system_tests/issues/113>`_)
  * Init is required now
  * Fix multiple init calls
  * Add init to a test, increase timeout and change an assertion to an expectation
  * Fix argc/argv
  * Wait for subscriber in publisher test
* Merge pull request `#105 <https://github.com/ros2/system_tests/issues/105>`_ from ros2/generator_expression
  use generator expressions for configuration specific tests
* Use generator expressions for configuration specific tests
* Merge pull request `#102 <https://github.com/ros2/system_tests/issues/102>`_ from ros2/rename_message_type_support
  support multiple type supports per rmw impl
* Support multiple type supports per rmw impl
* Merge pull request `#101 <https://github.com/ros2/system_tests/issues/101>`_ from ros2/windows_release
  build release on Windows
* Build release on Windows
* Merge pull request `#86 <https://github.com/ros2/system_tests/issues/86>`_ from ros2/refactor_typesupport
  use new approach to generate rmw implementation specific targets
* Use new approach to generate rmw implementation specific targets
* Merge pull request `#83 <https://github.com/ros2/system_tests/issues/83>`_ from ros2/missing_dep
  add missing dependency on rmw_implementation_cmake
* Add missing dependency on rmw_implementation_cmake
* Merge pull request `#59 <https://github.com/ros2/system_tests/issues/59>`_ from ros2/cpplint
  update code to pass ament_cpplint
* Update code to pass ament_cpplint
* Merge pull request `#58 <https://github.com/ros2/system_tests/issues/58>`_ from ros2/optional-qos-profile
  Made rmw_qos_profile argument optional
* Made rmw_qos_profile argument optional
* Merge pull request `#42 <https://github.com/ros2/system_tests/issues/42>`_ from ros2/test-services
  Added tests for services
* Added tests for services
* Merge pull request `#51 <https://github.com/ros2/system_tests/issues/51>`_ from ros2/issue_50
  disregard duplicate requests
* Merge pull request `#52 <https://github.com/ros2/system_tests/issues/52>`_ from ros2/reduce_test_times
  Reduce test times
* Remove all references to received_messages
* Rename rate variables
* Reduce test times
* Make duplicate requests just a warning not a failure
  fixes `#50 <https://github.com/ros2/system_tests/issues/50>`_
* Merge pull request `#34 <https://github.com/ros2/system_tests/issues/34>`_ from ros2/wrong_service_callback
  update test to catch repeated service callbacks
* Merge pull request `#35 <https://github.com/ros2/system_tests/issues/35>`_ from ros2/rmw_gid_support
  update intra proc tests with different assumptions
* Update intra proc tests with different assumptions
* Update test to catch repeated service callbacks
* Merge pull request `#17 <https://github.com/ros2/system_tests/issues/17>`_ from ros2/unbounded
  add fixtures with longer dynamic content
* Add fixtures with a string with more then 255 characted and more than 100 elements in a sequence
* Merge pull request `#27 <https://github.com/ros2/system_tests/issues/27>`_ from ros2/check_sample_valid_data
  add test to check for receiving callbacks for invalid data
* Add test to check for receiving callbacks for invalid data
* Merge pull request `#24 <https://github.com/ros2/system_tests/issues/24>`_ from ros2/qos
  Added support for QoS profiles
* Added support for QoS profiles
* Use linters
* [style] limit line length to 100 chars.
* Merge pull request `#21 <https://github.com/ros2/system_tests/issues/21>`_ from ros2/fix_more_windows_warnings
  fix more windows warnings
* Fix more windows warnings
* Add explicit build type
* Merge pull request `#18 <https://github.com/ros2/system_tests/issues/18>`_ from ros2/raise_warning_level
  raise warning level
* Raise warning level
* Merge pull request `#15 <https://github.com/ros2/system_tests/issues/15>`_ from ros2/test_array_submsgs
  add test to cover messages with an array of sub messages
* Add test to cover messages with a static array of sub messages
* Improve error messages
* Remove package name prefix
* Add test to cover messages with an array of sub messages
* Fix generation of test results for successful tests
* Merge pull request `#13 <https://github.com/ros2/system_tests/issues/13>`_ from ros2/single_process_pub_sub
  add tests for publish/subscribe in a single process
* Add tests for publish/subscribe in a single process
* Merge pull request `#12 <https://github.com/ros2/system_tests/issues/12>`_ from ros2/refactor_examples_and_interfaces
  changes to support renaming of interface packages
* Changes to support renaming of interface packages
* Merge pull request `#11 <https://github.com/ros2/system_tests/issues/11>`_ from ros2/update_message_api
  update message API
* Update message API
* Merge pull request `#6 <https://github.com/ros2/system_tests/issues/6>`_ from ros2/wjwwood_warnings_cleanup
  adjust use of braces to fix warnings with clang
* Adjust use of braces to fix warnings with clang
* Merge pull request `#9 <https://github.com/ros2/system_tests/issues/9>`_ from ros2/fix_narrowing_conversion_error_windows
  fix narrowing conversion error on windows
* Fix narrowing conversion error on windows
* Merge pull request `#5 <https://github.com/ros2/system_tests/issues/5>`_ from ros2/refactor_msg_gen
  refactor message generation
* Refactor message generation. (`ros2/ros2#48 <https://github.com/ros2/ros2/issues/48>`_)
* Merge pull request `#4 <https://github.com/ros2/system_tests/issues/4>`_ from ros2/test_nested
  add tests for nested messages
* Add tests for builtin messages
* Add tests for nested messages
* Merge pull request `#3 <https://github.com/ros2/system_tests/issues/3>`_ from ros2/dynamic_arrays
  add test for messages with dynamic arrays
* Use double curly braces on vector init lists to work on Windows
* Add test for messages with dynamic arrays
* Merge pull request `#2 <https://github.com/ros2/system_tests/issues/2>`_ from ros2/static_arrays
  add test for messages with static arrays
* Add test for messages with static arrays
* Remove obsolete comments
* Merge pull request `#1 <https://github.com/ros2/system_tests/issues/1>`_ from ros2/first_tests
  add generic tests for pub/sub and req/rep, add two message and service types for now
* Add generic tests for pub/sub and req/rep, add two message and service types for now
* Contributors: Dirk Thomas, Esteve Fernandez, Guillaume Papin, Jackie Kay, Mikael Arguedas, Morgan Quigley, Rafał Kozik, Shane Loretz, William Woodall, dhood, gerkey, michielb
