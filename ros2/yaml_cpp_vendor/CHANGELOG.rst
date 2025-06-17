^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package yaml_cpp_vendor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

8.0.2 (2023-01-10)
------------------
* Fixes policy CMP0135 warning for CMake >= 3.24 (`#35 <https://github.com/ros2/yaml_cpp_vendor/issues/35>`_) (`#41 <https://github.com/ros2/yaml_cpp_vendor/issues/41>`_)
* Contributors: mergify[bot]

8.0.1 (2022-04-13)
------------------
* Add missing dependency on yaml-cpp (`#32 <https://github.com/ros2/yaml_cpp_vendor/issues/32>`_)
* Contributors: Scott K Logan

8.0.0 (2022-02-04)
------------------
* Upgrade to yaml-cpp 0.7.0 (`#25 <https://github.com/ros2/yaml_cpp_vendor/issues/25>`_)
* Contributors: Chris Lalancette

7.1.1 (2021-12-07)
------------------
* Update maintainers to Audrow Nash (`#26 <https://github.com/ros2/yaml_cpp_vendor/issues/26>`_)
* Fix handling of CMAKE_C[XX]_FLAGS lists (`#24 <https://github.com/ros2/yaml_cpp_vendor/issues/24>`_)
* Contributors: Christophe Bedard, Scott K Logan

7.1.0 (2021-03-18)
------------------
* Always preserve source permissions in vendor packages (`#22 <https://github.com/ros2/yaml_cpp_vendor/issues/22>`_)
* Add an override flag to force vendored build (`#21 <https://github.com/ros2/yaml_cpp_vendor/issues/21>`_)
* Reapply "Use system installed yaml-cpp 0.6 if available (`#8 <https://github.com/ros2/yaml_cpp_vendor/issues/8>`_)" (`#16 <https://github.com/ros2/yaml_cpp_vendor/issues/16>`_)
* Revert "Use system installed yaml-cpp 0.6 if available (`#8 <https://github.com/ros2/yaml_cpp_vendor/issues/8>`_)" (`#15 <https://github.com/ros2/yaml_cpp_vendor/issues/15>`_)
* Use system installed yaml-cpp 0.6 if available (`#8 <https://github.com/ros2/yaml_cpp_vendor/issues/8>`_)
* Contributors: Ivan Santiago Paunovic, Scott K Logan, Sean Yen

7.0.2 (2020-05-26)
------------------
* Append to CMAKE_C_FLAGS instead of overwriting (`#11 <https://github.com/ros2/yaml_cpp_vendor/issues/11>`_)
* Export yaml-cpp via modern cmake (`#9 <https://github.com/ros2/yaml_cpp_vendor/issues/9>`_)
* Contributors: Karsten Knese, dodsonmg

7.0.1 (2020-04-27)
------------------

7.0.0 (2019-09-19)
------------------
* add .dsv file beside custom environment hook (`#7 <https://github.com/ros2/yaml_cpp_vendor/issues/7>`_)
  Signed-off-by: Dirk Thomas <dirk-thomas@users.noreply.github.com>
* Contributors: Dirk Thomas

6.0.1 (2019-05-08)
------------------
* Pass CMAKE_TOOLCHAIN_FILE if crosscompiling (`#6 <https://github.com/ros2/yaml_cpp_vendor/issues/6>`_)
* Contributors: Esteve Fernandez

6.0.0 (2019-04-11)
------------------
* Pass through compiler and c++ flags (`#4 <https://github.com/ros2/yaml_cpp_vendor/issues/4>`_)
* Contributors: Emerson Knapp

5.0.0 (2018-11-29)
------------------
* Renaming rviz_yaml_cpp_vendor to yaml_cpp_vendor (`#1 <https://github.com/ros2/yaml_cpp_vendor/issues/1>`_)
  * Change names from rviz_yaml_cpp to yaml_cpp.
* use latest yamlcpp commit to fix compilation with vc15.8.1 (`#353 <https://github.com/ros2/yaml_cpp_vendor/issues/353>`_)
* Contributors: Michael Carroll, Mikael Arguedas

4.0.1 (2018-06-28)
------------------

4.0.0 (2018-06-27)
------------------
* Fixed build with yaml-cpp installed system-wide. (`#160 <https://github.com/ros2/rviz/issues/160>`_)
* Contributors: Martin Idel, mjbogusz

3.0.0 (2018-02-07)
------------------
* Numerous changes to support Windows.
* Contributors: William Woodall

2.0.0 (2017-12-08)
------------------
* First version for ROS 2.
* Contributors: Steven! Ragnarok, William Woodall

1.12.11 (2017-08-02)
--------------------

1.12.10 (2017-06-05 17:37)
--------------------------

1.12.9 (2017-06-05 14:23)
-------------------------

1.12.8 (2017-05-07)
-------------------

1.12.7 (2017-05-05)
-------------------

1.12.6 (2017-05-02)
-------------------

1.12.5 (2017-05-01)
-------------------

1.12.4 (2016-10-27)
-------------------

1.12.3 (2016-10-19)
-------------------

1.12.2 (2016-10-18)
-------------------

1.12.1 (2016-04-20)
-------------------

1.12.0 (2016-04-11)
-------------------

1.11.14 (2016-04-03)
--------------------

1.11.13 (2016-03-23)
--------------------

1.11.12 (2016-03-22 19:58)
--------------------------

1.11.11 (2016-03-22 18:16)
--------------------------

1.11.10 (2015-10-13)
--------------------

1.11.9 (2015-09-21)
-------------------

1.11.8 (2015-08-05)
-------------------

1.11.7 (2015-03-02)
-------------------

1.11.6 (2015-02-13)
-------------------

1.11.5 (2015-02-11)
-------------------

1.11.4 (2014-10-30)
-------------------

1.11.3 (2014-06-26)
-------------------

1.11.2 (2014-05-13)
-------------------

1.11.1 (2014-05-01)
-------------------

1.11.0 (2014-03-04 21:40)
-------------------------

1.10.14 (2014-03-04 21:35)
--------------------------

1.10.13 (2014-02-26)
--------------------

1.10.12 (2014-02-25)
--------------------

1.10.11 (2014-01-26)
--------------------

1.10.10 (2013-12-22)
--------------------

1.10.9 (2013-10-15)
-------------------

1.10.7 (2013-09-16)
-------------------

1.10.6 (2013-09-03)
-------------------

1.10.5 (2013-08-28 03:50)
-------------------------

1.10.4 (2013-08-28 03:13)
-------------------------

1.10.3 (2013-08-14)
-------------------

1.10.2 (2013-07-26)
-------------------

1.10.1 (2013-07-16)
-------------------

1.10.0 (2013-06-27)
-------------------

1.9.30 (2013-05-30)
-------------------

1.9.29 (2013-04-15)
-------------------

1.9.27 (2013-03-15 13:23)
-------------------------

1.9.26 (2013-03-15 10:38)
-------------------------

1.9.25 (2013-03-07)
-------------------

1.9.24 (2013-02-16)
-------------------

1.9.23 (2013-02-13)
-------------------

1.9.22 (2013-02-12 16:30)
-------------------------

1.9.21 (2013-02-12 14:00)
-------------------------

1.9.20 (2013-01-21)
-------------------

1.9.19 (2013-01-13)
-------------------

1.9.18 (2012-12-18)
-------------------

1.9.17 (2012-12-14)
-------------------

1.9.16 (2012-11-14 15:49)
-------------------------

1.9.15 (2012-11-13)
-------------------

1.9.14 (2012-11-14 02:20)
-------------------------

1.9.13 (2012-11-14 00:58)
-------------------------

1.9.12 (2012-11-06)
-------------------

1.9.11 (2012-11-02)
-------------------

1.9.10 (2012-11-01 11:10)
-------------------------

1.9.9 (2012-11-01 11:01)
------------------------

1.9.8 (2012-11-01 10:52)
------------------------

1.9.7 (2012-11-01 10:40)
------------------------

1.9.6 (2012-10-31)
------------------

1.9.5 (2012-10-19)
------------------

1.9.4 (2012-10-15 15:00)
------------------------

1.9.3 (2012-10-15 10:41)
------------------------

1.9.2 (2012-10-12 13:38)
------------------------

1.9.1 (2012-10-12 11:57)
------------------------

1.9.0 (2012-10-10)
------------------
