^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package kinematics_interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.4.0 (2025-05-23)
------------------
* Use CMake targets for eigen (backport `#50 <https://github.com/ros-controls/kinematics_interface/issues/50>`_) (`#55 <https://github.com/ros-controls/kinematics_interface/issues/55>`_)
* Contributors: mergify[bot]

0.3.0 (2024-02-19)
------------------
* Move definition logger to cpp to avoid "multiple definition" linker error (backport `#21 <https://github.com/ros-controls/kinematics_interface/issues/21>`_) (`#32 <https://github.com/ros-controls/kinematics_interface/issues/32>`_)
* Contributors: mergify[bot]

0.2.0 (2023-11-16)
------------------

0.1.0 (2023-01-27)
------------------
* Fix overriding of install (`#13 <https://github.com/ros-controls/kinematics_interface/issues/13>`_)
* [CI] Fix and update formatting workflow and hooks (`#14 <https://github.com/ros-controls/kinematics_interface/issues/14>`_)
* Contributors: Dr. Denis, Tyler Weaver

0.0.2 (2022-10-21)
------------------

0.0.1 (2022-10-11)
------------------
* Make formatting of comments unified and nice. (`#11 <https://github.com/ros-controls/kinematics_interface/issues/11>`_)
* Enable interface to use Eigen per default (`#6 <https://github.com/ros-controls/kinematics_interface/issues/6>`_)
* Add CI setup to the repository. (`#8 <https://github.com/ros-controls/kinematics_interface/issues/8>`_)
* Update CMakeFiles, correct deps and formatting. (`#10 <https://github.com/ros-controls/kinematics_interface/issues/10>`_)
* add pre-commit to repo (`#7 <https://github.com/ros-controls/kinematics_interface/issues/7>`_)
* Fix more indentations and alphabetize
* Update dependencies and maintainers
* Kinematics interface with KDL implementation
* change directory name and use header only interface
* changed plugin name to kinematics_interface_kdl/KDLKinematics
* add calculate_jacobian as new interface method
* added argument to specify which link the Jacobian is calculated with respect to
* made interface stateless, changed method name, fixed small bugs
* updated README and cleaned up code
* added KDL plugin for kinematics interface
* Contributors: Paul Gesel, Andy Zelenak, Bence Magyar, Denis Štogl
