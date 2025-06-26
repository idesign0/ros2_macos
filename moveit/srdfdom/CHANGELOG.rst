^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package srdfdom
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.0.8 (2025-05-28)
------------------
* Modernize cmake
* Contributors: mosfet80

2.0.7 (2024-10-14)
------------------
* Fix installing headers
* Contributors: Robert Haschke

2.0.6 (2024-10-13)
------------------
* Fix linking against tinyxml2
* Install headers into include/srdfdom/srdfdom to make overlays work
* Contributors: Robert Haschke

2.0.5 (2024-10-06)
------------------
* Fix README instructions for ROS 2 (`#130 <https://github.com/moveit/srdfdom/issues/130>`_)
* Support conditional urdf/model.hpp include (`#127 <https://github.com/moveit/srdfdom/issues/127>`_)
* Remove rospy usage (`#129 <https://github.com/moveit/srdfdom/issues/129>`_)
* Add Jazzy to CI (`#128 <https://github.com/moveit/srdfdom/issues/128>`_)
* Update CMakeLists.txt (`#123 <https://github.com/moveit/srdfdom/issues/123>`_)
  - minimum cmake version: 3.8
  - use default C++17
* CI: Use clang-format-14
* CI: update pre-commit tool versions (`#125 <https://github.com/moveit/srdfdom/issues/125>`_)
* CI: drop Galactic, add Iron
* CI: Update action versions (`#121 <https://github.com/moveit/srdfdom/issues/121>`_)
* CI: update actions/checkout to version 3 (`#116 <https://github.com/moveit/srdfdom/issues/116>`_)
* Update .pre-commit-config.yaml (`#113 <https://github.com/moveit/srdfdom/issues/113>`_)
* Parse decimals in a locale-independent way (`#108 <https://github.com/moveit/srdfdom/issues/108>`_)
* Humble and formatting updates (`#107 <https://github.com/moveit/srdfdom/issues/107>`_)
* Contributors: AndyZe, Robert Haschke, Sebastian Castro, Vatan Aksoy Tezer, mosfet80

2.0.4 (2022-05-09)
------------------
* Parse <disable_default_collisions> and <enable_collisions> tags (#101, from #97)
* Fix Windows .dll install location (#98)
* Contributors: Robert Haschke, Abishalini Sivaraman, Akash, Henning Kayser

2.0.3 (2021-09-27)
------------------
* Use modern cmake (`#96 <https://github.com/ros-planning/srdfdom/issues/96>`_)
* Fix unit tests & setup.py file (`#94 <https://github.com/ros-planning/srdfdom/issues/94>`_)
* Fix use of tinyxml2_vendor (`#95 <https://github.com/ros-planning/srdfdom/issues/95>`_)
* Fixes for usage on windows (`#91 <https://github.com/ros-planning/srdfdom/issues/91>`_)
* Contributors: Akash, Jafar Abdi, Vatan Aksoy Tezer

2.0.2 (2021-05-24)
------------------
* Support rolling CI (`#85 <https://github.com/ros-planning/srdfdom/issues/85>`_)
* Contributors: Vatan Aksoy Tezer

2.0.1 (2021-04-09)
------------------
* SRDFWriter: Add functionality to generate xml for the joint properties
* Add a new JointProperties xml tag
* Add diff_drive type to virtual joints
* [fix] export cmake library install (`#76 <https://github.com/ros-planning/srdfdom/issues/76>`_)
* Contributors: David V. Lu, Jafar Abdi, Mark Moll, Robert Haschke, Tyler Weaver

2.0.0 (2020-11-23)
------------------
* [fix] Conflicting upstream dependency console_bridge
  * Add console_bridge libs with ament_target_dependencies (`#59 <https://github.com/ros-planning/srdfdom/issues/59>`_)
  * Use console_bridge_vendor dependency wrapper (`#61 <https://github.com/ros-planning/srdfdom/issues/61>`_)
* [maint] Inherit package VERSION from package.xml (`#74 <https://github.com/ros-planning/srdfdom/issues/74>`_)
* [maint] Symbol visibility for Windows support (`#69 <https://github.com/ros-planning/srdfdom/issues/69>`_)
* [maint] Proper exporting of TinyXML2 library to dependent ament packages (`#54 <https://github.com/ros-planning/srdfdom/issues/54>`_)
* [ros2-migration] Port to ROS 2 (`#52 <https://github.com/ros-planning/srdfdom/issues/52>`_)
  * Apply initial port by AcutronicsRobotics (`AcutronicRobotics:srdfdom#1 <https://github.com/AcutronicRobotics/srdfdom/issues/1>`_, `AcutronicRobotics:srdfdom#2 <https://github.com/AcutronicRobotics/srdfdom/issues/2>`_, `AcutronicRobotics:srdfdom#3 <https://github.com/AcutronicRobotics/srdfdom/issues/3>`_, `AcutronicRobotics:srdfdom#4 <https://github.com/AcutronicRobotics/srdfdom/issues/4>`_)
  * Enable Travis CI
* Contributors: Alejandro Hernandez Cordero, Anas Mchichou El Harrak, Henning Kayser, Hunter L. Allen, Josh Langsfeld, Lior Lustgarten, Patrick Beeson, Robert Haschke

0.6.2 (2020-09-09)
------------------
* [bugfix] Correctly return success in SRDFWriter::writeSRDF().
* Contributors: Robert Haschke

0.6.1 (2020-09-06)
------------------
* [bugfix] SRDFWriter: Correctly populate XML document
* [bugfix] SRDFWriter: Use locale independent conversion from double to string (`#67 <https://github.com/ros-planning/srdfdom/issues/67>`_)
* [maint]  Silence cmake warning
* Contributors: Robert Haschke

0.6.0 (2020-08-19)
------------------
* [maint] Switch from TinyXML to TinyXML2 (`#60 <https://github.com/ros-planning/srdfdom/issues/60>`_)
* [maint] add soname to library (`#63 <https://github.com/ros-planning/srdfdom/issues/63>`_)
* Contributors: Robert Haschke, Tyler Weaver

0.5.2 (2020-06-30)
------------------
* [maint]  Modernize Travis config (`#57 <https://github.com/ros-planning/srdfdom/issues/57>`_)
* [maint]  Modernize package.xml
* [maint]  Modernize python (python2 / python3 compatibility)
* [maint]  Modernize cmake and setup.py
* [maint]  Fix unittest
* [maint]  Use [[deprecated]] for better portability (`#47 <https://github.com/ros-planning/srdfdom/issues/47>`_)
* [maint]  Travis: enable ccache
* [maint]  Fix catkin_lint issues
* [maint]  Format code with clang-format (`#42 <https://github.com/ros-planning/srdfdom/issues/42>`_, `#43 <https://github.com/ros-planning/srdfdom/issues/43>`_)
* [bugfix] Parse group's robot states with C locale (`#44 <https://github.com/ros-planning/srdfdom/issues/44>`_)
* [bugfix] Trigger error in case of SRDF syntax error (`#41 <https://github.com/ros-planning/srdfdom/issues/41>`_)
* Contributors: Alejandro Hernández Cordero, Dave Coleman, Jonathan Binney, Michael Görner, Robert Haschke, Sean Yen, Simon Schmeisser, kkufieta

0.5.0 (2018-04-24)
------------------
* Switch to std::shared_ptr of C++11 (`#36 <https://github.com/ros-planning/srdfdom/issues/36>`_)
* Change log{Error,Warn} -> CONSOLE_BRIDGE_log{Error,Warn} (`#37 <https://github.com/ros-planning/srdfdom/issues/37>`_)
* Contributors: Chris Lalancette, Ian McMahon

0.4.2 (2017-01-30)
------------------
* [fix] gcc6 build error `#28 <https://github.com/ros-planning/srdfdom/issues/28>`_
* [fix] Compile with -std=c++11 (`#29 <https://github.com/ros-planning/srdfdom/issues/29>`_)
* [enhancement] cleanup urdfdom compatibility (`#27 <https://github.com/ros-planning/srdfdom/issues/27>`_)
* Contributors: Dmitry Rozhkov, Isaac I.Y. Saito, Robert Haschke, Victor Matare

0.4.1 (2016-09-22)
------------------
* [fix][system] Build failure for Ubuntu Wily and Debian Jesie (urdfdom compatibility `#25 <https://github.com/ros-planning/srdfdom/issues/25>`_)
  * test for existence of urdf typedef
  * if not existing, activate compatibility header
* Contributors: Michael Goerner, Robert Haschke

0.4.0 (2016-09-09)
------------------
* [fix] Define shared_ptr typedef (adjusting to the recent change in urdfdom) `#21 <https://github.com/ros-planning/srdfdom/issues/21>`_
* Contributors: Dave Coleman, Robert Haschke

0.3.2 (2016-08-25)
------------------
* [feat] Move SRDF-specific commands from moveit package `#14 <https://github.com/ros-planning/srdfdom/issues/14>`_
* [sys] remove ROS-dependent logging.
* [sys] Much cleanup in package.xml. `#12 <https://github.com/ros-planning/srdfdom/issues/12>`_ pkg-config is no longer used after https://github.com/ros-planning/srdfdom/commit/19b23e5900e9c179089e99caae52023f95d2fec8#diff-af3b638bc2a3e6c650974192a53c7291
* Contributors: Dave Coleman, Sarah Elliott, Robert Haschke, Isaac I.Y. Saito

0.3.1 (2016-08-01)
------------------
* Change logError to Warn if collision link missing `#10 <https://github.com/ros-planning/srdfdom/issues/10>`_ Since MoveIt continues to load anyway, it makes sense to change the unknown collision link pairs ROS Error to a ROS Warning. Everything continues to work if a specified set of collision-link pairs is missing.
* Contributors: Dave Coleman, Ian McMahon

0.3.0 (2015-06-16)
------------------
* Removed unwanted python compiled file
* Fixed path to resource in python test to work for rostest
* Fixed authors, added doc
* Fixed group_state parsing and changed chain as an aggregate
* Renamed groups as subgroups when integrated in a group
* Added the cpp tests in the python test
* Fixed missing install
* Added a python parser based on urdf_parser_py and using its reflection interface
* Contributors: Dave Coleman, Guillaume Walck

0.2.7 (2014-07-01)
------------------
* fixing dependencies for https://github.com/ros/rosdistro/issues/4633
* added travis build status indicator in README.md
* added travis support
* use FindTinyXML from cmake_module
* Contributors: Dave Coleman, Dave Hershberger, Ioan Sucan, Tully Foote

0.2.6 (2013-07-19)
------------------
* fix incorrect tag name
