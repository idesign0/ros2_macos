^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rosidl_typesupport_fastrtps_cpp
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.2.2 (2023-11-13)
------------------
* Account for alignment on `is_plain` calculations (`#108 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/108>`_) (`#110 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/110>`_)
* Contributors: mergify[bot]

2.2.1 (2023-07-18)
------------------
* Avoid redundant declarations in generated code for services and actions (backport `#102 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/102>`_) (`#104 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/104>`_)
* Contributors: mergify[bot]

2.2.0 (2022-03-30)
------------------
* Install generated headers to include/${PROJECT_NAME} (`#88 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/88>`_)
* Misc fastrtps typesupport generator cleanup (`#87 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/87>`_)
* Contributors: Shane Loretz

2.1.0 (2022-03-01)
------------------
* Install headers to include/${PROJECT_NAME} (`#86 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/86>`_)
* Contributors: Shane Loretz

2.0.4 (2022-01-13)
------------------
* Fix include order for cpplint (`#84 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/84>`_)
  * Fix include order for cpplint
  Relates to https://github.com/ament/ament_lint/pull/324
  * Use double-quotes for other includes
  This is backwards compatible with older versions of cpplint.
* Update maintainers to Shane Loretz (`#83 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/83>`_)
* Contributors: Audrow Nash, Jacob Perron

2.0.3 (2021-11-18)
------------------
* Re-introduce improvements to serialization of primitive bounded sequences for C++ type support (`#81 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/81>`_)
* Revert "Improve serialization of ... (`#79 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/79>`_)" (`#80 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/80>`_)
* Improve serialization of primitive bounded sequences in C++ type support (`#79 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/79>`_)
* Use FindPython3 explicitly instead of PythonInterp implicitly (`#78 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/78>`_)
* Contributors: Andrea Sorbini, Jorge Perez, Shane Loretz

2.0.2 (2021-08-09)
------------------
* Revert rosidl targets and dependencies exportation (`#76 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/76>`_)
  * Revert "Export rosidl_typesupport_fastrtps_c* dependencies (`#75 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/75>`_)"
  * Revert "Bundle and ensure the exportation of rosidl generated targets (`#73 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/73>`_)"
* Correctly inform that a BoundedSequence is bounded (`#71 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/71>`_)
* Contributors: Michel Hidalgo, Miguel Company

2.0.1 (2021-07-28)
------------------
* Export rosidl_typesupport_fastrtps_c* dependencies (`#75 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/75>`_)
* Contributors: Michel Hidalgo

2.0.0 (2021-07-23)
------------------
* Bundle and ensure the exportation of rosidl generated targets (`#73 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/73>`_)
* Fix Fast-RTPS C++ typesupport CLI extension (`#72 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/72>`_)
* Fastdds type support extensions (`#67 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/67>`_)
* Remove fastrtps dependency (`#68 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/68>`_)
* Contributors: Andrea Sorbini, Michel Hidalgo, Miguel Company

1.2.1 (2021-04-06)
------------------
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#69 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/69>`_)
* Contributors: shonigmann

1.2.0 (2021-03-18)
------------------
* Expose FastRTPS C++ typesupport generation via rosidl generate CLI (`#66 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/66>`_)
* Contributors: Michel Hidalgo

1.1.0 (2020-12-09)
------------------
* Update QDs with up-to-date content (`#64 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/64>`_)
* Fix item number in QD (`#59 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/59>`_)
* Update QL to 2
* Update package maintainers (`#55 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/55>`_)
* Update QD (`#53 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/53>`_)
* Add benchmark test to rosidl_typesupport_fastrtps_c/cpp (`#52 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/52>`_)
* Update exec dependencies (`#50 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/50>`_)
* Add Security Vulnerability Policy pointing to REP-2006 (`#44 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/44>`_)
* QD Update Version Stability to stable version (`#46 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/46>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Jorge Perez, Louise Poubel, Michel Hidalgo, Stephen Brawner

1.0.1 (2020-05-26)
------------------
* Revert usage of modern cmake. This breaks single typesupport builds again. (`#47 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/47>`_)
* Contributors: Ivan Santiago Paunovic

1.0.0 (2020-05-22)
------------------
* Use modern cmake to fix single typesupport builds (`#40 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/40>`_)
* Move generated headers to detail subdir (`#40 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/40>`_)
* Add tests for wstring conversion routines (`#43 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/43>`_)
* Update public API documentation (`#42 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/42>`_)
* Add feature documentation (`#41 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/41>`_)
* Add Quality Declaration and README (`#39 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/39>`_)
* Contributors: Ivan Santiago Paunovic, Scott K Logan, brawner

0.9.0 (2020-04-24)
------------------
* Export targets in addition to include directories / libraries (`#37 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/37>`_ `#38 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/38>`_)
* Update includes to use non-entry point headers from detail subdirirectory (`#36 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/36>`_)
* Use ament_cmake_ros (`#30 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/30>`_)
* Rename rosidl_generator_c namespace to rosidl_runtime_c (`#35 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/35>`_)
* Added rosidl_runtime_c depencency (`#32 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/32>`_)
* Export typesupport library in a separate cmake variable (`#34 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/34>`_)
* Contributors: Alejandro Hernández Cordero, Dirk Thomas, Ivan Santiago Paunovic

0.8.0 (2019-09-25)
------------------
* Fix typesupport for long double and wchar (`#26 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/26>`_)
* Contributors: Dirk Thomas

0.7.1 (2019-05-08)
------------------
* Add message namespace to type support struct (`#18 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/18>`_)
* Hard code size of wchar_t to 4 (`#25 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/25>`_)
* Fix size calculation for WStrings on non-Windows platforms (`#23 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/23>`_)
* Add WString support (`#22 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/22>`_)
* Simplify code using updated definition API (`#21 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/21>`_)
* Update code to match refactoring of rosidl definitions (`#20 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/20>`_)
* Remove usage of UnknownMessageType (`#19 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/19>`_)
* Contributors: Dirk Thomas, Jacob Perron, Karsten Knese, Michael Carroll

0.7.0 (2019-04-13)
------------------
* Change generators to IDL-based pipeline (`#14 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/14>`_)
* Contributors: Dirk Thomas

0.6.1 (2019-01-11)
------------------
* Change uncrustify max line length to 0 (`#17 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/17>`_)
  This is for compatibility with uncrustify v0.68.
* Contributors: Jacob Perron

0.6.0 (2018-11-16)
------------------
* Allow generated IDL files (`#12 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/12>`_)
* Enable generation of messages and services in an 'action' directory (`#11 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/11>`_)
* Remove unnecessary dll exports (`#8 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/8>`_)
* Fix the target dependency for automatic regeneration (`#7 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/7>`_)
* Add specialization of get_service_type_support_handle (`#6 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/6>`_)
* Avoid using undefined variable (`#5 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/5>`_)
* Remove more dead code (`#4 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/4>`_)
* Don't generate IDL files and remove unused code (`#2 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/2>`_)
* Contributors: Alexis Pojomovsky, Dirk Thomas, Michel Hidalgo, Miguel Company, Mikael Arguedas, Shane Loretz
