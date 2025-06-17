# rmw_dds_common Features

This package includes:

- A generic [`GraphCache`](rmw_dds_common/include/rmw_dds_common/graph_cache.hpp) to track DDS entities such as participants and data readers and writers, plus ROS nodes as an additional abstraction for any DDS based `rmw` implementation to use
- Common messages to communicate [ROS nodes discovery information](https://github.com/ros2/design/pull/250):
  - [`rmw_dds_common/msg/Gid`](rmw_dds_common/msg/Gid.msg)
  - [`rmw_dds_common/msg/NodeEntitiesInfo`](rmw_dds_common/msg/NodeEntitiesInfo.msg)
  - [`rmw_dds_common/msg/ParticipantEntitiesInfo`](rmw_dds_common/msg/ParticipantEntitiesInfo.msg)
- Some useful data types and utilities:
  - A generic [`Context`](rmw_dds_common/include/rmw_dds_common/context.hpp) type to withhold most state needed to implement [ROS nodes discovery](https://github.com/ros2/design/pull/250)
  - [Comparison utilities and some C++ operator overloads](rmw_dds_common/include/rmw_dds_common/gid_utils.hpp) for `rmw_gid_t` instances
  - [Conversion utilities](rmw_dds_common/include/rmw_dds_common/gid_utils.hpp) between `rmw_dds_common/msg/Gid` messages and `rmw_gid_t` instances
  - A function for checking the compatibility of two QoS profiles, [`qos_profile_check_compatible`](rmw_dds_common/include/rmw_dds_common/qos.hpp)
