This document is a declaration of software quality for the `foonathan_memory_vendor` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `foonathan_memory_vendor` Quality Declaration

The package `foonathan_memory_vendor` claims to be in the **Quality Level 2** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 2 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`foonathan_memory_vendor` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#versioning).

### Version Stability [1.ii]

`foonathan_memory_vendor` is at or above a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](package.xml).

### Public API Declaration [1.iii]

As a vendor package, which provides CMake files that configure the foonathan_memory package, there is no public API.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`foonathan_memory_vendor` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

CMake files that are not in the cmake directory are not installed, and are considered private.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`foonathan_memory_vendor` does not contain any c or c++ code, therefore changes will not affect ABI stability.

## Change Control Process [2]

`foonathan_memory_vendor` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#package-requirements).

### Change Requests [2.i]

This package requires that all changes occur through a pull request.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#package-requirements), all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

Pull requests are currently not tested on CI before merging.
Instead, changes to `foonathan_memory_vendor` are tested [nightly](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/foonathan_memory_vendor/).

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`foonathan_memory_vendor` is a vendor package and doesn't provide features of its own.
Therefore, this package does not require feature documentation.

### Public API Documentation [3.ii]

`foonathan_memory_vendor` does not have an API of its own and therefore does not require public API documentation.

### License [3.iii]

The license for `foonathan_memory_vendor` is Apache 2.0, and a summary is in each source file, the type is declared in the [package.xml](package.xml) manifest file, and a full copy of the license is in the [LICENSE](LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement.

Most recent test results can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/foonathan_memory_vendor/copyright/).

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `foonathan_memory_vendor`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

Results of the copyright tests can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/foonathan_memory_vendor/copyright/).

## Testing [4]

`foonathan_memory_vendor` is a package providing solely CMake files and therefore does not require API or feature tests and has no coverage or performance requirements (sections [4.i - 4.iv]).

### Linters and Static Analysis [4.v]

`foonathan_memory_vendor` uses and passes all the standard linters and static analysis tools for a CMake package as described in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#linters-and-static-analysis).

Results of linter tests can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/foonathan_memory_vendor/).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i]/[5.ii]

`foonathan_memory_vendor` has only "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime Non-ROS Dependencies [5.iii]

`foonathan_memory_vendor` has the following non-ROS runtime packages:

#### `foonathan_memory`

`foonathan_memory` is a C++ library used to augment std::allocator.

It is **Quality Level 2**, see its [Quality Declaration document](https://github.com/eProsima/Fast-DDS/blob/master/Quality_Declaration_foonathan_memory.md).

## Platform Support [6]

`foonathan_memory_vendor` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/foonathan_memory_vendor/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/foonathan_memory_vendor/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/foonathan_memory_vendor/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/foonathan_memory_vendor/)

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).

# Current status Summary

The chart below compares the requirements in the REP-2004 with the current state of the `foonathan_memory_vendor` package.

|Number|  Requirement| Current state |
|--|--|--|
|1| **Version policy** |---|
|1.i|Version Policy available | ✓ |
|1.ii|Stable version |✓||
|1.iii|Declared public API|None|
|1.iv|API stability policy|✓|
|1.v|ABI stability policy| N/A |
|1.vi|API/ABI stable within ros distribution|✓|
|2| **Change control process** |---|
|2.i| All changes occur on change request | ✓|
|2.ii| Contributor origin (DCO, CLA, etc) | ✓|
|2.iii| Peer review policy | ✓ |
|2.iv| CI policy for change requests | ✓ |
|2.v| Documentation policy for change requests | ✓ |
|3| **Documentation** | --- |
|3.i| Per feature documentation | N/A |
|3.ii| Per public API item documentation | N/A |
|3.iii| Declared License(s) | ✓ |
|3.iv| Copyright in source files| ✓ |
|3.v.a| Quality declaration linked to README | ✓ |
|3.v.b| Centralized declaration available for peer review |✓|
|4| **Testing** | --- |
|4.i| Feature items tests | N/A |
|4.ii| Public API tests | N/A |
|4.iii.a| Using coverage |N/A |
|4.iii.a| Coverage policy | N/A |
|4.iv.a| Performance tests (if applicable) | N/A |
|4.iv.b| Performance tests policy| N/A |
|4.v.a| Code style enforcement (linters)| ✓ |
|4.v.b| Use of static analysis tools | N/A |
|5| **Dependencies** | --- |
|5.i| Must not have ROS lower level dependencies | ✓ |
|5.ii| Optional ROS lower level dependencies| ✓ |
|5.iii| Justifies quality use of non-ROS dependencies |✓|
|6| **Platform support** | --- |
|6.i| Support targets Tier1 ROS platforms| ✓ |
|7| **Security** | --- |
|7.i| Vulnerability Disclosure Policy | ✓ |
