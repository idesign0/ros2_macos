^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package spdlog_vendor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.1 (2023-01-10)
------------------
* Fixes policy CMP0135 warning for CMake >= 3.24 (`#30 <https://github.com/ros2/spdlog_vendor/issues/30>`_) (`#32 <https://github.com/ros2/spdlog_vendor/issues/32>`_)
* Contributors: mergify[bot]

1.3.0 (2021-04-06)
------------------
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#24 <https://github.com/ros2/spdlog_vendor/issues/24>`_)
* Update to spdlog 1.8.2 (`#23 <https://github.com/ros2/spdlog_vendor/issues/23>`_)
* Contributors: Chris Lalancette, shonigmann

1.2.2 (2021-03-18)
------------------
* Remove a stale TODO (`#22 <https://github.com/ros2/spdlog_vendor/issues/22>`_)
* Always preserve source permissions in vendor packages (`#20 <https://github.com/ros2/spdlog_vendor/issues/20>`_)
* Contributors: Scott K Logan

1.2.1 (2021-01-25)
------------------
* Remove unnecessary call to find_package(PATCH) (`#18 <https://github.com/ros2/spdlog_vendor/issues/18>`_)
* Contributors: Chris Lalancette

1.2.0 (2020-12-08)
------------------
* Updated QD to 1 (`#16 <https://github.com/ros2/spdlog_vendor/issues/16>`_)
* bump spdlog version to 1.6.1 (`#15 <https://github.com/ros2/spdlog_vendor/issues/15>`_)
* Bump QD to level 3 and updated QD (`#14 <https://github.com/ros2/spdlog_vendor/issues/14>`_)
* Add Security Vulnerability Policy pointing to REP-2006. (`#13 <https://github.com/ros2/spdlog_vendor/issues/13>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Dirk Thomas

1.1.2 (2020-05-26)
------------------
* Add an external package QD for the spdlog library (`#8 <https://github.com/ros2/spdlog_vendor/issues/8>`_)
* Remove included extra ament_copyright (`#12 <https://github.com/ros2/spdlog_vendor/issues/12>`_)
* Enable Copyright linter tests + Add Contributing.md file (`#11 <https://github.com/ros2/spdlog_vendor/issues/11>`_)
* Add quality declaration spdlog_vendor (`#6 <https://github.com/ros2/spdlog_vendor/issues/6>`_)
* Contributors: Jorge Perez, Scott K Logan

1.1.1 (2020-04-24)
------------------
* Silence warning from GNUInstallDirs on Windows. (`#10 <https://github.com/ros2/spdlog_vendor/issues/10>`_)

1.1.0 (2020-04-10)
------------------
* suppress sign conversion warning (`#7 <https://github.com/ros2/spdlog_vendor/issues/7>`_)
* Fix linting (`#3 <https://github.com/ros2/spdlog_vendor/issues/3>`_)
* Spdlog vendor update (`#2 <https://github.com/ros2/spdlog_vendor/issues/2>`_)
* Enable linter tests on spdlog_vendor (`#1 <https://github.com/ros2/spdlog_vendor/issues/1>`_)
* Contributors: Anas Abou Allaban, Chris Lalancette, Jorge Perez, Karsten Knese

1.0.1 (2019-10-03)
------------------
* Add spdlog depend to the package.xml
* Don't build the benchmark, tests, or examples.
* Initial commit.
* Contributors: Chris Lalancette
