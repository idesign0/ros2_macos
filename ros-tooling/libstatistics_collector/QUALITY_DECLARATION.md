This document is a declaration of software quality for the `libstatistics_collector` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `libstatistics_collector` Quality Declaration

The package `libstatistics_collector` claims to be in the **Quality Level 1** category when it is used with a **Quality Level 1** middleware.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Quality Categories in REP-2004](https://www.ros.org/reps/rep-2004.html) of the ROS2 developer guide.

## Version Policy [1]

### Version Scheme [1.i]

`libstatistics_collector` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`libstatistics_collector` is at a stable version (`>= 1.0.0`.)
The current version can be found in its [package.xml](package.xml), and its change history can be found in its [CHANGELOG](CHANGELOG.rst).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Policy [1.iv]

`libstatistics_collector` follows semver versioning semantics and will not make changes to its API without increasing its major version number.

### ABI Stability Policy [1.v]

`libstatistics_collector` contains C and C++ code and is therefore concerned with ABI stability and will not change its ABI without increasing its major version number.

### ABI and ABI Stability Within a Released ROS Distribution [1.vi]

`libstatistics_collector` will not break API nor ABI within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

## Change Control Process [2]

`libstatistics_collector` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process).

### Change Requests [2.i]

All changes will occur through a pull request, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](./CONTRIBUTING.md).

### Peer Review Policy [2.iii]

All pull requests will be peer-reviewed, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Continuous Integration [2.iv]

All pull requests must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

Currently nightly results can be seen here:

* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastCompletedBuild/testReport/libstatistics_collector/)

###  Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`libstatistics_collector` has a [feature list](https://github.com/ros-tooling/libstatistics_collector/blob/master/README.md) and each item in the list links to the corresponding feature documentation. There is documentation for all of the features, and new features require documentation before being added.

### Public API Documentation [3.ii]

The API documentation is not publicly available yet.

### License [3.iii]

The license for `libstatistics_collector` is Apache 2.0, and a summary is in each source file, the type is declared in the [`package.xml`](./package.xml) manifest file, and a full copy of the license is in the [`LICENSE`](LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement. [Here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/libstatistics_collector/) can be found a list with the latest results of the various linters being run on the package.

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `libstatistics_collector`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement. Latest linter result report can be seen [here](http://build.ros2.org/view/Epr/job/Epr__libstatistics_collector__ubuntu_bionic_amd64/lastCompletedBuild/testReport/libstatistics_collector/copyright/).

## Testing [4]

### Feature Testing [4.i]

Each feature in `libstatistics_collector` has corresponding tests which simulate typical usage, and they are located in the [`test`](https://github.com/ros2/libstatistics_collector/tree/master/test) directory.
New features are required to have tests before being added.

Currently nightly test results can be seen here:

* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastCompletedBuild/testReport/libstatistics_collector/)

### Public API Testing [4.ii]

Each part of the public API has tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`libstatistics_collector` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use line coverage instead of branch coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Each time that a new PR is created [codecov](https://codecov.io/) will create diagram to show the current status of the test coverage and how this PR will affect the coverage of the package. Latest codecov result report can be seen [here](https://codecov.io/gh/ros-tooling/libstatistics_collector).

### Performance [4.iv]

`libstatistics_collector` follows the recommendations for performance testing of C/C++ code in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#performance), and opts to do performance analysis on each release rather than each change.

The performance tests of this package are located in the [test/benchmark directory](https://github.com/ros-tooling/libstatistics_collector/tree/master/test/benchmark).

Package and system level performance benchmarks that cover features of `libstatistics_collector` can be found at:
* [Benchmarks](http://build.ros2.org/view/Rci/job/Rci__benchmark_ubuntu_focal_amd64/BenchmarkTable/)
* [Performance](http://build.ros2.org/view/Rci/job/Rci__nightly-performance_ubuntu_focal_amd64/lastCompletedBuild/)

Changes that introduce regressions in performance must be adequately justified in order to be accepted and merged.

### Linters and Static Analysis [4.v]

`libstatistics_collector` uses and passes all the ROS2 standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis). Passing implies there are no linter/static errors when testing against CI of supported platforms.

Currently nightly test results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastCompletedBuild/testReport/libstatistics_collector/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastCompletedBuild/testReport/libstatistics_collector/)

## Dependencies [5]

### Direct and Optional Runtime ROS Dependencies [5.i]/[5.ii]

`libstatistics_collector` has the following runtime ROS dependencies:

#### `rcl`

The `rcl` package supports implementation of language specific ROS Client Libraries.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcl/blob/master/rcl/QUALITY_DECLARATION.md).

#### `rcpputils`

The `rcpputils` package is a C++ API consisting of macros, functions, and data structures intended for use throughout the ROS 2 codebase.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcpputils/blob/master/QUALITY_DECLARATION.md).

#### `rosidl_default_runtime`

The `rosidl_default_runtime` provides CMake functionality for finding and adding runtime dependencies for rosidl packages.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rosidl_defaults/blob/master/rosidl_default_runtime/QUALITY_DECLARATION.md).

#### `statistics_msgs`

The `statistics_msgs` package contains ROS 2 message definitions for reporting statistics for topics and system resources.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcl_interfaces/blob/master/statistics_msgs/QUALITY_DECLARATION.md).

#### `std_msgs`

The `std_msgs` package provides many basic message types.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/common_interfaces/blob/master/std_msgs/QUALITY_DECLARATION.md).

It has several "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime non-ROS Dependency [5.iii]

`libstatistics_collector` has no non-ROS runtime or build-time dependencies that need to be considered for this declaration.

## Platform Support [6]

`libstatistics_collector` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly build status can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastCompletedBuild/libstatistics_collector/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastCompletedBuild/libstatistics_collector/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastCompletedBuild/libstatistics_collector/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastCompletedBuild/libstatistics_collector/)

## Security

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).
