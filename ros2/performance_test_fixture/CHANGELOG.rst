^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package performance_test_fixture
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.0.9 (2022-07-25)
------------------
* Mirror rolling to main
* Add "cstring" to the list of includes (`#19 <https://github.com/ros2/performance_test_fixture/issues/19>`_)
* Contributors: Audrow Nash, Chris Lalancette

0.0.8 (2022-01-14)
------------------
* Fix include order for cpplint (`#16 <https://github.com/ros2/performance_test_fixture/issues/16>`_)
* Contributors: Jacob Perron

0.0.7 (2021-03-18)
------------------
* Record calls to calloc, update tests (`#15 <https://github.com/ros2/performance_test_fixture/issues/15>`_)
* Contributors: Scott K Logan

0.0.6 (2020-11-19)
------------------
* Make allocation counter atomic (`#13 <https://github.com/ros2/performance_test_fixture/issues/13>`_)
  Even if the benchmark itself isn't threaded, the process we're testing
  could be. In any case, this should prevent those shenanigans from
  messing up the measurement.
* Add methods for pausing/resuming performance metrics (`#10 <https://github.com/ros2/performance_test_fixture/issues/10>`_)
  * Add methods for pausing/resuming performance metrics
* Add benchmarks to evaluate overhead (`#11 <https://github.com/ros2/performance_test_fixture/issues/11>`_)
  * Add benchmarks to evaluate overhead in performance tests
* Add namespace performance_test_fixture to .cpp (`#9 <https://github.com/ros2/performance_test_fixture/issues/9>`_)
* Contributors: Scott K Logan, brawner

0.0.5 (2020-10-21)
------------------
* Export dependency on benchmark and osrf_testing_tools_cpp (`#8 <https://github.com/ros2/performance_test_fixture/issues/8>`_)
* Update maintainers (`#7 <https://github.com/ros2/performance_test_fixture/issues/7>`_)
* Contributors: Alejandro Hernández Cordero, Scott K Logan

0.0.4 (2020-09-17)
------------------
* Expose a function for resetting the heap counters (`#6 <https://github.com/ros2/performance_test_fixture/issues/6>`_)
* Stop recording memory operations sooner (`#5 <https://github.com/ros2/performance_test_fixture/issues/5>`_)
* Suppress memory tools warning if tests will be skipped (`#4 <https://github.com/ros2/performance_test_fixture/issues/4>`_)
* Contributors: Scott K Logan

0.0.3 (2020-09-02)
------------------
* Export dependency on ament_cmake_google_benchmark (`#3 <https://github.com/ros2/performance_test_fixture/issues/3>`_)
* Contributors: Scott K Logan

0.0.2 (2020-07-23)
------------------
* Add missing dependency on ament_cmake_google_benchmark (`#2 <https://github.com/ros2/performance_test_fixture/issues/2>`_)
* Contributors: Scott K Logan

0.0.1 (2020-07-23)
------------------
* Initial 'performance_test_fixture' package (`#1 <https://github.com/ros2/performance_test_fixture/issues/1>`_)
* Initial commit
* Contributors: Scott K Logan
