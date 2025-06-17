This document is a declaration of software quality for the `class_loader` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# class_loader Quality Declaration

The package `class_loader` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`class_loader` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning), and is at or above a stable version.

### Version Stability [1.ii]

`class_loader` is at a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](package.xml), and its change history can be found in its [CHANGELOG](CHANGELOG.rst).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`class_loader` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`class_loader` will maintain ABI stability within a ROS distribution.

## Change Control Process [2]

`class_loader` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process).

### Change Requests [2.i]

This package requires that all changes occur through a pull request. Check the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#pull-requests) for additional information.

### Contributor Origin [2.ii]

This package has a confirmation of contributor origin policy, which can be found in [CONTRIBUTING](./CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull requests must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/job/nightly_linux-aarch64_release/lastBuild/testReport/class_loader/)
* [linux_release](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/class_loader/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/class_loader/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/class_loader/)

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging

## Documentation [3]

### Feature Documentation [3.i]

`class_loader` has a ROS 2 [feature list](./README.md#usage). The feature list for ROS 1 can be found [here](http://wiki.ros.org/class_loader), where each item in the list links to the corresponding feature documentation.

### Public API Documentation [3.ii]

`class_loader` has documentation of its public API and it is [hosted](http://docs.ros2.org/latest/api/class_loader/index.html). There is documentation for all of the public API using docblocks, and new additions to the public API require documentation before being added.

### License [3.iii]

The license for `class_loader` is BSD-3-Clause, and a summary is in each source file, the type is declared in the [package.xml](./package.xml) manifest file, and a full copy of the license is in the [LICENSE](./LICENSE) file.

There is an automated test which runs a linter (ament_copyright) that ensures each file has a license statement.

### Copyright Statement [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `class_loader`.

There is an automated test which runs a linter (ament_copyright) that ensures each file has at least one copyright statement.

Most recent test results can be found [here](http://build.ros2.org/view/Rpr/job/Rpr__class_loader__ubuntu_focal_amd64/lastCompletedBuild/testReport/class_loader/copyright_check_tests_only/)

## Testing [4]

### Feature Testing [4.i]

Each feature in `class_loader` has corresponding tests which simulate typical usage, and they are located in the `test` directory.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/job/nightly_linux-aarch64_release/lastBuild/testReport/class_loader/)
* [linux_release](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/class_loader/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/class_loader/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/class_loader/)

### Public API Testing [4.ii]

Each part of the public API have tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`class_loader` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use branch coverage instead of line coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

This package has testing coverage of at least 95%.
Current coverage statistics can be viewed [here](https://ci.ros2.org/job/nightly_linux_coverage/lastSuccessfulBuild/cobertura/).
A description of how coverage statistics are calculated is summarized in the [ROS 2 On-boarding Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#note-on-coverage-runs).

### Performance [4.iv]

`class_loader` follows the recommendations for performance testing of C/C++ code in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#performance), and opts to do performance analysis on each release rather than each change.

System level performance benchmarks that cover features of `class_loader` can be found at:
* [Benchmarks](http://build.ros2.org/view/Rci/job/Rci__benchmark_ubuntu_focal_amd64/BenchmarkTable/)
* [Performance](http://build.ros2.org/view/Rci/job/Rci__nightly-performance_ubuntu_focal_amd64/lastCompletedBuild/)

Changes that introduce regressions in performance must be adequately justified in order to be accepted and merged.

### Linters and Static Analysis [4.v]

`class_loader` uses and passes all the standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

## Dependencies [5]

Below are evaluations of each of `class_loader`'s run-time and build-time dependencies that have been determined to influence the quality.

It has one "buildtool" dependency, which does not affect the resulting quality of the package because it does not contribute to the public library API.

It also has several test dependencies, which do not affect the resulting quality of the package because these are only used to build and run test code.

### Direct Runtime ROS Dependencies [5.i]/[5.ii]

#### `console_bridge_vendor`

The `console_bridge_vendor` package provides a wrapper around `console_bridge`, providing nothing but a dependency on `console_bridge` on some systems. On others, it provides an [ExternalProject](https://cmake.org/cmake/help/latest/module/ExternalProject.html) build of `console_bridge`.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/console_bridge_vendor/blob/master/QUALITY_DECLARATION.md).

#### `rcpputils`

The `rcpputils` package provides an API which contains common utilities and data structures needed when programming in C++.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcpputils/blob/master/QUALITY_DECLARATION.md).

### Direct Runtime non-ROS Dependency [5.iii]

#### `libconsole-bridge-dev`

The [libconsole-bridge-dev](https://github.com/ros/console_bridge/) is a ROS-independent, pure CMake (i.e. non-catkin and non-rosbuild package) that provides logging calls that mirror those found in rosconsole, but for applications that are not necessarily using ROS.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros/console_bridge/blob/master/QUALITY_DECLARATION.md).

## Platform Support [6]

`class_loader` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/job/nightly_linux-aarch64_release/lastBuild/testReport/class_loader/)
* [linux_release](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/class_loader/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/class_loader/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/class_loader/)

## Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).

# Current status Summary

The chart below compares the requirements in the REP-2004 with the current state of the class_loader package.

|Number|  Requirement| Current state |
|--|--|--|
|1| **Version policy** |---|
|1.i|Version Policy available | ✓ |
|1.ii|Stable version |✓|
|1.iii|Declared public API|✓|
|1.iv|API stability policy|✓|
|1.v|ABI stability policy|✓|
|1.vi_|API/ABI stable within ros distribution|✓|
|2| **Change control process** |---|
|2.i| All changes occur on change request | ✓|
|2.ii| Contributor origin (DCO, CLA, etc) | ✓|
|2.iii| Peer review policy | ✓ |
|2.iv| CI policy for change requests | ✓ |
|2.v| Documentation policy for change requests | ✓ |
|3| **Documentation** | --- |
|3.i| Per feature documentation | ✓ |
|3.ii| Per public API item documentation | ✓ |
|3.iii| Declared License(s) | ✓ |
|3.iv| Copyright in source files| ✓ |
|3.v.a| Quality declaration linked to README | ✓ |
|3.v.b| Centralized declaration available for peer review |✓|
|4| Testing | --- |
|4.i| Feature items tests | ✓ |
|4.ii| Public API tests | ✓ |
|4.iii.a| Using coverage |✓ |
|4.iii.a| Coverage policy | ✓ |
|4.iv.a| Performance tests (if applicable) | ? |
|4.iv.b| Performance tests policy| ✓ |
|4.v.a| Code style enforcement (linters)| ✓ |
|4.v.b| Use of static analysis tools | ✓ |
|5| Dependencies | --- |
|5.i| Must not have ROS lower level dependencies | ✓ |
|5.ii| Optional ROS lower level dependencies| ✓ |
|5.iii| Justifies quality use of non-ROS dependencies |✓|
|6| Platform support | --- |
|6.i| Support targets Tier1 ROS platforms| ✓ |
|7| Security | --- |
|7.i| Vulnerability Disclosure Policy | ✓ |
