^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_rclcpp
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.4 (2023-11-21)
-------------------

0.12.3 (2022-04-05)
-------------------

0.12.2 (2022-03-28)
-------------------

0.12.1 (2022-01-13)
-------------------
* Fix include order for cpplint (`#493 <https://github.com/ros2/system_tests/issues/493>`_)
* Fix test (`#488 <https://github.com/ros2/system_tests/issues/488>`_)
* Contributors: Jacob Perron, Mauro Passerino

0.12.0 (2021-11-18)
-------------------
* Updated maintainers (`#489 <https://github.com/ros2/system_tests/issues/489>`_)
* Add tests for rclcpp sigterm handler (`#485 <https://github.com/ros2/system_tests/issues/485>`_)
* Fix deprecated subscriber callback warnings (`#483 <https://github.com/ros2/system_tests/issues/483>`_)
* Fix deprecation warnings and failures after client API update (`#482 <https://github.com/ros2/system_tests/issues/482>`_)
* Use rosidl_get_typesupport_target() (`#480 <https://github.com/ros2/system_tests/issues/480>`_)
* Use rcpputils/scope_exit.hpp instead of rclcpp/scope_exit.hpp (`#479 <https://github.com/ros2/system_tests/issues/479>`_)
* Add test for defered service callback signature (`#478 <https://github.com/ros2/system_tests/issues/478>`_)
* Add changelogs (`#473 <https://github.com/ros2/system_tests/issues/473>`_)
* Contributors: Abrar Rahman Protyasha, Aditya Pande, Christophe Bedard, Ivan Santiago Paunovic, Shane Loretz

0.11.1 (2021-04-26)
-------------------

0.11.0 (2021-04-06)
-------------------
* Reenable test that used to be flaky. (`#467 <https://github.com/ros2/system_tests/issues/467>`_)
* Get_parameters_service\_ should return empty if allow_undeclared\_ is false. (`#466 <https://github.com/ros2/system_tests/issues/466>`_)
* Contributors: Michel Hidalgo, Tomoya Fujita

0.10.0 (2021-03-18)
-------------------
* Make test pass after rclcpp`#1532 <https://github.com/ros2/system_tests/issues/1532>`_. (`#465 <https://github.com/ros2/system_tests/issues/465>`_)
* Adapt tests to statically typed parameters. (`#462 <https://github.com/ros2/system_tests/issues/462>`_)
* Guard against TOCTTOU with rclcpp::ok and rclcpp:spin_some. (`#459 <https://github.com/ros2/system_tests/issues/459>`_)
* Update parameter client test with timeout. (`#457 <https://github.com/ros2/system_tests/issues/457>`_)
* Call rclcpp::init and rclcpp::shutdown in each test for test_rclcpp. (`#454 <https://github.com/ros2/system_tests/issues/454>`_)
* Set cppcheck timeout to 400 seconds. (`#453 <https://github.com/ros2/system_tests/issues/453>`_)
* Modify to match Waitable interface adding take_data. (`#444 <https://github.com/ros2/system_tests/issues/444>`_)
* Update maintainers. (`#450 <https://github.com/ros2/system_tests/issues/450>`_)
* Contributors: Audrow Nash, Ivan Santiago Paunovic, Jacob Perron, Shane Loretz, Stephen Brawner, tomoya

0.9.1 (2020-07-06)
------------------
* Fix rclcpp timeout subscriber test. (`#440 <https://github.com/ros2/system_tests/issues/440>`_)
  * Use nonzero lower bound for timeout checks.
  * Relax time tolerance.
* Show numbers of nanseconds in EXPECT with durations. (`#438 <https://github.com/ros2/system_tests/issues/438>`_)
  * Show numbers of nanseconds in expect with durations
  * Fix syntax
* Remove ament_pytest dependency from test_rclcpp. (`#437 <https://github.com/ros2/system_tests/issues/437>`_)
  It is not used in test_rclcpp anywhere.
* Contributors: Chris Lalancette, Dirk Thomas, Michel Hidalgo

0.9.0 (2020-06-04)
------------------
* Merge pull request `#431 <https://github.com/ros2/system_tests/issues/431>`_ from ros2/disable_flaky_parameter_test
  disable flakey test
* Disable flakey test
  Tracked at: https://github.com/ros2/rmw_cyclonedds/issues/183
* Make `test_executor.spin_some_max_duration` more reliable. (`#430 <https://github.com/ros2/system_tests/issues/430>`_)
* Change which node name cross-vendor tests are enabled. (`#428 <https://github.com/ros2/system_tests/issues/428>`_)
  * Change which rmws do cross node name tests
  Disable connext cross-vendor since it still uses 1 participant per node
  Enable Fast-RTPS and Cyclone cross vendor
  * Mark tests as skipped instead of not generating
  Generate all cross-vendor tests
  Whitelist which cross-vendor tests are expected to work
  * Whitespace
* Send output of test_rclcpp tests to screen. (`#419 <https://github.com/ros2/system_tests/issues/419>`_)
  This makes it easier to determine which tests are failing as we can see the gtest output.
* Avoid new deprecations. (`#426 <https://github.com/ros2/system_tests/issues/426>`_)
  * Avoid new deprecations
  * Avoid more deprecations
* Updates since changes to message_info in rclcpp. (`#423 <https://github.com/ros2/system_tests/issues/423>`_)
* Update the expected exception thrown when getting an invalid parameter type. (`#411 <https://github.com/ros2/system_tests/issues/411>`_)
  The type of the exception was changed in https://github.com/ros2/rclcpp/pull/1027
* Skip failing cross vendor tests after Fast-RTPS based rmw implementations are using one Participant per Context
* Setting AMENT_CMAKE_CPPCHECK_ADDITIONAL_INCLUDE_DIRS with rclcpp. (`#400 <https://github.com/ros2/system_tests/issues/400>`_)
* Specify stdout as the stream to look at. (`#398 <https://github.com/ros2/system_tests/issues/398>`_)
  * Specify stdout as the stream to look at.
* Code style only: wrap after open parenthesis if not in one line. (`#397 <https://github.com/ros2/system_tests/issues/397>`_)
* Remove ready_fn, and one self.proc_info. (`#391 <https://github.com/ros2/system_tests/issues/391>`_)
* Modifying test for failing on getting an empty node name. (`#374 <https://github.com/ros2/system_tests/issues/374>`_)
  * Modifing test for failing on getting an empty nodename
  * Outputing specific msg for empty names
* Set log format for test executables. (`#395 <https://github.com/ros2/system_tests/issues/395>`_)
  In order to prevent tests breaking when the default logging format
  changes, let's set an explicit log format for these test executables.
* Adjusted spin_some test due to new behavior. (`#394 <https://github.com/ros2/system_tests/issues/394>`_)
  Previously would constantly trigger based on a 0ms delay timer;
  now only evaluates a single timer once per spin_some call. Test
  now adds multiple timers with a short delay to simulate performing
  work.
  Relies on behavior change from `ros2/rclcpp#844 <https://github.com/ros2/rclcpp/issues/844>`_, addressing
  `ros2/rclcpp#471 <https://github.com/ros2/rclcpp/issues/471>`_
  Distribution Statement A; OPSEC `#2893 <https://github.com/ros2/system_tests/issues/2893>`_
* Contributors: CaptainTrunky, Chris Lalancette, Dirk Thomas, Ivan Santiago Paunovic, Jacob Perron, Peter Baughman, Shane Loretz, Steven! Ragnarök, Tully Foote, William Woodall, brawner, roger-strain

0.8.0 (2019-11-20)
------------------
* 0.8.0
* Bump test timeouts in test_rclcpp. (`#392 <https://github.com/ros2/system_tests/issues/392>`_)
* Don't set ignore_local_publications = true. (`#388 <https://github.com/ros2/system_tests/issues/388>`_)
  intra_process still needs local pub and sub to send msg.
* Remove tests for now removed, previously deprecated, features. (`#386 <https://github.com/ros2/system_tests/issues/386>`_)
* Fix some comparisons with a sign mismatch. (`#373 <https://github.com/ros2/system_tests/issues/373>`_)
* Contributors: ChenYing Kuo, Michael Carroll, Michel Hidalgo, Scott K Logan, William Woodall

0.7.1 (2019-05-29)
------------------

0.7.0 (2019-05-20)
------------------
* Improve tests for parameters. (`#366 <https://github.com/ros2/system_tests/issues/366>`_)
  * Improve parameter tests
  * Add parameters service tests for failures due to undeclared parameters
  * Refactor parameter test fixtures
  Renamed functions for consistency and reduced code-smell
  * Increase SIGTERM timeout in launch test
  * Increase launch test shutdown timeout
* Handle launch_testing assertExitCodes correctly. (`#367 <https://github.com/ros2/system_tests/issues/367>`_)
* Changes to avoid deprecated API's. (`#361 <https://github.com/ros2/system_tests/issues/361>`_)
  * Changes to avoid deprecated API's
  * Review comments
* Corrected publish calls with shared_ptr signature. (`#348 <https://github.com/ros2/system_tests/issues/348>`_)
  * Corrected publish calls with shared_ptr signature
  * Updated with PR comments
  * Correct linter failure
* Merge pull request `#357 <https://github.com/ros2/system_tests/issues/357>`_ from ros2/ros2_658_leftovers
* Corrected CMakelists
* [WIP] Modify get_node_names to return fully qualified names. (`#345 <https://github.com/ros2/system_tests/issues/345>`_)
  * Get_node_names now returns qualified names
  Amended the tests to reflect this
  * Auto stash before rebase of "jhdcs/master"
  * Corrected system tests for compatibility with qualified node names
  * Removed debug strings, use get_node_names()
  * Modifying what to look for in tests...
  * Cast c-string to std::string. Append slash to start
  * Undo changes to gitignore
  * Removed undesirable print statements
  * Remove unwanted includes
  * Removed more unwanted includes
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
* Read only parameters. (`#278 <https://github.com/ros2/system_tests/issues/278>`_)
  * Expect declared parameters + use_sim_time
  * Replace create_parameter with declare_parameter
  * Expect declared parameters + use_sim_time
  * Replace create_parameter with declare_parameter
  * Fixup node constructor now that we have NodeOptions
  * Cleanup test_parameters_server.cpp
  * Silence warnings for tests of deprecated methods
  * Remove redundant test (now lives in rclcpp's test_node.cpp)
  * Fixup tests
  * Extend deprecation warning suppression to support Windows too
  * Use option to allow declaring of parameters via yaml file for test
* Correct initialization of rmw_qos_profile_t struct instances. (`#344 <https://github.com/ros2/system_tests/issues/344>`_)
* Move away from deprecated rclcpp APIs. (`#343 <https://github.com/ros2/system_tests/issues/343>`_)
* Add launch along with launch_testing as test dependencies. (`#334 <https://github.com/ros2/system_tests/issues/334>`_)
* Drops legacy launch API usage. (`#328 <https://github.com/ros2/system_tests/issues/328>`_)
  * Drops legacy launch API usage.
  * Fixes style issues.
  * Drops more legacy launch API use cases.
  * Adds launch_testing as test_security dependency.
  * Applies misc fixes after Windows triaging.
  * Applies more fixes after Windows triaging.
  * Disables test_rclcpp cross vendor tests on Windows.
* Add in a test to ensure that 10 nodes can launch simultaneously. (`#327 <https://github.com/ros2/system_tests/issues/327>`_)
  * Add in a test to ensure that 10 nodes can launch simultaneously.
  * Make test_ten_nodes more generic.
  That is, allow it to be any number of nodes.  This involves:
  1.  Renaming to test_n_nodes
  2.  Changing the checking node to take a parameter
  3.  Changing the CMakeLists.txt to substitute in the number of nodes
  * Fix review feedback.
* Update for NodeOptions Node constructor. (`#329 <https://github.com/ros2/system_tests/issues/329>`_)
  * [test_rclcpp] Updates for NodeOptions.
  * Wrap long line.
* Remove unnecessary semicolon. (`#326 <https://github.com/ros2/system_tests/issues/326>`_)
* Ignore RCLError during Node constructor. (`#325 <https://github.com/ros2/system_tests/issues/325>`_)
  * Ignore RCLError during Node constructor
  * Print out error message
* Contributors: Chris Lalancette, Emerson Knapp, Jacob Perron, M. M, Michael Carroll, Michel Hidalgo, Shane Loretz, William Woodall, ivanpauno, jhdcs

0.6.0 (2018-12-14)
------------------
* Refactor to support init options and context. (`#313 <https://github.com/ros2/system_tests/issues/313>`_)
  * Refactor to support init options and context
  * Fix security tests
  * Pass context to timer api
  * Avoid custom main just for init/shutdown
  * Avoid terminate in ~thread on exceptions
  * Update expected output
  * Add missing fini in test fixture
  * Fixup pub/sub test fixture
* Add test for waitable. (`#314 <https://github.com/ros2/system_tests/issues/314>`_)
  * Add test for waitable
  * Fix is_ready
  * Fix linter issues
  * Remove visibility macros from testing helper class.
* Don't use %zd when printing an int64.
  The %zd specifier to printf is meant to be used when printing
  out a size_t; but in these tests, the return value (sum) is
  actually an int64_t.  MacOS High Sierra is complaining about
  these, so switch to a PRId64, which is the only thing that
  works cross-platform.
* Don't capture variables that aren't needed.
  This fixes warnings when compiling on MacOS (High Sierra),
  complaining that the captured variable(s) in the lambda is not
  being used.
* Use add_compile_options instead of setting only cxx flags
* Add in a test for the new get_parameter_or_set_default API. (`#296 <https://github.com/ros2/system_tests/issues/296>`_)
  * Add in a test for the new get_parameter_or_set_default API.
  * Switch to using get_parameter_or_set_default.
  * Rename get_parameter_or_set_default -> get_parameter_or_set
* Add test for spin_some(max_duration). (`#299 <https://github.com/ros2/system_tests/issues/299>`_)
* Fix indentation to comply with uncrusity 0.67. (`#286 <https://github.com/ros2/system_tests/issues/286>`_)
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
* Add test for set parameters atomically. (`#277 <https://github.com/ros2/system_tests/issues/277>`_)
* Get parameters that aren't set. (`#276 <https://github.com/ros2/system_tests/issues/276>`_)
* Initial values to node constructor. (`#272 <https://github.com/ros2/system_tests/issues/272>`_)
* Migrate launch -> launch.legacy. (`#273 <https://github.com/ros2/system_tests/issues/273>`_)
* Split ParameterVariant. (`#271 <https://github.com/ros2/system_tests/issues/271>`_)
  * ParameterTypeException
  * Rclcpp::parameter::ParameterVariant -> rclcpp::Parameter
* Parameter services automatically start. (`#270 <https://github.com/ros2/system_tests/issues/270>`_)
* Add cli args to Node constructor. (`#262 <https://github.com/ros2/system_tests/issues/262>`_)
* Prefix node names with a dash to separate it from the empty line separating the results from separate queries
* Add unit test to check for node names across rmw impl. (`#260 <https://github.com/ros2/system_tests/issues/260>`_)
* Increased timeout for tests with multiple wait_for_service. (`#259 <https://github.com/ros2/system_tests/issues/259>`_)
* Update style. (`#258 <https://github.com/ros2/system_tests/issues/258>`_)
* Contributors: Chris Lalancette, Dirk Thomas, Karsten Knese, Mikael Arguedas, Shane Loretz, William Woodall, dhood

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
* Regression test for set_parameters with bad callback reference. (`#253 <https://github.com/ros2/system_tests/issues/253>`_)
  * Add regression test for set_parameters with callback
  * Make it like the parameter_events_async node to excercise the bad reference
  * Create paramters_client\_ in constructor of node subclass
  Possible since https://github.com/ros2/rclcpp/pull/413
* Add regression test for recursive service calls. (`#254 <https://github.com/ros2/system_tests/issues/254>`_)
* Merge pull request `#252 <https://github.com/ros2/system_tests/issues/252>`_ from ros2/check_if_test_exists_before_adding_properties
  check if test exists before adding properties
* Check if test exists before adding properties
* Cmake 3.10 compatibility: pass absolute path to file(GENERATE) function. (`#251 <https://github.com/ros2/system_tests/issues/251>`_)
* Merge pull request `#245 <https://github.com/ros2/system_tests/issues/245>`_ from ros2/ament_cmake_pytest
  use ament_cmake_pytest instead of ament_cmake_nose
* Use ament_cmake_pytest instead of ament_cmake_nose
* 240 fixups
* Replaces "std::cout<<" with "printf". (`#240 <https://github.com/ros2/system_tests/issues/240>`_)
  * [test_communication]replace uses of iostream
  * [test_rclcpp] remove use of std::cout except flushing
  * Missed some
  * We use float duration not double
  * Remove now unused include
* Removing /bigobj flag on windows. (`#239 <https://github.com/ros2/system_tests/issues/239>`_)
* Increase remote parameter test to 60. (`#235 <https://github.com/ros2/system_tests/issues/235>`_)
* Add test for sync parameter_client. (`#231 <https://github.com/ros2/system_tests/issues/231>`_)
* Merge pull request `#234 <https://github.com/ros2/system_tests/issues/234>`_ from ros2/remove_indent_off
  remove obsolete INDENT-OFF usage
* Merge pull request `#233 <https://github.com/ros2/system_tests/issues/233>`_ from ros2/uncrustify_master
  update style to match latest uncrustify
* Remove obsolete INDENT-OFF usage
* Update style to match latest uncrustify
* 0.0.3
* Revert "apply forward slash for list_parameters. (`#224 <https://github.com/ros2/system_tests/issues/224>`_)". (`#229 <https://github.com/ros2/system_tests/issues/229>`_)
  This reverts commit 8e9d767891e4e619b2bbfbd4dac5e6fffafd84bc.
* Merge pull request `#228 <https://github.com/ros2/system_tests/issues/228>`_ from ros2/increase_timeout
  increase timeout of test
* Increase timeout of test
* Revert hack shortening node name. (`#227 <https://github.com/ros2/system_tests/issues/227>`_)
* Apply forward slash for list_parameters. (`#224 <https://github.com/ros2/system_tests/issues/224>`_)
* Call rclcpp::shutdown in all tests. (`#225 <https://github.com/ros2/system_tests/issues/225>`_)
* Use wait_for_service after creating parameters_client. (`#219 <https://github.com/ros2/system_tests/issues/219>`_)
  * Use wait_for_service after creating parameters_client
  * Increase timeout for parameter tests
  * Add prints so we can know where the test hangs
  * Don't make the timeout so big (usually wait_for_service is fast)
  * Reorder lines
  * No need for wait_for_service in local_parameters tests (we know the service is there)
  * Revert "No need for wait_for_service in local_parameters tests (we know the service is there)"
  This reverts commit dce810a515ad58299da353df18e0b7cb29a0b82b.
  * Connext needs the timeout to be high still
* Add tests for user-defined signal handler. (`#215 <https://github.com/ros2/system_tests/issues/215>`_)
  * Add tests for user-defined signal handler
  * Skip signal handler tests on Windows
  launch_testing will terminate the process instead of sending SIGINT, so the tests can't check the response to interrupt
  * Fixup
  * Remove argument parsing
* Ensure nodes have called rclcpp::shutdown before exiting. (`#220 <https://github.com/ros2/system_tests/issues/220>`_)
* Fix flaky multi-threaded test. (`#217 <https://github.com/ros2/system_tests/issues/217>`_)
  * Swap order of expected and actualy value in ASSERT and EXPECT macros
  * Create subscribers and wait before start publishing
  * Fix condition to not abort executor too early
  * Increase queue size to be able to hold all messages
  * Fix condition to not abort executor too early
  * Remove obsolete code, if the test hangs the CTest timeout will take care of it
  * Use actual topic name to work for intra process test too
* 0.0.2
* Use CMAKE_X_STANDARD and check compiler rather than platform
* Add test for avoid_ros_namespace_conventions qos. (`#206 <https://github.com/ros2/system_tests/issues/206>`_)
* Remove unnecessary topic name check. (`#203 <https://github.com/ros2/system_tests/issues/203>`_)
  * Remove incorrect and unnecessary topic name check
  * Up timeout for slow test
* Fix type and style. (`#201 <https://github.com/ros2/system_tests/issues/201>`_)
  * Fix type and style
  * Fix more style
* Fix tests for many core machines. (`#200 <https://github.com/ros2/system_tests/issues/200>`_)
* Support addition of node namespace in rclcpp API. (`#196 <https://github.com/ros2/system_tests/issues/196>`_)
* Use 64-bit integer for parameter tests. (`#197 <https://github.com/ros2/system_tests/issues/197>`_)
  * Use 64-bit integer for parameter tests
  * More fixes for Linux and Windows
* Tests for get_parameter_or and set_parameter_if_not_set. (`#193 <https://github.com/ros2/system_tests/issues/193>`_)
* Use -Wpedantic. (`#189 <https://github.com/ros2/system_tests/issues/189>`_)
  * Add pedantic flag
  * Fix pedantic warning
  * Fix C4456 warning
  * Reduce scope of wait_sets
  * Reduce scope rather than renaming variable
* Merge pull request `#187 <https://github.com/ros2/system_tests/issues/187>`_ from ros2/use_rmw_impl
  use rmw implementation
* Use rmw implementation
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
* Rename QoS policies. (`#184 <https://github.com/ros2/system_tests/issues/184>`_)
* Add test for creating clients and services in a Node constructor. (`#182 <https://github.com/ros2/system_tests/issues/182>`_)
  * Add test for creating clients and services in a Node constructor
  * Style fixes
* Merge pull request `#180 <https://github.com/ros2/system_tests/issues/180>`_ from ros2/typesupport_reloaded
  append build space to library path
* Append build space to library path
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
* Consistent naming when using CMake variable for rmw implementation. (`#169 <https://github.com/ros2/system_tests/issues/169>`_)
* Merge pull request `#166 <https://github.com/ros2/system_tests/issues/166>`_ from ros2/fix_cpplint
  comply with stricter cpplint rules
* Comply with stricter cpplint rules
* Add regression test for different behaviour between first and second client. (`#156 <https://github.com/ros2/system_tests/issues/156>`_)
  * Add regression test for different behaviour between first and second client
  * Lint
  * Fix compiler warnings
  * Spelling fixup
* Add sleep to avoid client/server race until we have a better solution. (`#159 <https://github.com/ros2/system_tests/issues/159>`_)
  * Add sleep to avoid client/server race until we have a better solution
  * Fix uncrustify being dumb
* Add tests for getting single parameter from node. (`#158 <https://github.com/ros2/system_tests/issues/158>`_)
  * Add tests for getting local parameters from node handle
  * Avoid gcc warnings
  * Try to avoid msbuild warnings
  * Use C++11 version of stdint.h to let tests pass on windows
* Merge pull request `#157 <https://github.com/ros2/system_tests/issues/157>`_ from ros2/init_vars
  init variables to avoid compiler warnings
* Init variables to avoid compiler warnings
* Add tests for param helpers. (`#155 <https://github.com/ros2/system_tests/issues/155>`_)
* Allow more time for multithreaded tests. (`#151 <https://github.com/ros2/system_tests/issues/151>`_)
  * Allow more time for multithreaded tests
  * Shorten time
* Merge pull request `#148 <https://github.com/ros2/system_tests/issues/148>`_ from ros2/remove_noop
  remove noops
* Remove noops
* Update schema url
* Merge pull request `#145 <https://github.com/ros2/system_tests/issues/145>`_ from ros2/sleep_if_not_wait_for_service
  use sleep if wait_for_service throws
* Use sleep if wait_for_service throws
* Add schema to manifest files
* Use wait_for_service to make Service tests less flaky. (`#132 <https://github.com/ros2/system_tests/issues/132>`_)
  * Use wait_for_service to make tests less flaky
  * Realign timeouts
  * Avoid using wait_for_service with fastrtps
  this can be undone once fastrtps supports wait_for_service
  * [test_communication] avoid wait_for_service with fastrtps
  it can be undone once fastrtps supports wait_for_service
  * Add test to ensure wait_for_service wakes after shutdown/sigint
* Update tests for changes in parameter handling. (`#140 <https://github.com/ros2/system_tests/issues/140>`_)
  * Update tests for changes in parameter handling
  * Use enum instead of constant
* Merge pull request `#136 <https://github.com/ros2/system_tests/issues/136>`_ from ros2/cmake35
  require CMake 3.5
* Require CMake 3.5
* Merge pull request `#133 <https://github.com/ros2/system_tests/issues/133>`_ from ros2/xenial
  fix compiler warning
* Fix compiler warning
* Merge pull request `#131 <https://github.com/ros2/system_tests/issues/131>`_ from ros2/longer_executor_test_for_windows
  wait a bit longer on the executor notification test
* Wait a bit longer on the executor notification test
* Merge pull request `#120 <https://github.com/ros2/system_tests/issues/120>`_ from dhood/test-linking-runtime
  Ensure using correct rmw implementation in tests
* Use RCL_ASSERT_RMW_ID_MATCHES to ensure correct rmw implementation is being used
* Add classname label to some tests. (`#116 <https://github.com/ros2/system_tests/issues/116>`_)
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
* Add tests for notify guard condition in node
  * Finish (?) notify tests
  * Republish to fix test. Publish in Connext is apparently not deterministic? What a bummer.
  * Put busy_wait_for_subscriber in its own utils.hpp
* Merge pull request `#111 <https://github.com/ros2/system_tests/issues/111>`_ from ros2/fix_assert_ge_order
  fix the order of the assert_ge check in test_publisher
* Try to fix a printf warning that only happens on Linux
* Fix the order of the assert_ge check in test_publisher
* Merge pull request `#110 <https://github.com/ros2/system_tests/issues/110>`_ from ros2/fix_cpplint
  resolve cpplint warnings
* Resolve cpplint warnings
* Merge pull request `#109 <https://github.com/ros2/system_tests/issues/109>`_ from ros2/fix_test_warnings_osx
  fix comparison warnings within uses of gtest macros
* Fix comparison warnings within uses of gtest macros
* Merge pull request `#108 <https://github.com/ros2/system_tests/issues/108>`_ from ros2/fix_flaky_subscription_and_spinning_test
  changed how the subscription_and_spinning test works
* Changed how the subscription_and_spinning test works
  it should now be less flaky
* Merge pull request `#104 <https://github.com/ros2/system_tests/issues/104>`_ from ros2/issue_192
  Add regression test for client scope issue
* Add regression test for `ros2/rclcpp#192 <https://github.com/ros2/rclcpp/issues/192>`_
* Merge pull request `#103 <https://github.com/ros2/system_tests/issues/103>`_ from ros2/spin_before_subscription_singlethreaded
  Spin before subscription: single-threaded
* Make spin_before_subscription case single-threaded and use "count_subscribers" in tests
* Test case for spinning before creating subscription
* Merge pull request `#106 <https://github.com/ros2/system_tests/issues/106>`_ from ros2/fix_executor_test
  Fix race condition in test_executor
* Use separate counter for each thread
* Merge pull request `#105 <https://github.com/ros2/system_tests/issues/105>`_ from ros2/generator_expression
  use generator expressions for configuration specific tests
* Use generator expressions for configuration specific tests
* Merge pull request `#102 <https://github.com/ros2/system_tests/issues/102>`_ from ros2/rename_message_type_support
  support multiple type supports per rmw impl
* Support multiple type supports per rmw impl
* Merge pull request `#101 <https://github.com/ros2/system_tests/issues/101>`_ from ros2/windows_release
  build release on Windows
* Build release on Windows
* Merge pull request `#80 <https://github.com/ros2/system_tests/issues/80>`_ from ros2/waitset_handle
  Add two executors spinning in same process test case
* Add two executors spinning in same process test case
  Add test for one executor per node, refactor for executor arguments
* Might want to increment i
* Merge pull request `#100 <https://github.com/ros2/system_tests/issues/100>`_ from ros2/fix_intra_process_test
  Fix flaky intraprocess test
* Adjust sleeps and timeouts to be more robust, especially for Connext on OSX
* Merge pull request `#98 <https://github.com/ros2/system_tests/issues/98>`_ from ros2/fix_flaky_subscription_test
  Fix flaky subscription test
* Fix flaky subscription test by adding:
  * A 1ms sleep between setup and the start of publishing; and
  * A maximum-2s loop of 10ms sleeps to wait for message delivery.
  Both features appear to be required to ensure reliable test results when the
  system is under load (e.g., `stress -c 8` on an 8-core machine).
* Merge pull request `#97 <https://github.com/ros2/system_tests/issues/97>`_ from ros2/fix_style
  fix style
* Fix style
* Merge pull request `#95 <https://github.com/ros2/system_tests/issues/95>`_ from ros2/flaky_services
  Try to fix flaky services test by partitioning topic names
* Add RMW_IMPLEMENTATION macro to make rmw specific names
* Try to fix flaky services test by partitioning topic names
* Merge pull request `#96 <https://github.com/ros2/system_tests/issues/96>`_ from ros2/fix_rmw_test_suffix
  fix missing rmw test suffix
* Fix missing rmw test suffix
* Merge pull request `#91 <https://github.com/ros2/system_tests/issues/91>`_ from ros2/reorganize
  Remove allocator test
* Merge pull request `#94 <https://github.com/ros2/system_tests/issues/94>`_ from ros2/fix_intraprocess_test
  Fix intraprocess test failure
* Make intraprocess more robust with a bounded sleep that checks for the goal
  condition after publishing.
* Merge pull request `#90 <https://github.com/ros2/system_tests/issues/90>`_ from ros2/increase_timeout_subscription_test
  Increase timeout on subscription test
* Increase timeout on subscription test
* Remove allocator test
* Merge pull request `#89 <https://github.com/ros2/system_tests/issues/89>`_ from ros2/fix_multithreaded_test
  Fix multithreaded test by specifying publisher queue size
* Specify a publisher queue size large enough to hold all the messages that will
  be published, to avoid the possibility that in the intraprocess case we lose
  messages, causing the test to fail to intermittently.
* Merge pull request `#88 <https://github.com/ros2/system_tests/issues/88>`_ from ros2/method_based_callback
  adding a test and a commented out test for the bind that doesn't compile
* Adding a test for subscribing directly with a method and direct std::bind re: `ros2/rclcpp#173 <https://github.com/ros2/rclcpp/issues/173>`_
* Merge pull request `#86 <https://github.com/ros2/system_tests/issues/86>`_ from ros2/refactor_typesupport
  use new approach to generate rmw implementation specific targets
* Use new approach to generate rmw implementation specific targets
* Merge pull request `#84 <https://github.com/ros2/system_tests/issues/84>`_ from ros2/reverse_ignore_logic
  Reverse ignore logic in allocator test
* Reverse ignore_middleware_tokens argument boolean
* Merge pull request `#83 <https://github.com/ros2/system_tests/issues/83>`_ from ros2/missing_dep
  add missing dependency on rmw_implementation_cmake
* Add missing dependency on rmw_implementation_cmake
* Merge pull request `#82 <https://github.com/ros2/system_tests/issues/82>`_ from ros2/multithreaded_wait
  Fix multithreaded test on Windows and Jenkins
* Fix multithreaded test for other platforms: increase timeout, busy wait to ensure condition is met
* Merge pull request `#77 <https://github.com/ros2/system_tests/issues/77>`_ from ros2/printfs
  Improvements to Allocator test
* Improvements to allocator test: argument parsing, reduce static global logic
* Merge pull request `#81 <https://github.com/ros2/system_tests/issues/81>`_ from ros2/license_header
  Fix license lint error
* Fix license lint error
* Merge pull request `#72 <https://github.com/ros2/system_tests/issues/72>`_ from ros2/multithreaded
  Test for multithreaded execution
* Multithreaded pub/sub, client/service, and intra-process tests
* Merge pull request `#79 <https://github.com/ros2/system_tests/issues/79>`_ from ros2/intra_process_lock
  Change State to Impl
* Change State to Impl
* Merge pull request `#76 <https://github.com/ros2/system_tests/issues/76>`_ from ros2/finite_timer
  Finite timer
* Pass TimerBase to callbacks in some tests for finitely firing timers
* Merge pull request `#74 <https://github.com/ros2/system_tests/issues/74>`_ from ros2/return-request
  Added test to check that the request is returned
* Added test to check that the request is returned
* Merge pull request `#71 <https://github.com/ros2/system_tests/issues/71>`_ from ros2/multiple_services_test
  Add new case to multiple_service_calls for "n" clients (currently 5)
* Add new case to multiple_service_calls
* Merge pull request `#73 <https://github.com/ros2/system_tests/issues/73>`_ from ros2/cancel
  Add test for cancel
* Add tests for cancel
* Merge pull request `#70 <https://github.com/ros2/system_tests/issues/70>`_ from ros2/executor_spin_future
  change namespace of FutureReturnCode
* Namespace correction of FutureReturnCode
* Merge pull request `#69 <https://github.com/ros2/system_tests/issues/69>`_ from ros2/fix_timer_tests
  fix timer behavior in test_spin
* Fix timer behavior in test_spin
* Merge pull request `#67 <https://github.com/ros2/system_tests/issues/67>`_ from ros2/rclcpp_library
  use fully qualified name
* Use fully qualified name
* Merge pull request `#65 <https://github.com/ros2/system_tests/issues/65>`_ from ros2/fix_osx_build
  Fix osx build
* Use enable_if with construct in allocator test
* Merge pull request `#64 <https://github.com/ros2/system_tests/issues/64>`_ from ros2/cpplint
  Fix cpplint warnings
* Fix cpplint warnings
* Merge pull request `#60 <https://github.com/ros2/system_tests/issues/60>`_ from ros2/allocator_template
  Allocator template
* Add allocator test
* Merge pull request `#63 <https://github.com/ros2/system_tests/issues/63>`_ from ros2/missing_test_dependency
  add missing test dependency on launch
* Add missing test dependency on launch
* Merge pull request `#62 <https://github.com/ros2/system_tests/issues/62>`_ from ros2/cpplint
  Fix cpplint warnings
* Merge pull request `#61 <https://github.com/ros2/system_tests/issues/61>`_ from ros2/cpplint-int
  Replace unsigned long with uint32_t
* Fix cpplint warnings
* Replace unsigned long with uint32_t
* Merge pull request `#44 <https://github.com/ros2/system_tests/issues/44>`_ from ros2/gtest-parameters
  Enable parameters tests
* Enable parameters tests
* Merge pull request `#59 <https://github.com/ros2/system_tests/issues/59>`_ from ros2/cpplint
  update code to pass ament_cpplint
* Merge pull request `#49 <https://github.com/ros2/system_tests/issues/49>`_ from ros2/parameter_to_yaml
  tests for new parameter to_string API
* Update code to pass ament_cpplint
* Merge pull request `#58 <https://github.com/ros2/system_tests/issues/58>`_ from ros2/optional-qos-profile
  Made rmw_qos_profile argument optional
* Made rmw_qos_profile argument optional
* Remove unused parameter
* Merge pull request `#57 <https://github.com/ros2/system_tests/issues/57>`_ from ros2/test_multiple_service_calls
  add test with multiple service calls
* Make uncrustify happy
* Merge pull request `#56 <https://github.com/ros2/system_tests/issues/56>`_ from ros2/create_subscription_with_queue_size
  add a test which uses the create_subscription with queue size api
* Add test with multiple service calls
* Simplify test
* Merge pull request `#55 <https://github.com/ros2/system_tests/issues/55>`_ from ros2/publish_const_reference
  added a test for publishers which uses the const reference api
* Add a test which uses the create_subscription with queue size api
* Added a test for publishers which uses the const reference api
* Tests for new parameter to_string API
* Merge pull request `#54 <https://github.com/ros2/system_tests/issues/54>`_ from ros2/publish_const_shared_ptr
  Test publishing a ConstSharedPtr
* Test publishing a ConstSharedPtr
* Merge pull request `#42 <https://github.com/ros2/system_tests/issues/42>`_ from ros2/test-services
  Added tests for services
* Added tests for services
* Merge pull request `#53 <https://github.com/ros2/system_tests/issues/53>`_ from ros2/const_shared_ptr
  Test for shared_ptr<const T> callback type
* Add case with callback signature with info
* Add test case for shared ptr to const
* Merge pull request `#52 <https://github.com/ros2/system_tests/issues/52>`_ from ros2/reduce_test_times
  Reduce test times
* Update exception string and add comments
* Reduce test times
* Merge pull request `#48 <https://github.com/ros2/system_tests/issues/48>`_ from ros2/spin_until_future_complete
  Spin until future complete
* Add test for spin_until_future_complete
* Merge pull request `#47 <https://github.com/ros2/system_tests/issues/47>`_ from ros2/main-test-timer
  Call rclcpp::init only once
* Call rclcpp::init only once
* Merge pull request `#46 <https://github.com/ros2/system_tests/issues/46>`_ from ros2/gtest-windows
  Added GTest include dir
* Added GTest include dir
* Merge pull request `#41 <https://github.com/ros2/system_tests/issues/41>`_ from ros2/gtest_location
  fix warnings on Windows
* Fix warnings on Windows
* Merge pull request `#40 <https://github.com/ros2/system_tests/issues/40>`_ from ros2/use_gmock_vendor
  fix compiler error on windows
* Fix compiler error on windows
* Merge pull request `#14 <https://github.com/ros2/system_tests/issues/14>`_ from ros2/test_parameters
  Add tests for parameters
* Merge pull request `#38 <https://github.com/ros2/system_tests/issues/38>`_ from ros2/intra_process_img
  use message_info.from_intra_process in test
* Added tests for parameters
* Use message_info.from_intra_process in test
* Merge pull request `#35 <https://github.com/ros2/system_tests/issues/35>`_ from ros2/rmw_gid_support
  update intra proc tests with different assumptions
* Update intra proc tests with different assumptions
* Merge pull request `#30 <https://github.com/ros2/system_tests/issues/30>`_ from ros2/test_repeated_publisher_subscriber
  add test with repeated publishers / subscribers
* Merge pull request `#28 <https://github.com/ros2/system_tests/issues/28>`_ from ros2/intra_process
  adding tests for intra process communications
* Merge pull request `#31 <https://github.com/ros2/system_tests/issues/31>`_ from ros2/fix_timer_test_name
  fix timer test name
* Add test with repeated publishers / subscribers
* Fix timer test name
* Adding tests for intra process communications
* Merge pull request `#24 <https://github.com/ros2/system_tests/issues/24>`_ from ros2/qos
  Added support for QoS profiles
* Added support for QoS profiles
* Merge pull request `#19 <https://github.com/ros2/system_tests/issues/19>`_ from ros2/wait_timeout
  Add test for timeout parameter
* Added test for timing out subscriber
* Remove linking against GTEST_MAIN_LIBRARIES explicitly
* Use linters
* Merge pull request `#26 <https://github.com/ros2/system_tests/issues/26>`_ from ros2/subscriber_not_deregistering
  update subscription test to check correct deregistration
* Update timer test to check correct deregistration
* Update subscription test to check correct deregistration
* Merge pull request `#25 <https://github.com/ros2/system_tests/issues/25>`_ from ros2/timer_test
  add test for timers
* Add tests for timers
* Relax test even more to make OS X happier. (`#23 <https://github.com/ros2/system_tests/issues/23>`_)
* Merge pull request `#23 <https://github.com/ros2/system_tests/issues/23>`_ from ros2/try_osx
  relax test to make OS X happy
* Relax test to make OS X happy
* Use gtest target only when available
* Merge pull request `#20 <https://github.com/ros2/system_tests/issues/20>`_ from ros2/test_rclcpp_package
  add test_rclcpp package testing subscriptions and spinning for now
* Add test_rclcpp package testing subscriptions and spinning for now
* Contributors: Brian Gerkey, Dirk Thomas, Esteve Fernandez, Guillaume Papin, Jackie Kay, Karsten Knese, Mikael Arguedas, Morgan Quigley, Rafał Kozik, Rohan Agrawal, Tully Foote, William Woodall, dhood, gerkey, nobody
