^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package kdl_parser
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.6.4 (2023-01-10)
------------------
* Backport rcutils logger fix to Humble (`#73 <https://github.com/ros/kdl_parser/issues/73>`_)
* Contributors: Joseph Schornak

2.6.3 (2022-08-05)
------------------
* Use orocos_kdl_vendor and orocos-kdl target (`#69 <https://github.com/ros/kdl_parser/issues/69>`_)
* Contributors: Scott K Logan

2.6.2 (2022-04-05)
------------------
* Depend on orocos-kdl vendor packages  (`#58 <https://github.com/ros/kdl_parser/issues/58>`_)
* Contributors: Jacob Perron

2.6.1 (2022-01-14)
------------------
* Install includes to include/ and misc CMake fixes (`#61 <https://github.com/ros/kdl_parser/issues/61>`_)
* Contributors: Shane Loretz

2.6.0 (2021-12-17)
------------------
* Update to uncrustify 0.72 (`#60 <https://github.com/ros/kdl_parser/issues/60>`_)
* Contributors: Chris Lalancette

2.5.0 (2020-10-05)
------------------
* Remove tinyxml dependency from kdl_parser. (`#43 <https://github.com/ros/kdl_parser/issues/43>`_)
* Contributors: Chris Lalancette

2.4.1 (2020-08-07)
------------------
* Remove unused find_library call (`#40 <https://github.com/ros/kdl_parser/issues/40>`_)
* Contributors: Michael Carroll

2.4.0 (2020-05-26)
------------------
* Deprecate treeFromXml (`#8 <https://github.com/ros2/kdl_parser/issues/8>`_)
* Contributors: Dan Rose

2.3.0 (2020-04-29)
------------------
* export targets in a addition to include directories / libraries (`#6 <https://github.com/ros2/kdl_parser/issues/6>`_)
* code style only: wrap after open parenthesis if not in one line (`#5 <https://github.com/ros2/kdl_parser/issues/5>`_)
* Contributors: Dirk Thomas

2.2.0 (2018-11-20)
------------------
* Fix up missing link tags in some XML files. (`#15 <https://github.com/ros2/kdl_parser/issues/15>`_)
* Contributors: Chris Lalancette

2.1.0 (2018-06-26)
------------------
* point to the source and bugtracker used in ros2 (`#3 <https://github.com/ros2/kdl_parser/issues/3>`_)
* Contributors: Mikael Arguedas

1.12.10 (2017-05-17)
--------------------
* Use result of find_package(orocos_kdl) properly (`#200 <https://github.com/ros/robot_model/issues/200>`_)
  orocos_kdl_LIBRARY_DIRS was not set

1.12.9 (2017-04-26)
-------------------

1.12.8 (2017-03-27)
-------------------
* add Chris and Shane as maintainers (`#184 <https://github.com/ros/robot_model/issues/184>`_)
* fix missed mandatory -std=c++11 flag (`#181 <https://github.com/ros/robot_model/issues/181>`_)
  collada_parser,kdl_parser,urdf: add c++11 flag,
  collada_parser: replace typeof with ansi __typeof\_\_
  builded/tested on gentoo
  Thanks den4ix for the contribution!
* Contributors: Denis Romanchuk, William Woodall

1.12.7 (2017-01-26)
-------------------

1.12.6 (2017-01-04)
-------------------
* Now using ``urdf::*ShredPtr`` instead of ``boost::shared_ptr`` (`#144 <https://github.com/ros/robot_model/issues/144>`_)
* Contributors: Jochen Sprickerhof

1.12.5 (2016-10-27)
-------------------
* fix segfault: safely handle empty robot model (`#154 <https://github.com/ros/robot_model/issues/154>`_)
* Contributors: Robert Haschke

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

1.11.7 (2015-04-22)
-------------------

1.11.6 (2014-11-30)
-------------------
* add version dependency on orocos_kdl >= 1.3.0
* Contributors: William Woodall

1.11.5 (2014-07-24)
-------------------
* Update KDL SegmentMap interface to optionally use shared pointers
  The KDL Tree API optionally uses shared pointers on platforms where
  the STL containers don't support incomplete types.
* Contributors: Brian Jensen

1.11.4 (2014-07-07)
-------------------

1.11.3 (2014-06-24)
-------------------
* kdl_parser: Adding kdl library explicitly so that dependees can find it
* Contributors: Jonathan Bohren

1.11.2 (2014-03-22)
-------------------

1.11.1 (2014-03-20)
-------------------

1.11.0 (2014-02-21)
-------------------
* fix test at kdl_parser
* Contributors: YoheiKakiuchi

1.10.18 (2013-12-04)
--------------------
* add DEPENDS for kdl_parser
* Contributors: Ioan Sucan

1.10.16 (2013-11-18)
--------------------
* check for CATKIN_ENABLE_TESTING

1.10.15 (2013-08-17)
--------------------
* fix `#30 <https://github.com/ros/robot_model/issues/30>`_
