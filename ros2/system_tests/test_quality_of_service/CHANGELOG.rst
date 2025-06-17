^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_quality_of_service
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.4 (2023-11-21)
-------------------

0.12.3 (2022-04-05)
-------------------

0.12.2 (2022-03-28)
-------------------

0.12.1 (2022-01-13)
-------------------
* Update maintainers to Aditya Pande and Shane Loretz (`#491 <https://github.com/ros2/system_tests/issues/491>`_)
* Contributors: Audrow Nash

0.12.0 (2021-11-18)
-------------------
* Updated maintainers (`#489 <https://github.com/ros2/system_tests/issues/489>`_)
* Fix deprecated subscriber callback warnings (`#483 <https://github.com/ros2/system_tests/issues/483>`_)
* Add changelogs (`#473 <https://github.com/ros2/system_tests/issues/473>`_)
* Contributors: Abrar Rahman Protyasha, Aditya Pande, Ivan Santiago Paunovic

0.11.1 (2021-04-26)
-------------------

0.11.0 (2021-04-06)
-------------------

0.10.0 (2021-03-18)
-------------------
* Add support for rmw_connextdds. (`#463 <https://github.com/ros2/system_tests/issues/463>`_)
* Run QoS tests. (`#441 <https://github.com/ros2/system_tests/issues/441>`_)
* Update maintainers. (`#450 <https://github.com/ros2/system_tests/issues/450>`_)
* Contributors: Andrea Sorbini, Jacob Perron, Michel Hidalgo

0.9.1 (2020-07-06)
------------------

0.9.0 (2020-06-04)
------------------
* Code style only: wrap after open parenthesis if not in one line. (`#397 <https://github.com/ros2/system_tests/issues/397>`_)
* Contributors: Dirk Thomas

0.8.0 (2019-11-20)
------------------
* 0.8.0
* Remove broken constructors. (`#389 <https://github.com/ros2/system_tests/issues/389>`_)
* Contributors: Dan Rose, Michael Carroll

0.7.1 (2019-05-29)
------------------
* QoS System Tests. (`#347 <https://github.com/ros2/system_tests/issues/347>`_)
  * Added new Quality of Service system test package
  Added simple test for liveliness
  * Added deadline system test
  Added lifespan system test
  * Added linting fixes
  * Added documentation
  Added minor liveliness test cleanup
  * Fix CMake warnings
  * Added base class for test nodes
  Added implementation headers and files
  Addressed review comments
  * Added simple deadline test
  Renamed implementing classes to avoid namespace confusion
  * Addressed review comments
  Fixed bugs in fixture bringup and teardown
  Fixed base class issues
  * Fix compiler errors
  * Update to be compatible with latest rclcpp API
  * Removes unused find_package for pytest.
  * Fix windows compiler warnings
  * Replace rclcpp::get_logger() with Node::get_logger()
* Contributors: Devin Bonnie

0.7.0 (2019-05-20)
------------------

0.6.0 (2018-12-14)
------------------

0.4.0 (2017-12-08)
------------------
