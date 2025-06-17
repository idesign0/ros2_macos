^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package kinematics_interface_kdl
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.4.0 (2025-05-23)
------------------
* Use CMake targets for eigen (backport `#50 <https://github.com/ros-controls/kinematics_interface/issues/50>`_) (`#55 <https://github.com/ros-controls/kinematics_interface/issues/55>`_)
* Contributors: mergify[bot]

0.3.0 (2024-02-19)
------------------

0.2.0 (2023-11-16)
------------------
* 🤔 Remove compile warnings and unify for-loop syntax. (`#15 <https://github.com/ros-controls/kinematics_interface/issues/15>`_)
* Contributors: Dr. Denis, Bence Magyar, Paul Gesel

0.1.0 (2023-01-27)
------------------
* Fix overriding of install (`#13 <https://github.com/ros-controls/kinematics_interface/issues/13>`_)
* Contributors: Tyler Weaver

0.0.2 (2022-10-21)
------------------

0.0.1 (2022-10-11)
------------------
* Enable interface to use Eigen per default (`#6 <https://github.com/ros-controls/kinematics_interface/issues/6>`_)
* Add CI setup to the repository. (`#8 <https://github.com/ros-controls/kinematics_interface/issues/8>`_)
* Remove duplicate of Eigen dependency.
* Update CMakeFiles, correct deps and formatting. (`#10 <https://github.com/ros-controls/kinematics_interface/issues/10>`_)
* add pre-commit to repo (`#7 <https://github.com/ros-controls/kinematics_interface/issues/7>`_)
  add pre-commit
* Fix failing KDL test (`#5 <https://github.com/ros-controls/kinematics_interface/issues/5>`_)
  * add verification for all inputs
  * add underscore to end_effector
  * add test to ensure inverse then forward calculation is approximately unit
* Fix more indentations and alphabetize
* Update dependencies and maintainers
* Kinematics interface with KDL implementation
* change directory name and use header only interface
* Contributors: Paul Gesel, Andy Zelenak, Bence Magyar, Denis Štogl
