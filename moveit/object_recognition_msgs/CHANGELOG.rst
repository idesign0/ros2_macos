^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package object_recognition_msgs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.0.0 (2020-07-23)
-----------
* Merge pull request `#11 <https://github.com/wg-perception/object_recognition_msgs/issues/11>`_ from PickNikRobotics/master
  Port to ROS2
* Suppress -Wredundant-decls warnings (`#1 <https://github.com/wg-perception/object_recognition_msgs/issues/1>`_)
* fixing msg port
* Update package.xml build tool & enable message generation
* Update package.xml format
* Update package.xml dependencies
* Update CMakeLists package command
* Update CMakeLists file list & interface generator command
* Update CMakeLists find_package list
* Default to C++ 14 & cmake C++ flags
* Update CMakeLists cmake version
* Specify Header origin
* Contributors: Jonathan Binney, Michael Lautman, Yu, Yan, ibaiape

0.4.2 (2020-04-23)
------------------
* Bump CMake version to avoid CMP0048
* Contributors: Jonathan Binney, Shane Loretz

0.4.1 (2014-05-11)
------------------
* have the package be architecture independent
* Contributors: Vincent Rabaud

0.4.0 (2013-12-08)
------------------
* "0.4.0"
* fixes `#3 <https://github.com/wg-perception/object_recognition_msgs/issues/3>`_ by moving docs to object_recognition_ros
  remove docs as their built depends on ecto. Put everything in object_recognition_ros
* let message generation handle Python
* no need for stack.xml now that Fuerte support is dropped
* remove redundant info in Table.msg
* add headers to the Table messages
* drop Fuerte support
* change email address
* Contributors: Vincent Rabaud

0.3.19 (2013-03-28)
-------------------
* use the mesh from shape_msgs
* Contributors: Vincent Rabaud

0.3.18 (2013-03-12)
-------------------

0.3.17 (2013-03-08)
-------------------
* rename the type to key to keep the DB paradigm
* add dependencies
* update the dependencies
* get the compile to compile even without ecto
* rename ObjectId to ObjectType
* add basic docs
* no need for the include folder that was only used by tabletop
* Contributors: Vincent Rabaud

0.3.16 (2013-02-26 20:19)
-------------------------
* add the std_msgs dependency
* Contributors: Vincent Rabaud

0.3.15 (2013-02-26 11:19)
-------------------------
* fix unneeded dependencies
* Contributors: Vincent Rabaud

0.3.14 (2013-02-24)
-------------------
* have the ecto cells be generated in object_recognition_ros
* CMake cleanups
* Contributors: Vincent Rabaud

0.3.13 (2013-01-13)
-------------------
* use the proper catkin variable
* Contributors: Vincent Rabaud

0.3.12 (2013-01-04)
-------------------
* remove some warnings
* remove some compilation error
* added an optional ROI into the object recognition action
* add an other conversion routine
* Contributors: Tommaso Cavallari, Vincent Rabaud

0.3.11 (2012-11-18 17:06)
-------------------------
* install to the right location on Fuerte
* Contributors: Vincent Rabaud

0.3.10 (2012-11-18 16:50)
-------------------------
* try to fix the Unknown CMake command "find_program_required".
* added header to RecognizedObjectArray
* remove the copyright tag
* Contributors: Tommaso Cavallari, Vincent Rabaud

0.3.9 (2012-11-03)
------------------
* fix the install paths
* Contributors: Vincent Rabaud

0.3.8 (2012-11-01)
------------------
* fix install on Fuerte
* find ecto so that we can check whether we are on Fuerte
* Contributors: Vincent Rabaud

0.3.7 (2012-10-30)
------------------
* call generate_messages before catkin_package to comply to the new API
* Contributors: Vincent Rabaud

0.3.6 (2012-10-11)
------------------
* fix dependencies
* Contributors: Vincent Rabaud

0.3.5 (2012-10-10)
------------------
* remove support for Electric which was broken anyway since we are using Shape from Fuerte and above
* comply to the new API
* comply to the new catkin API
* Contributors: Vincent Rabaud

0.3.4 (2012-09-08)
------------------
* have code work with Electric/Fuerte/Groovy
* use the cleaner pubsub API
* use the new ectomodule API
* remove the Shape msg, as mentioned before, this is in common_msgs now
* add the manifest so that we can build against it on fuerte with rosbuild
* Merge branch 'master' of https://github.com/wg-perception/object_recognition_msgs
* updated shape_conversions to use the common_msgs shape message
  instead of the arm_navigation one
* actually build the message
* Forgotten shape message
* Contributors: Jonathan Binney, Mac Mason, Vincent Rabaud

0.3.3 (2012-07-02)
------------------
* use the mesh message from common_msgs
* fix the new langs/langs-dev dependencies
* copy from the current dir
* Contributors: Vincent Rabaud

0.3.2 (2012-06-06)
------------------
* fix bad install of the .msg
* Contributors: Vincent Rabaud

0.3.1 (2012-06-04)
------------------

0.3.0 (2012-04-29)
------------------
* bump the version
* make the Shape be part of OR and not arm_navigation_msgs
* Contributors: Vincent Rabaud

0.2.0 (2012-04-10)
------------------
* bump version number
* use the proper macro to create the pub/sub cells
* add the missing array
* add more __init__ for electric
* make sure to use the _msgs msgs
* more quirks
* fix a few quirks
* build messages for Table
* do not depend on tabletop
* add some python stuff
* add the table msgs for now
* have cells publishing/subscribing to the msgs
* fix typos
* Update msg/RecognizedObject.msg
* and we do need ecto for electric ......
* no need for action/srv on electric
* find ecto to get find_ros_package
* update some dependencies
* use arm_navigation_msgs properly
* include Jenny's comments
* clearner messages and server
* fix th bad action msg
* clean the different messages
* do not generate action files for electric
* fix more electric stuff
* fix typo
* more fixes for electric
* fix the bad recognition of electric
* Contributors: Vincent Rabaud, pantofaru

0.1.0 (2012-03-17)
------------------
* create a special package for the messages
* Contributors: Vincent Rabaud
