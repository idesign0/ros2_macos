This document is a declaration of software quality for the `console_bridge_vendor` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `console_bridge_vendor` Quality Declaration

The package `console_bridge_vendor` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Quality Categories in REP-2004](https://www.ros.org/reps/rep-2004.html) of the ROS2 developer guide.

## Version Policy [1]

### Version Scheme [1.i]

`console_bridge_vendor` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`console_bridge_vendor` is at a stable version, i.e. >= 1.0.0. The current version can be found in its [package.xml](./package.xml).

### Public API Declaration [1.iii]

This is a vendor package for `libconsole-bridge-dev` and as such does not declare its own API.

### API Stability Policy [1.iv]/[1.vi]

`console_bridge_vendor` does not have an API of its own, and will limit its dependency on `libconsole-bridge-dev` to API stable releases within a released ROS distribution.

### ABI Stability Policy [1.v]/[1.vi]

`console_bridge_vendor` does not have an ABI of its own, and will limit its dependency on `libconsole-bridge-dev` to ABI stable releases within a released ROS distribution.

## Change Control Process [2]

`console_bridge_vendor` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process).

### Change Requests [2.i]

All changes will occur through a pull request, check the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](./CONTRIBUTING.md).

### Peer Review Policy [2.iii]

All pull requests will be peer-reviewed, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Continuous Integration [2.iv]

All pull requests must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers).

Current nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/console_bridge_vendor/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/console_bridge_vendor/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/console_bridge_vendor/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/console_bridge_vendor/)

###  Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`console_bridge_vendor` does not have features other than importing the external dependency `libconsole-bridge-dev` and therefore does not require feature documentation.

### Public API Documentation [3.ii]

`console_bridge_vendor` does not have an API and therefore does not require API documentation.

### License [3.iii]

The license for `console_bridge_vendor` is Apache 2.0, and a summary is in each source file, the type is declared in the [`package.xml`](./package.xml) manifest file, and a full copy of the license is in the `LICENSE` file. The vendored library, `libconsole-bridge-dev` license is BSD as stated in its quality declaration document (Section 5.iii).

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `console_bridge_vendor`.

## Testing [4]

### Feature Testing [4.i]

`console_bridge_vendor` does not have feature testing. For vendored packages, this will be considered to be addressed in the external dependency.

### Public API Testing [4.ii]

`console_bridge_vendor` does not have Public API testing. For vendored packages, this will be considered to be addressed in the external dependency.

### Coverage [4.iii]

`console_bridge_vendor` does not provide coverage testing. For vendored packages, this will be considered to be addressed in the external dependency.

### Performance [4.iv]

The Console Bridge package is benchmarked and the most recent test results can be found [here](http://build.ros2.org/view/Rci/job/Rci__benchmark_ubuntu_focal_amd64/BenchmarkTable/).

`console_bridge_vendor` does not provide performance testing. For vendored packages, this will be considered to be addressed in the external dependency.

### Linters and Static Analysis [4.v]

`console_bridge_vendor` is tested with `xmllint` and `lint_cmake` on [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers).

Current nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/console_bridge_vendor/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/console_bridge_vendor/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/console_bridge_vendor/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/console_bridge_vendor/)

## Dependencies [5]

### Direct and Optional Runtime ROS Dependencies [5.i]/[5.ii]

`console_bridge_vendor` does not have direct/optional runtime ROS dependencies.

### Direct Runtime non-ROS Dependency [5.iii]

`console_bridge_vendor` depends directly on the external dependency `libconsole-bridge-dev`, which is qualified as quality level 2 in its [Quality Declaration](https://github.com/ros/console_bridge/blob/master/QUALITY_DECLARATION.md).

## Platform Support [6]

`console_bridge_vendor` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers).

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).
