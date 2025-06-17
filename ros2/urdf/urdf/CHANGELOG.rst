^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package urdf
^^^^^^^^^^^^^^^^^^^^^^^^^^

2.6.1 (2024-08-22)
------------------
* Provide copy and move constructors for `model` (`#33 <https://github.com/ros2/urdf/issues/33>`_) (`#41 <https://github.com/ros2/urdf/issues/41>`_)
  In 4b73ae2998bec0db24aca07b0bf7fc37b8e4dae7 the copy and move
  constructors were accidentally disabled.
  This means that one must always wrap a `urdf::Model` into some form of
  pointer in order to pass it around or have it as a member.
  This commit restores the copy and move constructors, such that one is
  able to pass the `urdf::Model` around as a normal variable.
  See also:
  https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-five
  Co-authored-by: Daniel Reuter <daniel.robin.reuter@googlemail.com>
  Co-authored-by: Shane Loretz <sloretz@osrfoundation.org>
* Contributors: Kenji Brameld (TRACLabs)

2.6.0 (2022-03-01)
------------------
* Install headers to include/${PROJECT_NAME} (`#31 <https://github.com/ros2/urdf/issues/31>`_)
* Contributors: Shane Loretz

2.5.3 (2022-01-13)
------------------
* Add linter tests and fix errors (`#30 <https://github.com/ros2/urdf/issues/30>`_)
* Add in a Doxyfile to predefine macros. (`#28 <https://github.com/ros2/urdf/issues/28>`_)
* Contributors: Chris Lalancette, Jacob Perron

2.5.2 (2021-05-12)
------------------

2.5.1 (2020-12-10)
------------------
* Work around Windows min/max bug. (`#21 <https://github.com/ros2/urdf/issues/21>`_)
* Contributors: Chris Lalancette

2.5.0 (2020-10-05)
------------------
* Enable -Wall -Wextra -Wpedantic (`#20 <https://github.com/ros2/urdf/issues/20>`_)
* Add dependency on TinyXML2 (`#19 <https://github.com/ros2/urdf/issues/19>`_)
* Remove TinyXML dependency from urdf. (`#17 <https://github.com/ros2/urdf/issues/17>`_)
* Make urdf plugable and revive urdf_parser_plugin (`#13 <https://github.com/ros2/urdf/issues/13>`_)
* Contributors: Audrow Nash, Chris Lalancette, Shane Loretz

2.4.0 (2020-05-26)
------------------
* Deprecate methods that require tinyxml (`#12 <https://github.com/ros2/urdf/issues/12>`_)
* Contributors: Dan Rose

2.3.0 (2020-04-28)
------------------
* Export targets in a addition to include directories / libraries. (`#10 <https://github.com/ros2/urdf/issues/10>`_)
* Force explicit language for uncrustify. (`#9 <https://github.com/ros2/urdf/issues/9>`_)
* Code style only: wrap after open parenthesis if not in one line. (`#8 <https://github.com/ros2/urdf/issues/8>`_)
* Contributors: Dirk Thomas

2.2.0 (2018-11-20)
------------------
* Update to remove unneeded console_bridge dependency. (`#4 <https://github.com/ros2/urdf/issues/4>`_)
* Contributors: Mikael Arguedas

2.1.0 (2018-06-25)
------------------
* Update project URLs (`#2 <https://github.com/ros2/urdf/issues/2>`_)
* Contributors: Mikael Arguedas

2.0.0 (2017-12-08)
------------------
* Ported to ROS 2
* Added Windows compatibility
* Switched to ROS 2 code style
* Contributors: Chris Lalancette, Mikael Arguedas

1.12.12 (2017-11-08)
--------------------
* Switched to package format 2 and made rostest a test_depend (`#221 <https://github.com/ros/robot_model/pull/221>`_)
* Made rostest a test_depend (`#221 <https://github.com/ros/robot_model/pull/221>`_)
* Added missing dependency on tinyxml (`#213 <https://github.com/ros/robot_model/pull/213>`_)
* Contributors: Chris Lalancette, Mikael Arguedas


1.12.11 (2017-06-27)
--------------------
* Shared ptr yakkety (`#207 <https://github.com/ros/robot_model/issues/207>`_)
  * Forward declare urdf::Model when urdfdom version is > 0.4
  * Add test for upcasting from urdf::ModelSharedPtr to urdf::ModelInterfaceSharedPtr
* Contributors: Shane Loretz

1.12.10 (2017-06-24)
--------------------
* Change urdf::Model to use std::shared_ptrs in urdfdom > v0.4 (`#206 <https://github.com/ros/robot_model/issues/206>`_)
* Contributors: Dave Coleman

1.12.9 (2017-04-26)
-------------------

1.12.8 (2017-03-27)
-------------------
* Allow supplying NodeHandle for initParam (`#168 <https://github.com/ros/robot_model/issues/168>`_)
  * Allow supplying NodeHandle for initParam using new function.
  * fixed missing return statement in previous commit.
* add Chris and Shane as maintainers (`#184 <https://github.com/ros/robot_model/issues/184>`_)
* fix missed mandatory -std=c++11 flag (`#181 <https://github.com/ros/robot_model/issues/181>`_)
  collada_parser,kdl_parser,urdf: add c++11 flag,
  collada_parser: replace typeof with ansi __typeof\_\_
  builded/tested on gentoo
  Thanks den4ix for the contribution!
* Contributors: Denis Romanchuk, Piyush Khandelwal, William Woodall

1.12.7 (2017-01-26)
-------------------

1.12.6 (2017-01-04)
-------------------
* Addressed gcc6 build error in the urdf package, forward port of `#156 <https://github.com/ros/robot_model/issues/156>`_ (`#173 <https://github.com/ros/robot_model/issues/173>`_)
* Now using ``urdf::*ShredPtr`` instead of ``boost::shared_ptr`` (`#144 <https://github.com/ros/robot_model/issues/144>`_)
* Contributors: Jochen Sprickerhof, William Woodall

1.12.5 (2016-10-27)
-------------------
* Added urdf_compatibility.h header to define SharedPtr types (`#160 <https://github.com/ros/robot_model/issues/160>`_)
  This provides portability for downstream packages allowing them to use urdfdom 0.3 or 0.4.
* urdf: Explicitly cast shared_ptr to bool in unit test. (`#158 <https://github.com/ros/robot_model/issues/158>`_)
* Add smart ptr typedefs (`#153 <https://github.com/ros/robot_model/issues/153>`_)
* Addressed gcc6 build error in urdf which was related to use of the isystem flag (`#157 <https://github.com/ros/robot_model/issues/157>`_)
* Remove unneeded dependency on libpcrecpp (`#155 <https://github.com/ros/robot_model/issues/155>`_)
* Contributors: Bence Magyar, Jochen Sprickerhof, Lukas Bulwahn, Maarten de Vries, Robert Haschke

1.12.4 (2016-08-23)
-------------------

1.12.3 (2016-06-10)
-------------------

1.12.2 (2016-04-12)
-------------------

1.12.1 (2016-04-10)
-------------------

1.11.8 (2015-09-11)
-------------------
* Removed pcre hack for newer released collada-dom.
* Fixed link order of libpcrecpp.
* Contributors: Kei Okada

1.11.7 (2015-04-22)
-------------------
* Removed the exporting of Boost and pcre as they are not used in the headers, and added TinyXML because it is.
* Fixed a bug with pcrecpp on Ubuntu > 13.04.
* Contributors: Kei Okada, William Woodall

1.11.6 (2014-11-30)
-------------------
* Add install for static libs needed for Android cross-compilation
* Contributors: Gary Servin

1.11.5 (2014-07-24)
-------------------

1.11.4 (2014-07-07)
-------------------
* moving to new dependency for urdfdom and urdfdom_headers. https://github.com/ros/rosdistro/issues/4633
* Contributors: Tully Foote

1.11.3 (2014-06-24)
-------------------
* fix urdfdom_headers find_package re `ros/rosdistro#4633 <https://github.com/ros/rosdistro/issues/4633>`_
* Contributors: Tully Foote

1.11.2 (2014-03-22)
-------------------

1.11.1 (2014-03-20)
-------------------

1.11.0 (2014-02-21)
-------------------
* fix urdf files for test
* fix test at urdf
* Contributors: YoheiKakiuchi

1.10.18 (2013-12-04)
--------------------
* add DEPENDS for kdl_parser
* Contributors: Ioan Sucan

1.10.16 (2013-11-18)
--------------------
* check for CATKIN_ENABLE_TESTING
* fix for using collada_parser_plugin

1.10.15 (2013-08-17)
--------------------
* fix `#30 <https://github.com/ros/robot_model/issues/30>`_
