# random_numbers

*This branch has been ported to ROS 2.0*

This library contains wrappers for generating floating point values, integers, quaternions using boost libraries.

## Build Status

| Branch | CI Status |
| ------ | --------- |
| `ros2` | [![Formatting (pre-commit)](https://github.com/ros-planning/random_numbers/actions/workflows/format.yaml/badge.svg?branch=ros2)](https://github.com/ros-planning/random_numbers/actions/workflows/format.yaml?query=branch%3Aros2) [![Build and Test](https://github.com/ros-planning/random_numbers/actions/workflows/build_and_test.yaml/badge.svg?branch=ros2)](https://github.com/ros-planning/random_numbers/actions/workflows/build_and_test.yaml?query=branch%3Aros2) |
| `master (ROS1)` | [![Formatting (pre-commit)](https://github.com/ros-planning/random_numbers/actions/workflows/format.yaml/badge.svg?branch=master)](https://github.com/ros-planning/random_numbers/actions/workflows/format.yaml?query=branch%3Akinetic-devel) [![Build and Test](https://github.com/ros-planning/random_numbers/actions/workflows/industrial_ci_action.yaml/badge.svg?branch=master)](https://github.com/ros-planning/random_numbers/actions/workflows/build_and_test.yaml?query=branch%3Amaster) |


## Features

New: you can pass in a custom random number generator seed to allow optional deterministic behavior during debugging, testing, etc. using the secondary constructor.
