# libstatistics_collector

![License](https://img.shields.io/github/license/ros-tooling/libstatistics_collector)
[![Test libstatistics_collector](https://github.com/ros-tooling/libstatistics_collector/workflows/Test%20libstatistics_collector/badge.svg)](https://github.com/ros-tooling/libstatistics_collector/actions?query=workflow%3A%22Test+libstatistics_collector%22)
## Description

This C++ library provides the following:

- A `Collector` interface for implementing classes that collect observed data
 and generate statistics for them
- A `TopicStatisticsCollector` interface for implementing classes that
 collect and perform measurements for ROS2 topic statistics.
 Classes for calculating ROS 2 message age and message period statistics are
 also implemented.
- A `MovingAverageStatistics` class for calculating moving average statistics

## Quality Declaration

This package claims to be in the Quality Level 1 category, see the [Quality Declaration](./QUALITY_DECLARATION.md) for more details.
