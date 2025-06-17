^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tlsf_cpp
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.13.0 (2022-03-01)
-------------------
* Install includes to include/${PROJECT_NAME} (`#114 <https://github.com/ros2/realtime_support/issues/114>`_)
* Contributors: Shane Loretz

0.12.1 (2022-01-14)
-------------------

0.12.0 (2021-12-17)
-------------------
* Export modern CMake targets (`#110 <https://github.com/ros2/realtime_support/issues/110>`_)
* Remove the use of malloc hooks from the tlsf_cpp tests. (`#109 <https://github.com/ros2/realtime_support/issues/109>`_)
* Contributors: Chris Lalancette, Shane Loretz

0.11.0 (2021-03-18)
-------------------
* Add in the Apache license to tlsf_cpp. (`#108 <https://github.com/ros2/realtime_support/issues/108>`_)
* Contributors: Chris Lalancette

0.10.1 (2020-12-08)
-------------------

0.10.0 (2020-06-17)
-------------------

0.9.0 (2020-04-30)
------------------
* avoid new deprecations (`#92 <https://github.com/ros2/realtime_support/issues/92>`_)
* code style only: wrap after open parenthesis if not in one line (`#88 <https://github.com/ros2/realtime_support/issues/88>`_)
* Contributors: Dirk Thomas, William Woodall

0.8.2 (2019-11-13)
------------------
* Fix clang thread safety warning (`#86 <https://github.com/ros2/realtime_support/issues/86>`_)
* disable intra process test until intra process manager fixed up (`#84 <https://github.com/ros2/realtime_support/issues/84>`_)
* Contributors: Anas Abou Allaban, William Woodall

0.8.1 (2019-10-23)
------------------
* Fix Intra-Process API (`#80 <https://github.com/ros2/realtime_support/issues/80>`_)
* Contributors: Alberto Soragna

0.8.0 (2019-09-26)
------------------

0.7.1 (2019-05-08)
------------------
* Changes to avoid deprecated API's (`#77 <https://github.com/ros2/realtime_support/issues/77>`_)
* Avoid deprecated publish signature (`#76 <https://github.com/ros2/realtime_support/issues/76>`_)
* Contributors: William Woodall

0.7.0 (2019-04-14)
------------------
* Move away from deprecated rclcpp APIs (`#74 <https://github.com/ros2/realtime_support/issues/74>`_)
* Update for parameter idiom. (`#70 <https://github.com/ros2/realtime_support/issues/70>`_)
* Contributors: Emerson Knapp, Michael Carroll

0.6.0 (2018-11-20)
------------------
* Fix spacing to comply with uncrustify 0.67 (`#67 <https://github.com/ros2/realtime_support/issues/67>`_)
* Contributors: Mikael Arguedas

0.5.0 (2018-06-27)
------------------
* Update the maintainer (`#65 <https://github.com/ros2/realtime_support/issues/65>`_)
* Comply with new rclcpp subscription templates (`#64 <https://github.com/ros2/realtime_support/issues/64>`_)
* Fix non return code issue for main() function (`#63 <https://github.com/ros2/realtime_support/issues/63>`_)
* initial_values in node constructor (`#62 <https://github.com/ros2/realtime_support/issues/62>`_)
* Add cli args to Node constructor (`#61 <https://github.com/ros2/realtime_support/issues/61>`_)
* Fix test_tlsf__rmw_opensplice_cpp (`#59 <https://github.com/ros2/realtime_support/issues/59>`_)
* Contributors: Chris Lalancette, Karsten Knese, Shane Loretz, cshen, michielb

0.4.0 (2017-12-08)
------------------
* Update for rclcpp namespace removals (`#58 <https://github.com/ros2/realtime_support/issues/58>`_)
    * Remove ::publisher:: namespace
    * Remove node:: namespace
    * Remove utilities:: namespace
* Update style to match latest uncrustify (`#57 <https://github.com/ros2/realtime_support/issues/57>`_)
* 0.0.3
* Add missing dependencies on rmw and std_msgs (`#55 <https://github.com/ros2/realtime_support/issues/55>`_)
* Bump version for package consistency within repo (`#54 <https://github.com/ros2/realtime_support/issues/54>`_)
* Use CMAKE_X_STANDARD and check compiler rather than platform
* Pass namespace to rclcpp::Node constructors (`#53 <https://github.com/ros2/realtime_support/issues/53>`_)
* Use rmw implementation (`#51 <https://github.com/ros2/realtime_support/issues/51>`_)
* C++14 (`#50 <https://github.com/ros2/realtime_support/issues/50>`_)
* No need for test_depend if already exec_depend (`#49 <https://github.com/ros2/realtime_support/issues/49>`_)
* Consistent naming when using CMake variable for rmw implementation (`#48 <https://github.com/ros2/realtime_support/issues/48>`_)
* Fix spelling error in target suffix (`#47 <https://github.com/ros2/realtime_support/issues/47>`_)
* from ros2/rename_executable
* Add tlsf prefix to allocator_example target (`#46 <https://github.com/ros2/realtime_support/issues/46>`_)
* Remove usage of internal variables and noops (`#43 <https://github.com/ros2/realtime_support/issues/43>`_)
* Update schema url
* Add schema to manifest files
* Require CMake 3.5 (`#42 <https://github.com/ros2/realtime_support/issues/42>`_)
* Clean up allocator for gcc 5.3 (`#40 <https://github.com/ros2/realtime_support/issues/40>`_)
* Disable on Android (`#41 <https://github.com/ros2/realtime_support/issues/41>`_)
* Clean up workaround for old gcc compiler
* Remove warnings on g++5.3
* Use RCL_ASSERT_RMW_ID_MATCHES to ensure correct rmw implementation is being used (`#39 <https://github.com/ros2/realtime_support/issues/39>`_)
* Remove unneeded get_rmw_typesupport call (`#35 <https://github.com/ros2/realtime_support/issues/35>`_)
* Use CTest BUILD_TESTING (`#38 <https://github.com/ros2/realtime_support/issues/38>`_)
* Remove non-existent target dependency (`#36 <https://github.com/ros2/realtime_support/issues/36>`_)
* Remove extraneous argument (`#34 <https://github.com/ros2/realtime_support/issues/34>`_)
* Use target suffix for rclcpp (`#32 <https://github.com/ros2/realtime_support/issues/32>`_)
* Export the headers for isolated build (`#31 <https://github.com/ros2/realtime_support/issues/31>`_)
* Export tlsf package (`#30 <https://github.com/ros2/realtime_support/issues/30>`_)
* Fix style
* Remove obsolete lines
* Merge pull request  from ros2/waitset_handle
* Refactor for executor arguments change (`#18 <https://github.com/ros2/realtime_support/issues/18>`_)
* Ignore fastrtps (`#28 <https://github.com/ros2/realtime_support/issues/28>`_)
* Uncrustify (`#25 <https://github.com/ros2/realtime_support/issues/25>`_)
* Fix name of test class (`#23 <https://github.com/ros2/realtime_support/issues/23>`_)
* Add rmw impl suffix to test names (`#19 <https://github.com/ros2/realtime_support/issues/19>`_)
* Reorganize realtime_support repository (`#16 <https://github.com/ros2/realtime_support/issues/16>`_)
    * Add tlsf_cpp repo
* Contributors: Brian Gerkey, Dirk Thomas, Esteve Fernandez, Jackie Kay, Mikael Arguedas, Morgan Quigley, Steven! Ragnarök, William Woodall, dhood, gerkey
