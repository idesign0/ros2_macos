This document is a declaration of software quality for the `spdlog` external dependency imported by the `spdlog_vendor` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `spdlog` Quality Declaration

This quality declaration claims that `spdlog` is in the **Quality Level 1** category.

The upstream code of `spdlog` meets many, but not all of the criteria stated for Quality Level 1.
As `spdlog` is not a ROS package, these gaps are reconciled in the vendor package within ROS 2.
See [QUALITY_DECLARATION.md](./QUALITY_DECLARATION.md) for more details.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

There is a three-part version attached to `spdlog` releases.
The sources define the version parts as `MAJOR`.`MINOR`.`PATCH`.
There is an [old ticket](https://github.com/gabime/spdlog/issues/86) where the project's primary developer discusses versioning, but there is no documentation officially stating what approach the developer is taking.

### Version Stability [1.ii]

The latest release of `spdlog` has a stable version, i.e. `>= 1.0.0`.

### Public API Declaration [1.iii]

Because `spdlog` can be consumed as a header-only library, nearly all of the source code can be considered part of the public API.
For the compiled portion of `spdlog`, the `SPDLOG_API` C++ macro indicates the symbols that are part of the public API.

### API Stability Policy [1.iv]

There is no policy for API stability. However, the vendored package `spdlog_vendor` package importing the `spdlog` dependency is using a fixed version, in this case [1.6.1](https://github.com/gabime/spdlog/releases/tag/v1.6.1), which will provide API stability to downstream packages.

### ABI Stability Policy [1.v]

There is no policy for ABI stability. This is not a problem because the `spdlog_vendor` package importing the `spdlog` dependency is using a fixed version, in this case [1.6.1](https://github.com/gabime/spdlog/releases/tag/v1.6.1).

### ABI and ABI Stability Within a Released ROS Distribution [1.vi]

`spdlog`'s API and ABI stability within a ROS Distribution are maintained by `spdlog_vendor` which pins the `spdlog` dependency to a specific version, in this case [1.6.1](https://github.com/gabime/spdlog/releases/tag/v1.6.1).

## Change Control Process [2]

### Change Requests [2.i]

The source code repository for `spdlog` is hosted in GitHub, and the change process follows a typical GitHub pull request model.
The developer appears to be active in triaging incoming issues and addressing incoming pull requests in a timely manner.

### Contributor Origin [2.ii]

Changes to `spdlog` do not contain or require any confirmation of contributor origin.

### Peer Review Policy [2.iii]

It appears that a single developer is reviewing and merging the proposed changes.
That primary developer does commit directly to the repository, so not all changes appear to be peer-reviewed.

### Continuous Integration [2.iv]

Incoming pull requests automatically trigger continuous integration testing and must run successfully to be merged.
Automated testing runs for:
- Ubuntu Linux with gcc 4.8 and gcc 7, Clang 3.5, and Clang 10
- macOS High Sierra with Xcode 9.4.1
- Windows Server 2012 R2 with Visual Studio 2015
- Windows Server 2016 with Visual Studio 2017

Any commits to the development branch also trigger continuous integration testing.

## Documentation [3]

### Feature Documentation [3.i]

A feature list is included in the `spdlog` documentation [in the repository](https://github.com/gabime/spdlog#features).

### Public API Documentation [3.ii]

The API for `spdlog` is documented with a targeted example for each advertised feature.
This documentation is available [in the repository](https://github.com/gabime/spdlog#usage-samples), and there is more detailed information on some specific topics available in the GitHub Wiki for `spdlog`.

### License [3.iii]

The license for `spdlog` is MIT, and all of the code includes a header stating so.
The `spdlog` repository also includes a [`LICENSE`](https://github.com/gabime/spdlog/blob/v1.x/LICENSE) file with the full terms.

There is some `spdlog` test code which is licensed under Boost 1.0, and the full license text is [included](https://github.com/gabime/spdlog/blob/v1.x/tests/catch.license) alongside the affected source code files.

### Copyright Statement [3.iv]

Each of the `spdlog` source files containing code include a copyright statement with the license information in the file's header.

## Testing [4]

### Feature Testing [4.i]

Each feature in spdlog has corresponding tests which simulate typical usage, and they are located [here](https://github.com/gabime/spdlog/blob/v1.6.1/tests/). The result of these tests can be found at:
 - [Linux](https://travis-ci.org/github/gabime/spdlog/builds/691497445)
 - [macOS](https://travis-ci.org/github/gabime/spdlog/builds/691497445)
 - [Windows](https://ci.appveyor.com/project/gabime/spdlog/builds/33133070)

`spdlog` is pinned to a specific version by its vendor package, so we're fixed to a fixed set of features.

### Public API Testing [4.ii]

`spdlog` has API tests which simulate typical usage, and they are located in link to test source code. The result of these tests can be found at:
 - [Linux](https://travis-ci.org/github/gabime/spdlog/builds/691497445)
 - [macOS](https://travis-ci.org/github/gabime/spdlog/builds/691497445)
 - [Windows](https://ci.appveyor.com/project/gabime/spdlog/builds/33133070)

`spdlog` is pinned to a specific version by its vendor package, so we're fixed to a fixed API.

### Coverage [4.iii]

ROS 2 is only using part of `spdlog`'s API. Latest code coverage can be found [here](https://ci.ros2.org/job/ci_linux_coverage/lastSuccessfulBuild/cobertura/). The result shows the API that is being used in the ROS 2 codebase.

In particular ROS 2 is using the [logger class](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/logger.h#L46), the [pattern formatter](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/pattern_formatter-inl.h) and the [basic sink](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/sinks/base_sink-inl.h).

Tests are located [here](https://github.com/gabime/spdlog/blob/v1.6.1/tests/). In particular there are some of them which cover the used API, for example:

  - [Pattern formatter test](https://github.com/gabime/spdlog/blob/v1.6.1/tests/test_pattern_formatter.cpp)
  - [Sink tests](https://github.com/gabime/spdlog/blob/v1.6.1/tests/test_sink.h)

In the `rcl_logging_spdlog` package there are some tests that are covering the used API. These tests are located [here](https://github.com/ros2/rcl_logging/tree/master/rcl_logging_spdlog/test).

### Performance [4.iv]

Performance tests for `spdlog` can be built by specifying the `SPDLOG_BUILD_BENCH` CMake option.
The benchmarks are not run as part of continuous integration, but some results are included in the `spdlog` documentation [in the repository](https://github.com/gabime/spdlog#benchmarks).

### Linters and Static Analysis [4.v]

`clang-tidy` and `clang-format` are configured to run for `spdlog`, but are not run as part of continuous integration.
There are two sanitizer options that can be enabled with CMake options:
* `SPDLOG_SANITIZE_ADDRESS`
* `SPDLOG_SANITIZE_THREAD`

The ASAN option is enabled as part of continuous integration.

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i]

As an external dependency, there are no ROS dependencies in `spdlog`.

### Optional Direct Runtime ROS Dependencies [5.ii]

As an external dependency, there are no ROS dependencies in `spdlog`.

### Direct Runtime non-ROS Dependency [5.iii]

There are no external dependencies in `spdlog`.

## Platform Support [6]

The `spdlog` documentation states that it is supported on the following platforms, and more:
* Linux (gcc 4.8-7.0) (Partially [Tested in CI](https://travis-ci.org/github/gabime/spdlog))
* Windows (msvc 2013+) (Partially [Tested in CI](https://ci.appveyor.com/project/gabime/spdlog))
* macOS (clang 3.5-7.0) (Partially [Tested in CI](https://travis-ci.org/github/gabime/spdlog))

## Security [7]

### Vulnerability Disclosure Policy [7.i]

The Vulnerability Disclosure Policy is defined in the vendored Quality Declaration.
