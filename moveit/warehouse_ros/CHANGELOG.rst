^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package warehouse_ros
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.0.5 (2024-08-29)
------------------
* Add constructor taking a NodeParametersInterface (`#97 <https://github.com/ros-planning/warehouse_ros/issues/97>`_)
* Refactor processing of parameters in loadDatabase() (`#94 <https://github.com/ros-planning/warehouse_ros/issues/94>`_)
* Fix public OpenSSL dependency (`#86 <https://github.com/ros-planning/warehouse_ros/issues/86>`_)
* Contributors: Bjar Ne, Calen Robinson, Robert Haschke, Sebastian Jahr, Vatan Aksoy Tezer

2.0.4 (2021-09-27)
------------------
* Updated tf2_geometry_msgs.h to tf2_geometry_msgs.hpp  (`#85 <https://github.com/ros-planning/warehouse_ros/issues/85>`_)
* Contributors: Diego Rojas, Vatan Aksoy Tezer

2.0.3 (2021-06-29)
------------------
* Fix OpenSSL export, use package.xml format 3 (`#83 <https://github.com/ros-planning/warehouse_ros/issues/83>`_)
* Contributors: Henning Kayser

2.0.2 (2021-06-29)
------------------
* Use ament_export_targets to fix exporting dependencies (`#80 <https://github.com/ros-planning/warehouse_ros/issues/80>`_)
* Sync with kinetic-devel branch up-to https://github.com/ros-planning/warehouse_ros/commit/25c94751a96b02e46859fec36915c9e8f38106e5 (`#78 <https://github.com/ros-planning/warehouse_ros/issues/78>`_)
* Fix MD5 calculation (`#79 <https://github.com/ros-planning/warehouse_ros/issues/79>`_)
  MD5 checksums may contain NULLs, but are not guaranteed to be zero-terminated.
  Co-authored-by: Bjar Ne <gleichdick@users.noreply.github.com>
* [ROS2] Add prerelease tests (`#76 <https://github.com/ros-planning/warehouse_ros/issues/76>`_)
* Add Galactic CI (`#75 <https://github.com/ros-planning/warehouse_ros/issues/75>`_)
* Fix building on windows (`#73 <https://github.com/ros-planning/warehouse_ros/issues/73>`_)
* Contributors: Akash, Bjar Ne, Jafar Abdi, Vatan Aksoy Tezer

2.0.1 (2021-05-24)
------------------

* List OpenSSL as build depend (`#68 <https://github.com/ros-planning/warehouse_ros/issues/68>`_)
* Update CI and add Rolling Test (`#69 <https://github.com/ros-planning/warehouse_ros/issues/69>`_)
* Add badges for CI to README (`#62 <https://github.com/ros-planning/warehouse_ros/issues/62>`_)
* Add python black formatter to pre-commit (`#66 <https://github.com/ros-planning/warehouse_ros/issues/66>`_)
* Add copyright notices and test (`#53 <https://github.com/ros-planning/warehouse_ros/issues/53>`_)
* Add github actions ci using industrial_ci (`#54 <https://github.com/ros-planning/warehouse_ros/issues/54>`_, `#55 <https://github.com/ros-planning/warehouse_ros/issues/55>`_)
  * Enable ccache (`#56 <https://github.com/ros-planning/warehouse_ros/issues/56>`_, `#61 <https://github.com/ros-planning/warehouse_ros/issues/61>`_, `#65 <https://github.com/ros-planning/warehouse_ros/issues/65>`_)
* Contributors: Tyler Weaver

2.0.0 (2020-11-20)
------------------
* [maint] Fix `-Wcast-qual` compile warnings (`#49 <https://github.com/ros-planning/warehouse_ros/issues/49>`_)
* [ros2-migration] Port to ROS 2 (`#48 <https://github.com/ros-planning/warehouse_ros/issues/48>`_)
  * Migrate CMakeLists.txt, package.xml to ROS 2
  * ROS 2 API Migration (Logging, messages, node, tf2)
  * Implement ROS 2 message serialization
  * Hotfix for MD5sum message type matching
  * Enable CI: clang-format, ament_lint on Foxy
* Contributors: Yu Yan

0.9.4 (2020-04-25)
------------------
* Cleanup: fix catkin_lint warnings, remove obsolete test folder
* Fix unused-parameter warnings (`#44 <https://github.com/ros-planning/warehouse_ros/issues/44>`_)
* Bump required cmake version (`#45 <https://github.com/ros-planning/warehouse_ros/issues/45>`_)
* Contributors: Michael Görner, Robert Haschke

0.9.3 (2019-08-18)
------------------
* Fix const char* -> std::string conversion
* Fix install location for warehouse_ros. (`#43 <https://github.com/ros-planning/warehouse_ros/issues/43>`_)
* Contributors: Robert Haschke, Sean Yen

0.9.2 (2018-12-07)
------------------
* Fix various smaller issues. (`#41 <https://github.com/ros-planning/warehouse_ros/issues/41>`_)
  * fix guard name
  * virtual destructor for abstract class
  * use managed pointers - createUniqueInstance()
  * switch to C++11
  * clang-tidy modernize-use-override
* Contributors: Robert Haschke

0.9.1 (2018-10-17)
------------------
* fix missing return value (`#40 <https://github.com/ros-planning/warehouse_ros/issues/40>`_)
* update include statements to use new pluginlib and class_loader headers (`#38 <https://github.com/ros-planning/warehouse_ros/issues/38>`_)
* Contributors: Mikael Arguedas, Robert Haschke

0.9.0 (2016-06-20)
------------------
* [fix] Omit dependency on mongo (and replace with pluginlib) `#32 <https://github.com/ros-planning/warehouse_ros/issues/22>`_
* [fix] Specifically including a header that seems to be required from Ubuntu Xenial.
* [sys] Ensure headers and libraries are present for downstream pkgs `#17 <https://github.com/ros-planning/warehouse_ros/issues/17>`_
* [sys] Update CI config to test Jade and Kinetic `#30 <https://github.com/ros-planning/warehouse_ros/issues/30>`_
* [sys] Add rostest file and configs.
* Contributors: Connor Brew, Dave Coleman, Ioan A Sucan, Isaac I.Y. Saito, Michael Ferguson, Scott K Logan

0.8.8 (2014-10-01)
------------------
* Merge pull request `#13 <https://github.com/ros-planning/warehouse_ros/issues/13>`_ from corot/master
  Issue `#11 <https://github.com/ros-planning/warehouse_ros/issues/11>`_: Add a Python library
* Merge pull request `#15 <https://github.com/ros-planning/warehouse_ros/issues/15>`_ from v4hn/shared-static-mongodb
  only export MongoDB dependency for shared mongodb-library
* only export MongoDB dependency for shared mongodb-library
  libmongoclient.a uses quite a number of other libs and the exact
  requirements can't be read from a cmake/pc file.
  Therefore it makes more sense to keep the dependency hidden from ROS
  when we use the static lib. libwarehouse_ros then provides all required functions.
  ... This is a bit like creating a libmongoclient.so, but the whole problem
  exists because debian/ubuntu don't provide this one, right?
  The shared library can - and has to - be exported as a dependency to ROS.
* Missing part of https://github.com/corot/world_canvas/issues/10:
  requires both mongodb and mongodb-dev
* Merge branch 'master' of https://github.com/corot/warehouse_ros.git
* Add kwargs also to insert so we can solves issues as
  https://github.com/corot/world_canvas/issues/13
* Add kwargs to ensure_index so we can solves issues as
  https://github.com/corot/world_canvas/issues/13
* Add python-pymongo dependency
* Issue https://github.com/corot/world_canvas/issues/11: rospy queue_size
  warnings
* Issue `#11 <https://github.com/ros-planning/warehouse_ros/issues/11>`_: Add a Python library
* Contributors: Ioan A Sucan, corot, v4hn

0.8.5 (2014-02-23)
------------------
* Fixed malloc.h inclusion on Mac OS X
* Rename README.rst to README.md
* added travis support
* Contributors: Acorn, Dave Hershberger, Ioan A Sucan, Marco Esposito

0.8.4 (2013-07-03)
------------------
* update how we find MongoDB

0.8.2 (2013-07-03)
------------------
* fix typo and use correct install location
* add config.h.in for deciding how to include mongo headers

0.8.1 (2013-07-03)
------------------
* fix linking issues (missing SSL symbols) in deps, undef defined macros
