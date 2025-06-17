^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_security
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.4 (2023-11-21)
-------------------
* Extract sros_artifacts fixture into a CMake script (`#524 <https://github.com/ros2/system_tests/issues/524>`_)
* Use test fixtures to create SROS artifacts (`#524 <https://github.com/ros2/system_tests/issues/524>`_)
* Contributors: Scott K Logan

0.12.3 (2022-04-05)
-------------------

0.12.2 (2022-03-28)
-------------------

0.12.1 (2022-01-13)
-------------------

0.12.0 (2021-11-18)
-------------------
* Updated maintainers (`#489 <https://github.com/ros2/system_tests/issues/489>`_)
* Fix deprecated subscriber callback warnings (`#483 <https://github.com/ros2/system_tests/issues/483>`_)
* Add changelogs (`#473 <https://github.com/ros2/system_tests/issues/473>`_)
* Simplify the test_secure_subscriber code. (`#471 <https://github.com/ros2/system_tests/issues/471>`_)
* Update includes after rcutils/get_env.h deprecation (`#472 <https://github.com/ros2/system_tests/issues/472>`_)
* Contributors: Abrar Rahman Protyasha, Aditya Pande, Chris Lalancette, Christophe Bedard, Ivan Santiago Paunovic

0.11.1 (2021-04-26)
-------------------

0.11.0 (2021-04-06)
-------------------

0.10.0 (2021-03-18)
-------------------
* Add support for rmw_connextdds. (`#463 <https://github.com/ros2/system_tests/issues/463>`_)
* Update deprecated gtest macros. (`#449 <https://github.com/ros2/system_tests/issues/449>`_)
* Update maintainers. (`#450 <https://github.com/ros2/system_tests/issues/450>`_)
* Run test_security on CycloneDDS as well. (`#408 <https://github.com/ros2/system_tests/issues/408>`_)
* Contributors: Andrea Sorbini, Audrow Nash, Jacob Perron, Mikael Arguedas

0.9.1 (2020-07-06)
------------------
* Remove invalid cert folder to force regeneration of certificates. (`#434 <https://github.com/ros2/system_tests/issues/434>`_)
* Contributors: Mikael Arguedas

0.9.0 (2020-06-04)
------------------
* Disable Connext security tests on Windows. (`#433 <https://github.com/ros2/system_tests/issues/433>`_)
  * Disable Connext security tests on Windows
  Connext needs a different version of OpenSSL (1.0.2n) than the system
  version.
  Disabling these tests until we can figure out how to run them on CI.
  * Use parentheses
* Re-enable Fast-RTPS security tests. (`#415 <https://github.com/ros2/system_tests/issues/415>`_)
* Make Connext use Connext's openssl on all platforms. (`#409 <https://github.com/ros2/system_tests/issues/409>`_)
  * Make connext use Connext's openssl on all platforms
  Ubuntu Focal, Homebrew and chocolatey now provide OpenSSL 1.1.1
  RTI Connext 5.3.1 supports only 1.0.2 so we need to modify the library path and the path for connext's version of openssl be used when running tests with connext. Use system OpenSSL for other rmw implementations
* Fix outdated variable name and error on file generation failure. (`#412 <https://github.com/ros2/system_tests/issues/412>`_)
  * POLICY_RESULT->GENERATE_ARTIFACTS_RESULT
  * Fail CMake configure if security artifacts fail to be generated
* Update security environment variables. (`#422 <https://github.com/ros2/system_tests/issues/422>`_)
* Security-context -> enclave. (`#414 <https://github.com/ros2/system_tests/issues/414>`_)
* Disable Fast-RTPS security tests until they work on Focal. (`#413 <https://github.com/ros2/system_tests/issues/413>`_)
  https://github.com/eProsima/Fast-RTPS/issues/1087 is the
  issue that needs to be resolved.
* Use keystore root as security root directory, and not contexts folder. (`#410 <https://github.com/ros2/system_tests/issues/410>`_)
* Update test_security tests to use security contexts
* Remove ready_fn, and one self.proc_info. (`#391 <https://github.com/ros2/system_tests/issues/391>`_)
* Contributors: Chris Lalancette, Ivan Santiago Paunovic, Jacob Perron, Mikael Arguedas, Peter Baughman, Ruffin, Steven! Ragnarök

0.8.0 (2019-11-20)
------------------
* 0.8.0
* [test_security] Generate security artifacts using sros2. (`#380 <https://github.com/ros2/system_tests/issues/380>`_)
  * Remove publisher and subscriber security artifacts
  * Use sros2 to generated security artifacts
  * Print info message to stdout
  * Generate publisher_missing_key as well
  * Generate publisher_invalid_cert
  * Restore environment between tests
  * Give test more explainatory names
  * Reenable xmllint now that no DDS xml files are hosted in the sources
  * Valid_node_names_list -> node_name_list
* Use new message name. (`#379 <https://github.com/ros2/system_tests/issues/379>`_)
* Contributors: Michael Carroll, Mikael Arguedas

0.7.1 (2019-05-29)
------------------

0.7.0 (2019-05-20)
------------------
* Handle launch_testing assertExitCodes correctly. (`#367 <https://github.com/ros2/system_tests/issues/367>`_)
* Changes to avoid deprecated API's. (`#361 <https://github.com/ros2/system_tests/issues/361>`_)
  * Changes to avoid deprecated API's
  * Review comments
* Corrected publish calls with shared_ptr signature. (`#348 <https://github.com/ros2/system_tests/issues/348>`_)
  * Corrected publish calls with shared_ptr signature
  * Updated with PR comments
  * Correct linter failure
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
* Add launch along with launch_testing as test dependencies. (`#334 <https://github.com/ros2/system_tests/issues/334>`_)
* Drops legacy launch API usage. (`#328 <https://github.com/ros2/system_tests/issues/328>`_)
  * Drops legacy launch API usage.
  * Fixes style issues.
  * Drops more legacy launch API use cases.
  * Adds launch_testing as test_security dependency.
  * Applies misc fixes after Windows triaging.
  * Applies more fixes after Windows triaging.
  * Disables test_rclcpp cross vendor tests on Windows.
* Contributors: Jacob Perron, Michel Hidalgo, William Woodall, ivanpauno

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
* Update package maintainer. (`#309 <https://github.com/ros2/system_tests/issues/309>`_)
* Adjusting namespace for security tests. (`#308 <https://github.com/ros2/system_tests/issues/308>`_)
* Use new error handling API from rcutils. (`#306 <https://github.com/ros2/system_tests/issues/306>`_)
  * Use new error handling API from rcutils
  * Fix some more cases where the new error handling API is used
* Merge pull request `#303 <https://github.com/ros2/system_tests/issues/303>`_ from ros2/hidmic/namespace-messages-with-subfolder
  Handles msg files with the same name in different subfolders
* Handles msg files with the same name in different subfolders.
* Separating identity and permission CAs. (`#298 <https://github.com/ros2/system_tests/issues/298>`_)
* Regenerate security artifacts. (`#289 <https://github.com/ros2/system_tests/issues/289>`_)
* Match rmw_fastrtps_dynamic_cpp / rmw_connext_dynamic_cpp. (`#288 <https://github.com/ros2/system_tests/issues/288>`_)
* Disable xmllint correctly. (`#287 <https://github.com/ros2/system_tests/issues/287>`_)
* Disable xmllint for now. (`#287 <https://github.com/ros2/system_tests/issues/287>`_)
* Comment out currently unused rclpy dependency. (`#280 <https://github.com/ros2/system_tests/issues/280>`_)
* Migrate launch -> launch.legacy. (`#273 <https://github.com/ros2/system_tests/issues/273>`_)
* Place RTI OpenSSL on the (LD_LIBRARY\_)PATH on Linux. (`#263 <https://github.com/ros2/system_tests/issues/263>`_)
  * Use RTI_OPENSSL env vars for tests
  * Pass PATH to tests directly
  * Remove dead code and add comments
  * Read env once and modify env var in a platform agnostic way
  * Use TO_CMAKE_PATH
  * Modify path only for Linux for now
  just pass unchanged PATH on other platforms
  Modify PATH only if connext is being tested
  modify path for all connext tests (not sure why it worked before)
  * TEST_PATH_WITH_RTI_BIN -> TEST_PATH
* Sslv3 certificates with CA:false extension. (`#265 <https://github.com/ros2/system_tests/issues/265>`_)
* New security files including governance fix. (`#264 <https://github.com/ros2/system_tests/issues/264>`_)
  provide wide domain id range to work on all ci machines
* [test_communication] Unique namespaces. (`#256 <https://github.com/ros2/system_tests/issues/256>`_)
  * Add namespace to pubsub tests
  * Add namespace to service tests
  * Uncrustify
  * Use UTC time rather than datetime
  * Single quotes
  * Make arguments mandatory like in C++
* Contributors: Alexis Pojomovsky, Dirk Thomas, Michael Carroll, Michel Hidalgo, Mikael Arguedas, Ruffin, William Woodall

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
* Add comment for DYLD_LIBRARY_PATH and connext
* Use brew to find openssl library path and add it to the library dirs. (`#250 <https://github.com/ros2/system_tests/issues/250>`_)
* Find gtest before macro invocation so that its not find during each macro invocation. (`#246 <https://github.com/ros2/system_tests/issues/246>`_)
* Merge pull request `#245 <https://github.com/ros2/system_tests/issues/245>`_ from ros2/ament_cmake_pytest
  use ament_cmake_pytest instead of ament_cmake_nose
* Use ament_cmake_pytest instead of ament_cmake_nose
* Add namespace to avoid crosstalk in security 'no connection' tests. (`#243 <https://github.com/ros2/system_tests/issues/243>`_)
  * Add namespace for security tests so 'no connection' tests don't have crosstalk
  * Rename namespace var
* Restore bigobj. (`#241 <https://github.com/ros2/system_tests/issues/241>`_)
  * [test_communication] restore bigobj
  * [test_security] restore bigobj
  * Make it explicit that bigobj is needed only in debug mode
* Replaces "std::cout<<" with "printf". (`#240 <https://github.com/ros2/system_tests/issues/240>`_)
  * [test_communication]replace uses of iostream
  * [test_rclcpp] remove use of std::cout except flushing
  * Missed some
  * We use float duration not double
  * Remove now unused include
* Merge pull request `#230 <https://github.com/ros2/system_tests/issues/230>`_ from ros2/test_connext_secure
  Test connext secure
* Update security files with domain id wildcards
  move all the testing logic within the if SECURITY bloc
  print unexpected exception
  hack (only for Connext o_O) because publisher keeps publishing even if subscriber terminated
  Revert "hack (only for Connext o_O) because publisher keeps publishing even if subscriber terminated"
  This reverts commit 3349510fc5bf6a0349c99d88b632eada827c3564.
  leave more time to nodes to shut down
  cleanup cmake and reduce test timeout
  test if bigobj is actually needed
  duh increase all timeouts
  don't use iostreams
* Move security tests in different package
  generate new security files with latest sros2 generation script
* Contributors: Dirk Thomas, Mikael Arguedas, dhood
