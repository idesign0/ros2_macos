This document is a declaration of software quality for the `fastrtps_cmake_module` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `fastrtps_cmake_module` Quality Declaration

The package `fastrtps_cmake_module` claims to be in the **Quality Level 4** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 4 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`fastrtps_cmake_module` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`fastrtps_cmake_module` is at a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](package.xml), and its change history can be found in its [CHANGELOG](CHANGELOG.rst).

### Public API Declaration [1.iii]

`fastrtps_cmake_module` does not provide any public API of its own.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`fastrtps_cmake_module` does not provide its own API and therefore API stability will not be impacted by changes.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`fastrtps_cmake_module` does not contain any C or C++ code, therefore changes will not affect ABI stability.

## Change Control Process [2]

`fastrtps_cmake_module` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#quality-practices).

### Change Requests [2.i]

All changes will occur through a pull request, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull request must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers).

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`fastrtps_cmake_module` does not currently have its own features.

### Public API Documentation [3.ii]

`fastrtps_cmake_module` does not have a public API and therefore does not require API documentation.

### License [3.iii]

The license for `fastrtps_cmake_module` is Apache 2.0, the type is declared in the [package.xml](package.xml) manifest file, and a full copy of the license is in the [LICENSE](../LICENSE) file.

There are no source files in this package and therefore linters do not check for a license statement.

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `fastrtps_cmake_module`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

The results of the test can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/fastrtps_cmake_module/copyright/).

## Testing [4]

`fastrtps_cmake_module` is a package providing solely CMake files and therefore does not require tests and has no coverage or performance requirements.

### Linters and Static Analysis [4.v]

`fastrtps_cmake_module` uses and passes all the standard linters and static analysis tools for a CMake package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Results of linter tests can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/fastrtps_cmake_module/).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i]/[5.ii]

`fastrtps_cmake_module` does not have any runtime ROS dependencies.

### Direct Runtime Non-ROS Dependencies [5.iii]
`fastrtps_cmake_module` does not have any runtime non-ROS dependencies.

## Platform Support [6]

`fastrtps_cmake_module` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/fastrtps_cmake_module/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/fastrtps_cmake_module/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/fastrtps_cmake_module/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/fastrtps_cmake_module/)

## Vulnerability Disclosure Policy [7.i]

This package does not yet have a Vulnerability Disclosure Policy
