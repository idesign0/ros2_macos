^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package class_loader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.2.0 (2022-01-14)
------------------
* Install includes to include/ (`#191 <https://github.com/ros/class_loader/issues/191>`_)
* Fix include order for cpplint (`#192 <https://github.com/ros/class_loader/issues/192>`_)
* Update maintainers to Geoffrey Biggs and Michael Carroll (`#190 <https://github.com/ros/class_loader/issues/190>`_)
* Fix spelling mistake (`#184 <https://github.com/ros/class_loader/issues/184>`_)
* Contributors: Audrow Nash, David V. Lu!!, Jacob Perron, Shane Loretz

2.1.2 (2021-04-12)
------------------
* Remove travis. (`#182 <https://github.com/ros/class_loader/issues/182>`_)
* Contributors: Chris Lalancette

2.1.1 (2021-04-06)
------------------
* Change index.ros.org -> docs.ros.org. (`#181 <https://github.com/ros/class_loader/issues/181>`_)
* Contributors: Chris Lalancette

2.1.0 (2021-03-18)
------------------
* Fix ternary null check found by clang static analysis (`#176 <https://github.com/ros/class_loader/issues/176>`_)
* Update QD to QL 1 (`#177 <https://github.com/ros/class_loader/issues/177>`_)
* Updated console_bridge QL in QD
* Update package maintainers. (`#169 <https://github.com/ros/class_loader/issues/169>`_)
* enable building a static library (`#163 <https://github.com/ros/class_loader/issues/163>`_)
* Update Quality Declaration to reflect QL 2 (`#160 <https://github.com/ros/class_loader/issues/160>`_).
* Increase coverage with a graveyard behavior test and unmanaged instance test (`#159 <https://github.com/ros/class_loader/issues/159>`_)
* Add Security Vulnerability Policy pointing to REP-2006. (`#157 <https://github.com/ros/class_loader/issues/157>`_)
* Clean up and improve documentation (`#156 <https://github.com/ros/class_loader/issues/156>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Dirk Thomas, Michel Hidalgo, Stephen Brawner, ahcorde, brawner

2.0.1 (2020-05-26)
------------------
* Added QD to doxygen related pages (`#155 <https://github.com/ros/class_loader/issues/155>`_)
* Updated class_loader QD (`#152 <https://github.com/ros/class_loader/issues/152>`_)
* fix copy and paste error (`#154 <https://github.com/ros/class_loader/issues/154>`_)
* Fixed warning (`#151 <https://github.com/ros/class_loader/issues/151>`_)
* Increased code coverage (`#141 <https://github.com/ros/class_loader/issues/141>`_)
* Added Doxyfile (`#148 <https://github.com/ros/class_loader/issues/148>`_)
* Added quality declaration draft (`#142 <https://github.com/ros/class_loader/issues/142>`_)
* Contributors: Alejandro Hernández Cordero, Tully Foote

2.0.0 (2020-04-25)
------------------
* Export CMake targets in a addition to include directories / libraries. (`#147 <https://github.com/ros/class_loader/issues/147>`_)
* Fixed references to poco in error strings. (`#144 <https://github.com/ros/class_loader/issues/144>`_)
* Removed poco dependency. Shared library management is now provided by rcpputils. (`#139 <https://github.com/ros/class_loader/issues/139>`_)
* Add missing LICENSE file, matching 3-clause BSD (`#137 <https://github.com/ros/class_loader/issues/137>`_)
* Code style change: wrap after open parenthesis if not in one line (`#138 <https://github.com/ros/class_loader/issues/138>`_)
* Fix Travis on macOS. (`#135 <https://github.com/ros/class_loader/issues/135>`_)
* Use .empty() to check for an empty string. (`#132 <https://github.com/ros/class_loader/issues/132>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Dirk Thomas, Jorge Perez

1.4.0 (2019-09-18)
------------------
* Fixed setting AbstractMetaObjectBase base class typeid. (`#127 <https://github.com/nuclearsandwich/class_loader/issues/127>`_)
* Corrected export of class_loader library. (`#129 <https://github.com/nuclearsandwich/class_loader/issues/129>`_)
* Reduced the number of threads spun up in stress test. (`#128 <https://github.com/nuclearsandwich/class_loader/issues/128>`_)
* Contributors: Emerson Knapp, Shane Loretz, bpwilcox

1.3.1 (2019-05-08)
------------------
* Using ament_target_dependencies when possible (`#124 <https://github.com/ros/class_loader/issues/124>`_)
* Contributors: ivanpauno

1.3.0 (2019-04-12)
------------------
* Updated test configuration to check copyright of files where possible. (`#123 <https://github.com/ros/class_loader/issues/123>`_)
* Updated to use ament_target_dependencies where possible. (`#121 <https://github.com/ros/class_loader/issues/121>`_)
* Contributors: ivanpauno, jpsamper2009

1.2.0 (2018-11-16)
------------------
* Updated maintainer to Steven! Ragnarok the maintainer (`#107 <https://github.com/ros/class_loader/issues/107>`_)
* Added free impl\_ in AbstractMetaObjectBase destructor (`#103 <https://github.com/ros/class_loader/issues/103>`_)
* Overhauled CI.u (`#106 <https://github.com/ros/class_loader/issues/106>`_)
* Fixed spacing to comply with uncrusity 0.67 (`#99 <https://github.com/ros/class_loader/issues/99>`_)
* Updated to use console_bridge_vendor (`#98 <https://github.com/ros/class_loader/issues/98>`_)
* Contributors: Chris Ye, Mikael Arguedas

0.3.2 (2015-04-22)
------------------
* Fixed wrong handling of false statement (pkg-config was not installed)
* Make catkin optional again
* Contributors: Esteve Fernandez, Janosch Machowinski, Matthias Goldhoorn

0.3.1 (2014-12-23)
------------------
* Depend on boost
* Use FindPoco.cmake from ros/cmake_modules
*  Honor BUILD_SHARED_LIBS and do not force building shared libraries.
* Contributors: Esteve Fernandez, Gary Servin, Scott K Logan

0.3.0 (2014-06-25)
------------------
* Use system-provided console-bridge
* Contributors: Esteve Fernandez

0.2.5 (2014-03-04)
------------------
* Changed format of debug messages so that rosconsole_bridge can correctly parse the prefix
* Improved debug output

0.2.4 (2014-02-12)
------------------
* fix race condition with multi threaded library loading (`#16 <https://github.com/ros/class_loader/issues/16>`_)

0.2.3 (2013-08-21)
------------------
* fix missing class name in logWarn output

0.2.2 (2013-07-14)
------------------
* check for CATKIN_ENABLE_TESTING (`#10 <https://github.com/ros/class_loader/issues/10>`_)
* fix find Poco to return full lib path (`#8 <https://github.com/ros/class_loader/issues/8>`_)
* add missing runtime destination for library under Windows
* add Boosst component system

0.2.1 (2013-06-06)
------------------
* improve check for Poco foundation and headers (`#7 <https://github.com/ros/class_loader/issues/7>`_)

0.2.0 (2013-03-13)
------------------
* use find_package for Poco/dl instead to make it work on other platforms
* update Poco cmake file to include libdl on non-windows systems
* No longer CATKIN_DEPEND on console_bridge

0.1.27 (2013-01-25)
-------------------
* change warning message for managed/unmanaged instance mixture in lazy loading mode

0.1.26 (2013-01-17)
-------------------
* fix all instances marked as unmanaged

0.1.25 (2013-01-16)
-------------------
* fix redundant destructor definition being pulled into plugin library for metaobjects instead of being contained with libclass_loader.so

0.1.24 (2013-01-14 15:27)
-------------------------
* fix syntax error for logInform

0.1.23 (2013-01-14 15:23)
-------------------------
* downgrade some warning messages to be info/debug

0.1.22 (2013-01-14 15:01)
-------------------------
* add safety checks for mixing of managed/unmanaged mixing as well as pointer equivalency check between graveyard and newly created metaobjects

0.1.21 (2013-01-13)
-------------------
* fix compile issue on OSX in dependent packages (`#3 <https://github.com/ros/class_loader/issues/3>`_)
* add more debug information

0.1.20 (2012-12-21 16:04)
-------------------------
* first public release for Groovy
