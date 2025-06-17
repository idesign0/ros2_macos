^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package pybind11_vendor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.4.2 (2022-12-06)
------------------
* Add missing buildtool dependency on git (`#20 <https://github.com/ros2/pybind11_vendor/issues/20>`_)
* Contributors: Scott K Logan

2.4.1 (2022-05-10)
------------------
* Update to pybind11 2.9.1.
* Rename patch file for history continuity.
* Contributors: Steven! Ragnarök

2.4.0 (2022-03-28)
------------------
* Use sha256 hash instead of tag (`#12 <https://github.com/ros2/pybind11_vendor/issues/12>`_)
* Install headers to include/${PROJECT_NAME} (`#11 <https://github.com/ros2/pybind11_vendor/issues/11>`_)
* Contributors: Shane Loretz

2.3.0 (2022-01-14)
------------------
* Update pybind11 to 2.7.1. (`#10 <https://github.com/ros2/pybind11_vendor/issues/10>`_)
  This is the version that is shipped in Ubuntu 22.04.
* Contributors: Chris Lalancette

2.2.6 (2020-12-10)
------------------
* Update maintainers (`#7 <https://github.com/ros2/pybind11_vendor/issues/7>`_)
* Contributors: Shane Loretz

2.2.5 (2020-08-17)
------------------
* Merge pull request `#3 <https://github.com/ros2/pybind11_vendor/issues/3>`_ from ros2/fix_windows_warning
* remove passing in CMAKE_BUILD_TYPE
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* cleanup
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* do not define CMAKE_BUILD_TYPE on windows
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* suppress all developer warnings
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* suppress warning on windows
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* attempt to fix windows warning
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* Disable building pybind11 tests (`#1 <https://github.com/ros2/pybind11_vendor/issues/1>`_)
  Signed-off-by: Karsten Knese <karsten@openrobotics.org>
* Update to pybind 2.5.0 (`#2 <https://github.com/ros2/pybind11_vendor/issues/2>`_)
  Signed-off-by: Mabel Zhang <mabel@openrobotics.org>
* Create pybind11 vendor package.
  Signed-off-by: Michael Carroll <michael@openrobotics.org>
* Contributors: Karsten Knese, Mabel Zhang, Michael Carroll
