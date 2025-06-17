^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rqt_publisher
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.5.0 (2022-08-02)
------------------

1.1.3 (2021-06-29)
------------------
* Fix: Do not ignore field values introduced by the user. (`#28 <https://github.com/ros-visualization/rqt_publisher/issues/28>`_)
* Fix modern setuptools warning about dashes instead of underscores. (`#29 <https://github.com/ros-visualization/rqt_publisher/issues/29>`_)
* Contributors: Chris Lalancette, coalman321

1.1.2 (2021-04-27)
------------------
* Changed the build type to ament_python and fixed package to run with ros2 run (`#18 <https://github.com/ros-visualization/rqt_publisher/issues/18>`_)
* Drop numpy.float128 references (`#26 <https://github.com/ros-visualization/rqt_publisher/issues/26>`_)
* Contributors: Alejandro Hernández Cordero, Michel Hidalgo

1.1.1 (2021-03-16)
------------------
* Use rosidl_runtime_py instead of rqt_py_common where possible (`#24 <https://github.com/ros-visualization/rqt_publisher/issues/24>`_)
* Add now() to evaluation (`#22 <https://github.com/ros-visualization/rqt_publisher/issues/22>`_)
* Contributors: Ivan Santiago Paunovic, Yossi Ovcharik

1.1.0 (2019-10-03)
------------------
* fix setting expressions on sequence items (`#16 <https://github.com/ros-visualization/rqt_publisher/issues/16>`_)

1.0.3 (2018-12-13)
------------------
* Prepending node namespace to relative topics and adding validation for topic names (`#7 <https://github.com/ros-visualization/rqt_publisher/issues/7>`_)
* Contributors: brawner

1.0.2 (2018-12-12)
------------------
* fix cleaning up publisher on shutdown (`#6 <https://github.com/ros-visualization/rqt_publisher/issues/6>`_)

1.0.1 (2018-12-12)
------------------
* destroy publishers on removal (`#4 <https://github.com/ros-visualization/rqt_publisher/issues/4>`_)
* Contributors: Mike Lautman

1.0.0 (2018-12-11)
------------------
* Port to ROS2 (`#2 <https://github.com/ros-visualization/rqt_publisher/issues/2>`_)
* autopep8 (`#1 <https://github.com/ros-visualization/rqt_publisher/issues/1>`_)
* Contributors: Mike Lautman, brawner

0.4.8 (2017-04-24)
------------------

0.4.7 (2017-03-02)
------------------

0.4.6 (2017-02-27)
------------------

0.4.5 (2017-02-03)
------------------

0.4.4 (2017-01-24)
------------------
* use Python 3 compatible syntax (`#421 <https://github.com/ros-visualization/rqt_common_plugins/pull/421>`_)

0.4.3 (2016-11-02)
------------------

0.4.2 (2016-09-19)
------------------
* use checkbox for boolean types (`#372 <https://github.com/ros-visualization/rqt_common_plugins/issues/372>`_)

0.4.1 (2016-05-16)
------------------

0.4.0 (2016-04-27)
------------------
* Support Qt 5 (in Kinetic and higher) as well as Qt 4 (in Jade and earlier) (`#359 <https://github.com/ros-visualization/rqt_common_plugins/pull/359>`_)

0.3.13 (2016-03-08)
-------------------
* fixed rqt_publisher plugin to fill message slots for individual fields of primitive arrays
* use proper icon names for add/remove
* Contributors: Robert Haschke, Vincent Rabaud

0.3.12 (2015-07-24)
-------------------

0.3.11 (2015-04-30)
-------------------

0.3.10 (2014-10-01)
-------------------
* update plugin scripts to use full name to avoid future naming collisions

0.3.9 (2014-08-18)
------------------

0.3.8 (2014-07-15)
------------------

0.3.7 (2014-07-11)
------------------
* fix compatibility with Groovy, use queue_size for Python publishers only when available (`#243 <https://github.com/ros-visualization/rqt_common_plugins/issues/243>`_)
* export architecture_independent flag in package.xml (`#254 <https://github.com/ros-visualization/rqt_common_plugins/issues/254>`_)

0.3.6 (2014-06-02)
------------------
* use queue_size for Python publishers

0.3.5 (2014-05-07)
------------------

0.3.4 (2014-01-28)
------------------

0.3.3 (2014-01-08)
------------------
* add groups for rqt plugins, renamed some plugins (`#167 <https://github.com/ros-visualization/rqt_common_plugins/issues/167>`_)

0.3.2 (2013-10-14)
------------------
* fix regression of 0.3.1 (rospack not defined)

0.3.1 (2013-10-09)
------------------
* improve performance to fill combo box with message types (`#177 <https://github.com/ros-visualization/rqt_common_plugins/issues/177>`_)

0.3.0 (2013-08-28)
------------------

0.2.17 (2013-07-04)
-------------------

0.2.16 (2013-04-09 13:33)
-------------------------

0.2.15 (2013-04-09 00:02)
-------------------------

0.2.14 (2013-03-14)
-------------------

0.2.13 (2013-03-11 22:14)
-------------------------

0.2.12 (2013-03-11 13:56)
-------------------------

0.2.11 (2013-03-08)
-------------------

0.2.10 (2013-01-22)
-------------------

0.2.9 (2013-01-17)
------------------

0.2.8 (2013-01-11)
------------------

0.2.7 (2012-12-24)
------------------

0.2.6 (2012-12-23)
------------------

0.2.5 (2012-12-21 19:11)
------------------------

0.2.4 (2012-12-21 01:13)
------------------------

0.2.3 (2012-12-21 00:24)
------------------------

0.2.2 (2012-12-20 18:29)
------------------------

0.2.1 (2012-12-20 17:47)
------------------------

0.2.0 (2012-12-20 17:39)
------------------------
* first release of this package into groovy
