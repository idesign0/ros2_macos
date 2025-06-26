^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package random_numbers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.0.4 (2025-06-06)
------------------
* Fix propagation of Boost::random dependency
* Contributors: Robert Haschke

2.0.3 (2025-06-01)
------------------
* Fix declaration of include directories
* Contributors: Robert Haschke

2.0.2 (2025-05-28)
------------------
* Modernize cmake (`#50 <https://github.com/ros-planning/random_numbers/issues/50>`_)
* Replace deprecated ament_target_libraries with target_link_libraries (`#47 <https://github.com/ros-planning/random_numbers/issues/47>`_)
* Contributors: David V. Lu, mosfet80

2.0.1 (2021-05-24)
------------------
* Support rolling on CI (`#32 <https://github.com/ros-planning/random_numbers/issues/32>`_)
* Migrate to GitHub Actions (`#30 <https://github.com/ros-planning/random_numbers/issues/30>`_)
* Fix Travis README badge (`#26 <https://github.com/ros-planning/random_numbers/issues/26>`_)
* Enable WINDOWS_EXPORT_ALL_SYMBOLS property for MSVC support (`#23 <https://github.com/ros-planning/random_numbers/issues/23>`_)
* Declare specific boost depedencies (`#22 <https://github.com/ros-planning/random_numbers/issues/22>`_)
  * removed unnecessary libboost-math libraries as only headers-only parts are used
* Fix clang-format, ament_lint_cmake (`#25 <https://github.com/ros-planning/random_numbers/issues/25>`_)
* Contributors: Henning Kayser, Lior Lustgarten, Mikael Arguedas, Robert Haschke, Tyler Weaver, Vatan Aksoy Tezer

2.0.0 (2020-11-23)
------------------
* [maint] Run Travis on Foxy and check ament_lint
* Contributors: Henning Kayser

1.0.0 (2020-07-10)
------------------
* Fix ROS2 port (`#18 <https://github.com/ros-planning/random_numbers/issues/18>`_)
* Update Travis for ROS2 (`#14 <https://github.com/ros-planning/random_numbers/issues/14>`_, `#16 <https://github.com/ros-planning/random_numbers/issues/16>`_)
* Fix shared library install path (`#13 <https://github.com/ros-planning/random_numbers/issues/13>`_)
* Contributors: Henning Kayser, Mike Lautman, Robert Haschke, Sean Yen

0.3.2 (2018-02-27)
------------------
* Update maintainership. (`#11 <https://github.com/ros-planning/random_numbers/issues/11>`_)
* Contributors: Steven! Ragnarök

0.3.1 (2016-04-04)
------------------
* Merge pull request `#10 <https://github.com/ros-planning/random_numbers/issues/10>`_ from jspricke/cmake_lib
  Use catkin variables for install dirs
* Contributors: Dave Coleman, Jochen Sprickerhof

0.3.0 (2014-09-05)
------------------
* Update README.md with Documentation
* Allow the randomly generated seed to be saved so that experiments / benc...
* Initialize static int to 0
* Save the first_seed even when passed in manually.
* Allow the randomly generated seed to be saved so that experiments / benchmarks can be recreated in the future
* Added ability to specify random number generator seed for stochastic behavior
* Added travis build status indicator in README.md
* Contributors: Dave Coleman, Dave Hershberger, Ioan A Sucan

0.2.0 (2013-07-16)
------------------
* Merge pull request `#2 <https://github.com/ros-planning/random_numbers/issues/2>`_ from wjwwood/patch-1
  Fix linkedit error on OS X with newer versions of Boost
* Fix linkedit error on OS X with newer versions of Boost
  When building `random_numbers` on OS X with Boost 1.53.0 I get:
  ```
  ==> Processing catkin package: 'random_numbers'
  ==> Creating build directory: 'build_isolated/random_numbers'
  ==> Building with env: '/Users/william/moveit_ws/install_isolated/env.sh'
  ==> cmake /Users/william/moveit_ws/src/random_numbers -DCATKIN_DEVEL_PREFIX=/Users/william/moveit_ws/devel_isolated/random_numbers -DCMAKE_INSTALL_PREFIX=/Users/william/moveit_ws/install_isolated
  -- The C compiler identification is Clang 4.2.0
  -- The CXX compiler identification is Clang 4.2.0
  -- Check for working C compiler: /usr/bin/cc
  -- Check for working C compiler: /usr/bin/cc -- works
  -- Detecting C compiler ABI info
  -- Detecting C compiler ABI info - done
  -- Check for working CXX compiler: /usr/bin/c++
  -- Check for working CXX compiler: /usr/bin/c++ -- works
  -- Detecting CXX compiler ABI info
  -- Detecting CXX compiler ABI info - done
  -- Using CATKIN_DEVEL_PREFIX: /Users/william/moveit_ws/devel_isolated/random_numbers
  -- Using CMAKE_PREFIX_PATH: /Users/william/moveit_ws/install_isolated
  -- This workspace overlays: /Users/william/moveit_ws/install_isolated
  -- Found PythonInterp: /usr/bin/python (found version "2.7.2")
  -- Found PY_em: /Library/Python/2.7/site-packages/em.pyc
  -- Found gtest: gtests will be built
  -- Using CATKIN_TEST_RESULTS_DIR: /Users/william/moveit_ws/build_isolated/random_numbers/test_results
  -- catkin 0.5.65
  WARNING: 'catkin' should be listed as a buildtool dependency in the package.xml (instead of build dependency)
  -- Boost version: 1.53.0
  -- Found the following Boost libraries:
  --   date_time
  --   thread
  -- Configuring done
  -- Generating done
  -- Build files have been written to: /Users/william/moveit_ws/build_isolated/random_numbers
  ==> make -j4 -l4 in '/Users/william/moveit_ws/build_isolated/random_numbers'
  Scanning dependencies of target random_numbers
  [100%] Building CXX object CMakeFiles/random_numbers.dir/src/random_numbers.cpp.o
  Linking CXX shared library /Users/william/moveit_ws/devel_isolated/random_numbers/lib/librandom_numbers.dylib
  Undefined symbols for architecture x86_64:
  "boost::system::system_category()", referenced from:
  ___cxx_global_var_init3 in random_numbers.cpp.o
  boost::thread_exception::thread_exception(int, char const*) in random_numbers.cpp.o
  "boost::system::generic_category()", referenced from:
  ___cxx_global_var_init1 in random_numbers.cpp.o
  ___cxx_global_var_init2 in random_numbers.cpp.o
  ld: symbol(s) not found for architecture x86_64
  clang: error: linker command failed with exit code 1 (use -v to see invocation)
  make[2]: *** [/Users/william/moveit_ws/devel_isolated/random_numbers/lib/librandom_numbers.dylib] Error 1
  make[1]: *** [CMakeFiles/random_numbers.dir/all] Error 2
  make: *** [all] Error 2
  Traceback (most recent call last):
  File "./src/catkin/bin/../python/catkin/builder.py", line 658, in build_workspace_isolated
  number=index + 1, of=len(ordered_packages)
  File "./src/catkin/bin/../python/catkin/builder.py", line 443, in build_package
  install, jobs, force_cmake, quiet, last_env, cmake_args, make_args
  File "./src/catkin/bin/../python/catkin/builder.py", line 297, in build_catkin_package
  run_command(make_cmd, build_dir, quiet)
  File "./src/catkin/bin/../python/catkin/builder.py", line 186, in run_command
  raise subprocess.CalledProcessError(proc.returncode, ' '.join(cmd))
  CalledProcessError: Command '/Users/william/moveit_ws/install_isolated/env.sh make -j4 -l4' returned non-zero exit status 2
  <== Failed to process package 'random_numbers':
  Command '/Users/william/moveit_ws/install_isolated/env.sh make -j4 -l4' returned non-zero exit status 2
  Reproduce this error by running:
  ==> /Users/william/moveit_ws/install_isolated/env.sh make -j4 -l4
  Command failed, exiting.
  ```
  Adding the `system` element to the `Boost` components being found fixes this.
* fix typo
* Create README.md
* update description
* Merge pull request `#1 <https://github.com/ros-planning/random_numbers/issues/1>`_ from ablasdel/patch-1
  Update package.xml to buildtool_depend
* Update package.xml to buildtool_depend
* Added tag 0.1.3 for changeset 78f37b23c724
* Contributors: Aaron Blasdel, Tully Foote, William Woodall, isucan

0.1.3 (2012-10-12 20:13)
------------------------
* removing outdated install rule
* fixing install rule
* Added tag 0.1.2 for changeset 42db44939f5e
* Contributors: Tully Foote

0.1.2 (2012-10-12 19:50)
------------------------
* forgot rename
* Added tag 0.1.2 for changeset 79869d337273
* updating catkinization and 0.1.2
* Added tag 0.1.1 for changeset 2e564507c3d1
* Contributors: Ioan Sucan, Tully Foote

0.1.1 (2012-06-18 13:21)
------------------------
* fix manifest
* Added tag 0.1.0 for changeset a1286e23910e
* Contributors: Ioan Sucan

0.1.0 (2012-06-18 13:17)
------------------------
* add initial version
* Contributors: Ioan Sucan
