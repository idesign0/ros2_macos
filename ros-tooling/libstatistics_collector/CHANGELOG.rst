^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package libstatistics_collector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.3 (2024-11-25)
------------------
* Fix MovingAverageStatistics::max\_ Default Value (`#201 <https://github.com/ros-tooling/libstatistics_collector/issues/201>`_) (`#204 <https://github.com/ros-tooling/libstatistics_collector/issues/204>`_)
* Contributors: mergify[bot]

1.3.2 (2024-07-26)
------------------
* Add in missing cstdint include. (`#165 <https://github.com/ros-tooling/libstatistics_collector/issues/165>`_) (`#197 <https://github.com/ros-tooling/libstatistics_collector/issues/197>`_)
* Contributors: mergify[bot]

1.3.1 (2023-02-21)
------------------
* Check if message has a "header" field with a stamp subfield of type builtin_interfaces::msg::Time (`#54 <https://github.com/ros-tooling/libstatistics_collector/issues/54>`_) (`#153 <https://github.com/ros-tooling/libstatistics_collector/issues/153>`_)
* Contributors: Tim Clephas

1.3.0 (2022-11-07)
------------------
* Bump hmarr/auto-approve-action from 2.1.0 to 2.2.0
* Contributors: dependabot[bot]

1.2.0 (2022-03-29)
------------------
* Bump pascalgn/automerge-action from 0.14.3 to 0.15.2
* Bump ros-tooling/setup-ros from 0.2.2 to 0.3.0
* Bump actions/upload-artifact from 2.3.1 to 3
* Bump actions/upload-artifact from 2.2.4 to 2.3.1
* Bump actions/checkout from 2 to 3
* Bump ros-tooling/setup-ros from 0.2.1 to 0.2.2 (`#123 <https://github.com/ros-tooling/libstatistics_collector/issues/123>`_)
* Install includes to include/${PROJECT_NAME} (`#122 <https://github.com/ros-tooling/libstatistics_collector/issues/122>`_)
* Bump codecov/codecov-action from 2.0.3 to 2.1.0
* Bump pascalgn/automerge-action from 0.14.2 to 0.14.3
* Bump codecov/codecov-action from 2.0.2 to 2.0.3
* Use rosidl_get_typesupport_target() (`#116 <https://github.com/ros-tooling/libstatistics_collector/issues/116>`_)
* Bump codecov/codecov-action from 2.0.1 to 2.0.2
* Bump codecov/codecov-action from 1.5.2 to 2.0.1
* Bump actions/upload-artifact from 1 to 2.2.4
* Bump codecov/codecov-action from 1.5.1 to 1.5.2
* Remove cloudwatch reporting (`#110 <https://github.com/ros-tooling/libstatistics_collector/issues/110>`_)
* Bump codecov/codecov-action from 1.3.1 to 1.5.1
* Bump ros-tooling/setup-ros from 0.2.0 to 0.2.1
* Bump pascalgn/automerge-action from 0.14.1 to 0.14.2
* Bump ros-tooling/setup-ros from 0.1 to 0.2.0
* Bump pascalgn/automerge-action from 0.13.1 to 0.14.1
* Fix autoapprove
* Package.json explicitly owned by emerson to minimize notifications
* Replace index.ros.org links -> docs.ros.org. (`#94 <https://github.com/ros-tooling/libstatistics_collector/issues/94>`_)
* Bump hmarr/auto-approve-action from v2.0.0 to v2.1.0
* Bump codecov/codecov-action from v1.2.1 to v1.3.1
* Use latest versions of CI actions (`#92 <https://github.com/ros-tooling/libstatistics_collector/issues/92>`_)
* Contributors: Chris Lalancette, Emerson Knapp, Shane Loretz, dependabot[bot]

1.1.0 (2021-03-19)
------------------
* fix: measured values after the decimal point are truncated `#79 <https://github.com/ros-tooling/libstatistics_collector/issues/79>`_ (`#80 <https://github.com/ros-tooling/libstatistics_collector/issues/80>`_)
* Update linter to run on rolling+focal (`#81 <https://github.com/ros-tooling/libstatistics_collector/issues/81>`_)
* Add automerge.yml config file (`#70 <https://github.com/ros-tooling/libstatistics_collector/issues/70>`_)
* Update QD to QL 1 (`#68 <https://github.com/ros-tooling/libstatistics_collector/issues/68>`_)
* Updated QD (`#64 <https://github.com/ros-tooling/libstatistics_collector/issues/64>`_)
* Updated QD Performance tests (`#58 <https://github.com/ros-tooling/libstatistics_collector/issues/58>`_)
* Added benchmark test to libstatistics_collector (`#57 <https://github.com/ros-tooling/libstatistics_collector/issues/57>`_)
  * Added benchmark test to libstatistics_collector
  * cppcheck supressed unknown macro warning - macos
  * Reset heap counters
  * Added feedback
  * Remove unknownMacro suppression from CMakeLists.txt
  * Added feedback
  * moved benchmark test to test/benchmark
  * Added feedback
  Co-authored-by: Devin Bonnie <47613035+dabonnie@users.noreply.github.com>
* Report failed workflows (`#56 <https://github.com/ros-tooling/libstatistics_collector/issues/56>`_)
  Allow codecov failures to be silent
* Add default CODEOWNERS file (`#55 <https://github.com/ros-tooling/libstatistics_collector/issues/55>`_)
* Remove repo activity from individual repositories in favor of centralized reporting (`#52 <https://github.com/ros-tooling/libstatistics_collector/issues/52>`_)
* Don't attempt to report if originating from a fork (`#43 <https://github.com/ros-tooling/libstatistics_collector/issues/43>`_)
* Removed doxygen warnings (`#41 <https://github.com/ros-tooling/libstatistics_collector/issues/41>`_)
  Co-authored-by: Anas Abou Allaban <allabana@amazon.com>
* Add autoapprove action for dependabot (`#40 <https://github.com/ros-tooling/libstatistics_collector/issues/40>`_)
* Create Dependabot config file (`#31 <https://github.com/ros-tooling/libstatistics_collector/issues/31>`_)
  * Create Dependabot config file
  * Randomize time of run
  Co-authored-by: dependabot-preview[bot] <27856297+dependabot-preview[bot]@users.noreply.github.com>
  Co-authored-by: Prajakta Gokhale <prajaktg@amazon.com>
* Updated QD to 3 (`#30 <https://github.com/ros-tooling/libstatistics_collector/issues/30>`_)
* Add Security Vulnerability Policy pointing to REP-2006. (`#24 <https://github.com/ros-tooling/libstatistics_collector/issues/24>`_)
  Co-authored-by: Emerson Knapp <537409+emersonknapp@users.noreply.github.com>
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Devin Bonnie, Emerson Knapp, Lucas Han, Prajakta Gokhale, Stephen Brawner, hsgwa

1.0.1 (2020-05-27)
------------------
* Added quality declaration (`#21 <https://github.com/ros-tooling/libstatistics_collector/issues/21>`_)
  * Added quality declaration
  * Added feedback
  * Fixed rep link
  * Fixed QD
  * added feedback
  * Added feedback
* Added Doxyfile (`#23 <https://github.com/ros-tooling/libstatistics_collector/issues/23>`_)
* Run CI on Focal (`#20 <https://github.com/ros-tooling/libstatistics_collector/issues/20>`_)
* Run lint worflow on Docker (`#19 <https://github.com/ros-tooling/libstatistics_collector/issues/19>`_)
* Fix annotation syntax for thread safety attributes (`#18 <https://github.com/ros-tooling/libstatistics_collector/issues/18>`_)
* Remove unused strategy matrix for ASAN CI job (`#17 <https://github.com/ros-tooling/libstatistics_collector/issues/17>`_)
* Refactor workflow to extract CW reporting (`#15 <https://github.com/ros-tooling/libstatistics_collector/issues/15>`_)
* Contributors: Alejandro Hernández Cordero, Emerson Knapp, Thomas Moulard

1.0.0 (2020-04-29)
------------------
* Bump actions versions (`#14 <https://github.com/ros-tooling/libstatistics_collector/issues/14>`_)
  * Bump actions versions
  * Use upload-artifact v1
* Bump setup-ros to 0.0.20 (`#10 <https://github.com/ros-tooling/libstatistics_collector/issues/10>`_)
  0.0.20 is also installing numpy which currently
  prevents the CI runs from succeeding.
* Log workflow results to CloudWatch (`#11 <https://github.com/ros-tooling/libstatistics_collector/issues/11>`_)
* Add repo activity workflow (`#12 <https://github.com/ros-tooling/libstatistics_collector/issues/12>`_)
* export targets in a addition to include directories / libraries (`#8 <https://github.com/ros-tooling/libstatistics_collector/issues/8>`_)
* Fix windows warning (`#6 <https://github.com/ros-tooling/libstatistics_collector/issues/6>`_)
* Apply windows vibility fix changes (`#5 <https://github.com/ros-tooling/libstatistics_collector/issues/5>`_)
  * Apply windows vibility fix changes
  * Add test fixes
  * Add ament_cmake_ros dependency
  * Alphasort CmakeLists.txt
  * Alphasort package.xml
  * Alphasort collector.hpp
  * Add quotes in cmake
* Fix README test badge (`#4 <https://github.com/ros-tooling/libstatistics_collector/issues/4>`_)
  * Fix README test badge
  * Fix bracket
* Fix license format and add linter actions (`#3 <https://github.com/ros-tooling/libstatistics_collector/issues/3>`_)
* Update GH Actions badge
* Move libstatistics_collector folder from system_metrics_collector (`#2 <https://github.com/ros-tooling/libstatistics_collector/issues/2>`_)
  * Moved libstatistics_collector folder from system_metrics_collector
  * Address review comments
  * Add actions, reflect changes in statistics_msgs
  * Add CONTRIBUTING.md
  Co-authored-by: Prajakta Gokhale <prajaktg@amazon.com>
* Initial commit
* Contributors: Devin Bonnie, Dirk Thomas, Emerson Knapp, Prajakta Gokhale, Thomas Moulard
