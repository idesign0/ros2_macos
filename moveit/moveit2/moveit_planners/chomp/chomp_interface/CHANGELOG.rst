^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package chomp_interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.5.9 (2025-04-15)
------------------

2.5.8 (2025-02-09)
------------------

2.5.7 (2024-12-29)
------------------

2.5.6 (2024-11-17)
------------------

2.5.5 (2023-09-10)
------------------
* Re-enable clang-tidy check `performance-unnecessary-value-param` (backport `#1703 <https://github.com/ros-planning/moveit2/issues/1703>`_)
  * Re-enable clang-tidy check performance-unnecessary-value-param (`#1703 <https://github.com/ros-planning/moveit2/issues/1703>`_)
  * Fix clang-tidy issues (`#1706 <https://github.com/ros-planning/moveit2/issues/1706>`_)
  Co-authored-by: Henning Kayser <henningkayser@picknik.ai>
  Co-authored-by: Robert Haschke <rhaschke@users.noreply.github.com>
* Contributors: mergify[bot]

2.5.4 (2022-11-04)
------------------
* Add planner configurations to CHOMP and PILZ (`#1522 <https://github.com/ros-planning/moveit2/issues/1522>`_) (`#1656 <https://github.com/ros-planning/moveit2/issues/1656>`_)
  (cherry picked from commit 888fc5358280b20edc394947e98341c0f03dc0bd)
  Co-authored-by: Sebastian Jahr <sebastian.jahr@picknik.ai>
* Improve CMake usage (`#1550 <https://github.com/ros-planning/moveit2/issues/1550>`_) (`#1555 <https://github.com/ros-planning/moveit2/issues/1555>`_)
  Co-authored-by: Sebastian Jahr <sebastian.jahr@picknik.ai>
* Contributors: mergify[bot]

2.5.3 (2022-07-28)
------------------

2.5.2 (2022-07-18)
------------------
* Remove unnecessary rclcpp.hpp includes (`#1333 <https://github.com/ros-planning/moveit2/issues/1333>`_)
* Contributors: Jafar

2.5.1 (2022-05-31)
------------------

2.5.0 (2022-05-26)
------------------
* Make TOTG the default time-parameterization algorithm everywhere (`#1218 <https://github.com/ros-planning/moveit2/issues/1218>`_)
* Make moveit_common a 'depend' rather than 'build_depend' (`#1226 <https://github.com/ros-planning/moveit2/issues/1226>`_)
* Merge https://github.com/ros-planning/moveit/commit/424a5b7b8b774424f78346d1e98bf1c9a33f0e78
* Merge https://github.com/ros-planning/moveit/commit/ab42a1d7017b27eb6c353fb29331b2da08ab0039
* Remove unused parameters. (`#1018 <https://github.com/ros-planning/moveit2/issues/1018>`_)
* MSA: Add STOMP + OMPL-CHOMP configs (`#2955 <https://github.com/ros-planning/moveit2/issues/2955>`_)
* Move MoveItErrorCode class to moveit_core (`#3009 <https://github.com/ros-planning/moveit2/issues/3009>`_)
* Merge PRs `#2948 <https://github.com/ros-planning/moveit2/issues/2948>`_ (improve CI) and `#2949 <https://github.com/ros-planning/moveit2/issues/2949>`_ (simplify ROS .test files)
* Use test_environment.launch in unittests
* Contributors: Abishalini, AndyZe, Cory Crean, Jafar, Jafar Abdi, Rick Staa, Robert Haschke, jeoseo

2.4.0 (2022-01-20)
------------------
* moveit_build_options()
  Declare common build options like CMAKE_CXX_STANDARD, CMAKE_BUILD_TYPE,
  and compiler options (namely warning flags) once.
  Each package depending on moveit_core can use these via moveit_build_options().
* Contributors: Robert Haschke

2.3.2 (2021-12-29)
------------------

2.3.1 (2021-12-23)
------------------
* Port CHOMP Motion Planner to ROS 2 (`#809 <https://github.com/ros-planning/moveit2/issues/809>`_)
* Update Maintainers of MoveIt package (`#697 <https://github.com/ros-planning/moveit2/issues/697>`_)
* clang-tidy: modernize-make-shared, modernize-make-unique (`#2762 <https://github.com/ros-planning/moveit/issues/2762>`_)
* Contributors: Dave Coleman, Henning Kayser, Robert Haschke, andreas-botbuilt, pvanlaar

1.1.1 (2020-10-13)
------------------
* [maint] Add comment to MOVEIT_CLASS_FORWARD (`#2315 <https://github.com/ros-planning/moveit/issues/2315>`_)
* Contributors: Felix von Drigalski

1.1.0 (2020-09-04)
------------------
* [feature] Replace $(find xacro)/xacro -> xacro (`#2282 <https://github.com/ros-planning/moveit/issues/2282>`_)
* [feature] Start new joint_state_publisher_gui on param use_gui (`#2257 <https://github.com/ros-planning/moveit/issues/2257>`_)
* [feature] Optional cpp version setting (`#2166 <https://github.com/ros-planning/moveit/issues/2166>`_)
* [feature] Change API of ChompPlanner::solve() to not use message
* [fix] Various fixes for upcoming Noetic release (`#2180 <https://github.com/ros-planning/moveit/issues/2180>`_)
* [fix] clang-tidy fixes (`#2050 <https://github.com/ros-planning/moveit/issues/2050>`_)
* [fix] Fix compiler warnings (`#1773 <https://github.com/ros-planning/moveit/issues/1773>`_)
* [fix] Small fixes to chomp planner (`#1407 <https://github.com/ros-planning/moveit/issues/1407>`_)
* [maint] Replace namespaces robot_state and robot_model with moveit::core (`#1924 <https://github.com/ros-planning/moveit/issues/1924>`_)
* [maint] Add Missing License (`#1779 <https://github.com/ros-planning/moveit/issues/1779>`_)
* [maint] Switch from include guards to pragma once (`#1615 <https://github.com/ros-planning/moveit/issues/1615>`_)
* [maint] Remove ! from MoveIt name (`#1590 <https://github.com/ros-planning/moveit/issues/1590>`_)
* Contributors: Ayush Garg, Chittaranjan Srinivas Swaminathan, Dave Coleman, Jonathan Binney, Markus Vieth, Robert Haschke, Sean Yen, Tyler Weaver, Yoan Mollard

1.0.6 (2020-08-19)
------------------
* [maint] Migrate to clang-format-10
* [maint] Optimize includes (`#2229 <https://github.com/ros-planning/moveit/issues/2229>`_)
* Contributors: Markus Vieth, Robert Haschke

1.0.5 (2020-07-08)
------------------

1.0.4 (2020-05-30)
------------------

1.0.3 (2020-04-26)
------------------
* [maint] Windows build: Fix binary artifact install locations. (`#1575 <https://github.com/ros-planning/moveit/issues/1575>`_)
* [maint] Use CMAKE_CXX_STANDARD to enforce c++14 (`#1607 <https://github.com/ros-planning/moveit/issues/1607>`_)
* Contributors: Robert Haschke, Sean Yen

1.0.2 (2019-06-28)
------------------
* [fix] Fix chomp planner (`#1512 <https://github.com/ros-planning/moveit/issues/1512>`_)
  * Fix start-state handling
  * remove time parameterization from planning code
* Contributors: Robert Haschke

1.0.1 (2019-03-08)
------------------
* [improve] Apply clang tidy fix to entire code base (Part 1) (`#1366 <https://github.com/ros-planning/moveit/issues/1366>`_)
* Contributors: Robert Haschke, Yu, Yan

1.0.0 (2019-02-24)
------------------
* [fix] catkin_lint issues (`#1341 <https://github.com/ros-planning/moveit/issues/1341>`_)
* Contributors: Dave Coleman, Robert Haschke

0.10.8 (2018-12-24)
-------------------

0.10.7 (2018-12-13)
-------------------

0.10.6 (2018-12-09)
-------------------
* [maintenance] Rearranged CHOMP-related modules within moveit_planners/chomp (`#1251 <https://github.com/ros-planning/moveit/issues/1251>`_)
* Contributors: Robert Haschke

0.10.5 (2018-11-01)
-------------------

0.10.4 (2018-10-29)
-------------------

0.10.3 (2018-10-29)
-------------------
* [fix] Build regression (`#1134 <https://github.com/ros-planning/moveit/issues/1134>`_)
* [fix] compiler warnings (`#1089 <https://github.com/ros-planning/moveit/issues/1089>`_)
* Contributors: Robert Haschke

0.10.2 (2018-10-24)
-------------------
* [fix] chomp tests: fix order of moveit includes (`#970 <https://github.com/ros-planning/moveit/issues/970>`_)
* [fix] needs to depend on cmake_modules. (`#976 <https://github.com/ros-planning/moveit/issues/976>`_)
* [capability][chomp] Failure recovery options for CHOMP by tweaking parameters (`#987 <https://github.com/ros-planning/moveit/issues/987>`_)
* [capability][chomp] cleanup of unused parameters and code + addition of trajectory initialization methods (linear, cubic, quintic-spline) (`#960 <https://github.com/ros-planning/moveit/issues/960>`_)
* [maintenance] various compiler warnings (`#1038 <https://github.com/ros-planning/moveit/issues/1038>`_)
* [maintenance] add minimum required pluginlib version (`#927 <https://github.com/ros-planning/moveit/issues/927>`_)
* Contributors: Chris Lalancette, Michael Görner, Mikael Arguedas, Raghavender Sahdev, Robert Haschke

0.10.1 (2018-05-25)
-------------------
* [fix] dependencies for chomp interface test (`#778 <https://github.com/ros-planning/moveit/issues/778>`_)
* [maintenance] MoveIt tf2 migration (`#830 <https://github.com/ros-planning/moveit/issues/830>`_)
* Contributors: Bence Magyar, Dave Coleman, Ian McMahon, Mikael Arguedas, Robert Haschke, Stephan, Will Baker

0.9.11 (2017-12-25)
-------------------

0.9.10 (2017-12-09)
-------------------
* [package.xml] Update maintainers (Add a release-maintainer etc.)
  For the reasoning, see https://github.com/ros-planning/moveit/issues/259
* Contributors: Isaac I.Y. Saito

0.9.9 (2017-08-06)
------------------
* [improve] Chomp use PlanningScene (`#546 <https://github.com/ros-planning/moveit/issues/546>`_) to partially address `#305 <https://github.com/ros-planning/moveit/issues/305>`_
* Contributors: Simon Schmeisser

0.9.8 (2017-06-21)
------------------

0.9.7 (2017-06-05)
------------------

0.9.6 (2017-04-12)
------------------

0.9.5 (2017-03-08)
------------------

0.9.4 (2017-02-06)
------------------
* [maintenance] clang-format upgraded to 3.8 (`#367 <https://github.com/ros-planning/moveit/issues/367>`_)
* Contributors: Dave Coleman

0.9.3 (2016-11-16)
------------------
* Merge pull request `#330 <https://github.com/ros-planning/moveit/issues/330>`_ from davetcoleman/kinetic-package.xml
  Updated package.xml maintainers and author emails
* Updated package.xml maintainers and author emails
* Contributors: Dave Coleman, Ian McMahon

0.9.2 (2016-11-05)
------------------

0.9.0 (2016-10-19)
------------------
* Replace broken Eigen3 with correctly spelled EIGEN3 (`#254 <https://github.com/ros-planning/moveit/issues/254>`_)
  * Fix Eigen3 dependency throughout packages
  * Eigen 3.2 does not provide EIGEN3_INCLUDE_DIRS, only EIGEN3_INCLUDE_DIR
* Use shared_ptr typedefs in collision_distance_field and chomp.
* Fix CHOMP planner and CollisionDistanceField (`#155 <https://github.com/ros-planning/moveit/issues/155>`_)
  * Copy collision_distance_field package
  * Resurrect chomp
  * remove some old Makefiles and manifests
  * Correct various errors
  * Code formatting, author, description, version, etc
  * Add definitions for c++11. Nested templates problem.
  * Add name to planner plugin.
  * Change getJointModels to getActiveJointModels.
  * Call robot_state::RobotState::update in setRobotStateFromPoint.
  * Create README.md
  * Improve package.xml, CMake config and other changes suggested by jrgnicho.
  * Remove some commented code, add scaling factors to computeTimeStampes
  * Add install targets in moveit_experimental and chomp
  * Add install target for headers in chomp pkgs.
  * Remove unnecessary debugging ROS_INFO.
  * Port collision_distance_field test to indigo.
  * Remove one assertion that makes collision_distance_field test to fail.
* Contributors: Chittaranjan Srinivas Swaminathan, Dave Coleman, Maarten de Vries

0.8.3 (2016-08-21)
------------------
