^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rosidl_runtime_py
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.9.3 (2022-11-07)
------------------
* [humble] Backport. Expand timestamps for std_msgs.msg.Header and builtin_interfaces.msg.Time if 'auto' and 'now' are passed as values (`#20 <https://github.com/ros2/rosidl_runtime_py/issues/20>`_)
* Contributors: Esteve Fernandez

0.9.2 (2022-03-28)
------------------
* add yaml dump flow style. (`#16 <https://github.com/ros2/rosidl_runtime_py/issues/16>`_)
* Update maintainers (`#15 <https://github.com/ros2/rosidl_runtime_py/issues/15>`_)
  * Update maintainers to Shane Loretz
  * Update Shane's email
  Co-authored-by: Shane Loretz <sloretz@openrobotics.org>
* Contributors: Audrow Nash, Tomoya Fujita

0.9.1 (2021-03-18)
------------------
* Add pytest.ini so local tests don't display warning (`#12 <https://github.com/ros2/rosidl_runtime_py/issues/12>`_)
* Contributors: Chris Lalancette

0.9.0 (2020-04-29)
------------------
* Improve exception message if value doesn't match expected type (`#11 <https://github.com/ros2/rosidl_runtime_py/issues/11>`_)
* More verbose test_flake8 error messages (same as `ros2/launch_ros#135 <https://github.com/ros2/launch_ros/issues/135>`_)
* Add special case for importing action feedback message (`#9 <https://github.com/ros2/rosidl_runtime_py/issues/9>`_)
* Limit the interface path to prevent security issues (`#7 <https://github.com/ros2/rosidl_runtime_py/issues/7>`_)
* Fix type annotations (`#8 <https://github.com/ros2/rosidl_runtime_py/issues/8>`_)
* Contributors: ChenYing Kuo, Dirk Thomas, Jacob Perron

0.8.2 (2019-11-08)
------------------
* Add .gitignore (`#5 <https://github.com/ros2/rosidl_runtime_py/issues/5>`_)
* Do not remove interface namespaces (`#6 <https://github.com/ros2/rosidl_runtime_py/issues/6>`_)
* Contributors: Gaël Écorchard, Jacob Perron

0.8.1 (2019-10-23)
------------------
* Fix get_interfaces() implementation. (`#4 <https://github.com/ros2/rosidl_runtime_py/issues/4>`_)
* Add functions for getting interface names and paths (`#3 <https://github.com/ros2/rosidl_runtime_py/issues/3>`_)
* Contributors: Jacob Perron, Michel Hidalgo

0.8.0 (2019-09-26)
------------------
* install resource marker file for package (`#2 <https://github.com/ros2/rosidl_runtime_py/issues/2>`_)
* Update setup.py URLs
* Add contributing and license files
* install package manifest (`ros2/rosidl_python#86 <https://github.com/ros2/rosidl_python/issues/86>`_)
* Rename TestConfig class to Config to avoid pytest warning (`ros2/rosidl_python#82 <https://github.com/ros2/rosidl_python/issues/82>`_)
* fix set_message_fields for numpy array (`ros2/rosidl_python#81 <https://github.com/ros2/rosidl_python/issues/81>`_)
* Fix type name queries for string types. (`ros2/rosidl_python#77 <https://github.com/ros2/rosidl_python/issues/77>`_)
* Require keyword arguments be passed by name (`ros2/rosidl_python#80 <https://github.com/ros2/rosidl_python/issues/80>`_)
* Add utility functions to get interface from identifier
* Make import_message_from_namespaced_type operate on NamespacedType (`ros2/rosidl_python#74 <https://github.com/ros2/rosidl_python/issues/74>`_)
* handle set_message_fields given some non-basic type (`ros2/rosidl_python#68 <https://github.com/ros2/rosidl_python/issues/68>`_)
* Add no_arr and no_str parameters (`ros2/rosidl_python#38 <https://github.com/ros2/rosidl_python/issues/38>`_)
* Contributors: Dirk Thomas, Jacob Perron, Jeremie Deray, Michel Hidalgo, Mikael Arguedas, Vinnam Kim, ivanpauno

0.7.6 (2019-05-30)
------------------
* fix logic around populating arrays (`ros2/rosidl_python#62 <https://github.com/ros2/rosidl_python/issues/62>`_)
* Fix the setting of array variables. (`ros2/rosidl_python#59 <https://github.com/ros2/rosidl_python/issues/59>`_)
* Contributors: Chris Lalancette, Dirk Thomas

0.7.5 (2019-05-29)
------------------

0.7.4 (2019-05-20)
------------------

0.7.3 (2019-05-08 17:57)
------------------------

0.7.2 (2019-05-08 16:58)
------------------------
* allow unicode chars in yaml output (`ros2/rosidl_python#50 <https://github.com/ros2/rosidl_python/issues/50>`_)
* add xmllint linter test (`ros2/rosidl_python#53 <https://github.com/ros2/rosidl_python/issues/53>`_)
* store types as tuple of abstract types (`ros2/rosidl_python#33 <https://github.com/ros2/rosidl_python/issues/33>`_)
* Update tests that were missed during switch to use new interfaces (`ros2/rosidl_python#51 <https://github.com/ros2/rosidl_python/issues/51>`_)
* [rosidl_runtime_py] Use new test interface definitions
* simplify code using updated definition API (`ros2/rosidl_python#45 <https://github.com/ros2/rosidl_python/issues/45>`_)
* Contributors: Dirk Thomas, Jacob Perron, Mikael Arguedas

0.7.1 (2019-04-14 12:48)
------------------------

0.7.0 (2019-04-14 05:05)
------------------------
* fix echo of numpy.number values (`ros2/rosidl_python#37 <https://github.com/ros2/rosidl_python/issues/37>`_)
* Refactor rosidl_runtime_py functions
* Add rosidl_runtime_py package
* Contributors: Dirk Thomas, Jacob Perron
