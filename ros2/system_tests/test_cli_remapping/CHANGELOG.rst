^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_cli_remapping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.4 (2023-11-21)
-------------------

0.12.3 (2022-04-05)
-------------------

0.12.2 (2022-03-28)
-------------------

0.12.1 (2022-01-13)
-------------------

0.12.0 (2021-11-18)
-------------------
* Update python nodes SIGINT handling (`#490 <https://github.com/ros2/system_tests/issues/490>`_)
* Updated maintainers (`#489 <https://github.com/ros2/system_tests/issues/489>`_)
* Add changelogs (`#473 <https://github.com/ros2/system_tests/issues/473>`_)
* Contributors: Aditya Pande, Ivan Santiago Paunovic

0.11.1 (2021-04-26)
-------------------
* Fix test_cli_remapping flaky test. (`#470 <https://github.com/ros2/system_tests/issues/470>`_)
* Contributors: Shane Loretz

0.11.0 (2021-04-06)
-------------------

0.10.0 (2021-03-18)
-------------------
* Update maintainers. (`#450 <https://github.com/ros2/system_tests/issues/450>`_)
* Enable -Wall, -Wextra, and -Wpedantic. (`#448 <https://github.com/ros2/system_tests/issues/448>`_)
* Contributors: Audrow Nash, Jacob Perron

0.9.1 (2020-07-06)
------------------

0.9.0 (2020-06-04)
------------------
* Remove ready_fn, and one self.proc_info. (`#391 <https://github.com/ros2/system_tests/issues/391>`_)
* Contributors: Peter Baughman

0.8.0 (2019-11-20)
------------------
* 0.8.0
* Use of -r/--remap flags where appropriate. (`#384 <https://github.com/ros2/system_tests/issues/384>`_)
* Adapt to '--ros-args ... [--]'-based ROS args extraction. (`#381 <https://github.com/ros2/system_tests/issues/381>`_)
  * Use --ros-args in test_cli package tests.
  * Use --ros-args in test_cli_remapping package tests.
* Contributors: Michael Carroll, Michel Hidalgo

0.7.1 (2019-05-29)
------------------

0.7.0 (2019-05-20)
------------------
* Fix deprecation warnings. (`#364 <https://github.com/ros2/system_tests/issues/364>`_)
* Changes to avoid deprecated API's. (`#361 <https://github.com/ros2/system_tests/issues/361>`_)
  * Changes to avoid deprecated API's
  * Review comments
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
* Contributors: Jacob Perron, Michel Hidalgo, William Woodall

0.6.0 (2018-12-14)
------------------
* Use debug python executable on windows. (`#281 <https://github.com/ros2/system_tests/issues/281>`_)
  * Use debug python executable on windows
  * Get python debug executable from pythonextra
* Use new launch legacy namespace. (`#275 <https://github.com/ros2/system_tests/issues/275>`_)
* Add tests for command line remapping. (`#268 <https://github.com/ros2/system_tests/issues/268>`_)
  * Add tests for command line remapping
* Contributors: Mikael Arguedas, Shane Loretz

0.4.0 (2017-12-08)
------------------
