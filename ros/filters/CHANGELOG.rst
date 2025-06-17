^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package filters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.2.1 (2025-05-14)
------------------
* Ignore unused parameter in FilterBase::reconfigureCB()
* Contributors: Christophe Bedard

2.2.0 (2025-05-01)
------------------
* Handle dynamic reconfiguration of parameters
* Enable the use of a default for parameters
* Configure github action that runs tests
* Add Windows support
* Clean up filter chain parameter loading
* Make read only configurable
* Added parameter already declared check and allowed override
* Contributors: Dominik Moss, Jeanine van Bruggen, Jonathan Binney, Silvio Traversaro

2.1.2 (2024-04-17)
------------------
* Fix the RHEL-9 build.
* Fix CMake 3.18
* Add a virtual definition of configure() in MultiChannelFilterBase.
* Contributors: Chris Lalancette, Jonathan Binney, Ryan Friedman

2.1.1 (2024-03-02)
------------------
* Provide target based dependencies in cmake for filter_base and libraries.
* Support Boost 1.71 (Ubuntu 20.04 Humble)
* Allow to iterate over RealtimeCircularBuffer.
* Change function signature to use pointer instead of array.
  This really has no downstream consequence, but makes it so that
  rosdoc2 can successfully generate documentation.
* Contributors: Chris Lalancette, Daisuke Nishimatsu, Jonathan Binney,
  Patrick Roncagliolo, Ryan Friedman, wep21

2.1.0 (2021-07-14)
------------------
* Fix compiler warnings+test failures on CI (`#56 <https://github.com/ros/filters/issues/56>`_)
* Solve statically parameter error (`#48 <https://github.com/ros/filters/issues/48>`_)
* Contributors: Jon Binney, Patrick Lascombz

2.0.1 (2021-07-08)
------------------
* Add Jon binney as a maintainer (`#54 <https://github.com/ros/filters/issues/54>`_)
* missing export includes (`#50 <https://github.com/ros/filters/issues/50>`_)
* changed free functions to be inline (`#43 <https://github.com/ros/filters/issues/43>`_)
* Update default_plugins.xml (fix plugin names)
* Contributors: Hang, Jonathan Binney, Marwan Taher, Steve Macenski

2.0.0 (2020-05-14)
------------------
* Fix a bug in the tests.
* Fixes for Foxy uncrustify.
* Address peer review comments.
* Enforce proper code style.
* Cleanup types and literals.
* Lint all sources.
* Address pending review comments.
* Misc boost fixes
* Fix wrong include guard
* FilterBase<T> to avoid name hiding
* Include what is used
* Alphabetize package deps
* Get rid of unnecessary retval
* Include what is used
* remove unnecessary ';'
* Use `empty()` instead of `size() == 0`
* `Instal` -> `Install`
* Add ament_package()
* Port filters to ROS Eloquent
* [noetic] deprecate h for hpp (`#34 <https://github.com/ros/filters/issues/34>`_)
* [noetic] Delete unused code (`#33 <https://github.com/ros/filters/issues/33>`_)
* Bump CMake version to avoid CMP0048
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Michel Hidalgo, Shane Loretz

1.9.0 (2020-03-10)
------------------
* Reduce dependency on boost (`#30 <https://github.com/ros/filters/issues/30>`_)
* Contributors: Shane Loretz

1.8.1 (2017-04-25)
------------------
* Fix warning about string type
* Contributors: Jon Binney

1.8.0 (2017-04-07)
------------------

* Remove promiscuous filter finding
  When specifying filters, you must now include the package name and exact
  filter name. Previously a workaround would search packages for any filter
  with the specified string in the filter name. This has been deprecated for
  a while, and here we're removing it. `#14 <https://github.com/ros/filters/issues/14>`
* Contributors: Jon Binney

1.7.5 (2017-03-16)
------------------
* make rostest in CMakeLists optional (`ros/rosdistro#3010 <https://github.com/ros/rosdistro/issues/3010>`_)
* check for CATKIN_ENABLE_TESTING
* Add support for boolean parameters (fix `#6 <https://github.com/ros/filters/issues/6>`_)
* Contributors: Boris Gromov, Lukas Bulwahn, Tully Foote

1.7.4 (2013-07-23)
------------------
* Remove trailing whitespace and old <cpp> export
* Removing vestigial files
* Merge pull request `#5 <https://github.com/ros/filters/issues/5>`_ from ahendrix/hydro-devel
  Fix pluginlib macros and createInstance usage.
* Fix pluginlib macros and createInstance usage.
* Contributors: Austin Hendrix, William Woodall, jonbinney

1.7.3 (2013-06-27)
------------------
* use new pluginlib API
* Contributors: Jon Binney

1.7.2 (2013-06-26)
------------------
* a bunch of CMake fixes to get tests passing.
* fixing linking
* 1.7.2
  For bugfix release
* Merge pull request `#3 <https://github.com/ros/filters/issues/3>`_ from dgossow/patch-1
  Export pluginlib as dependency
* Export pluginlib as dependency
* Contributors: David Gossow, Tully Foote

1.7.1 (2013-05-24)
------------------
* bump version for bugfix
* Merge pull request `#2 <https://github.com/ros/filters/issues/2>`_ from jonbinney/install_headers
  added install rule for headers in cmakelists
* added install rule for headers in cmakelists
* Contributors: Jon Binney, jonbinney

1.7.0 (2013-05-23)
------------------
* bump version for hydro release
* reenabled rostests
* builds with catkin
* branched to separate cpp and ROS parts
  --HG--
  branch : cpp_separation
* Added tag unstable for changeset 661a74b486de
  --HG--
  branch : filters
* Added tag filters-1.6.0 for changeset 925818adeafe
  --HG--
  branch : filters
* 1.6.0
  --HG--
  branch : filters
* creating unary stack to refactor from common
  --HG--
  branch : filters
* url fix
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4037239
* patch for `#4144 <https://github.com/ros/filters/issues/4144>`_ including backwards compatability.  Also added check to give nice error and quit if invalid filter name, instead of throwing exception
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4030358
* rest of fix for `#4181 <https://github.com/ros/filters/issues/4181>`_ tests
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4030356
* basic tests for `#4181 <https://github.com/ros/filters/issues/4181>`_ more to come
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4030355
* Added Ubuntu platform tags to manifest
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4029647
* removing deprecated TransferFunctionFilter it is replaced by SingleChannelTransferFunctionFilter `#3703 <https://github.com/ros/filters/issues/3703>`_
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4028320
* fixing segfault in realtime circular buffer if zero length `#3785 <https://github.com/ros/filters/issues/3785>`_ `#3762 <https://github.com/ros/filters/issues/3762>`_
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4027710
* adding namespace to all debugging/errors for filter chain loader `#3239 <https://github.com/ros/filters/issues/3239>`_
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4026552
* updating the tests
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4026106
* adding single channel transferfunctionfilter
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4026105
* fixing build for karmic
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4025262
* doc reviewed
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4024954
* all API issues cleared for filters
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4024863
* Fixing warning message in filter_chain.h with regard to `#2959 <https://github.com/ros/filters/issues/2959>`_
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4024757
* removing hard codeing of filter_chain parameter list name.  `#2618 <https://github.com/ros/filters/issues/2618>`_  Backwards compatable statement left in with ROS_WARN to change
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4024289
* removing unused dependency on tinyxml
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4024141
* Copying commit from latest to trunk. 'Added temporary OSX blacklist files'
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4023977
* migration part 1
  --HG--
  branch : filters
  extra : convert_revision : svn%3Aeb33c2ac-9c88-4c90-87e0-44a10359b0c3/stacks/common/trunk/filters%4023884
* Contributors: Jon Binney, Ken Conley, gerkey, jonbinney, kwc, leibs, mwise, sachinc, tfoote, vpradeep
