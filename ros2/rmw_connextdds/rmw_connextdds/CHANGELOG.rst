^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rmw_connextdds
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.11.4 (2025-03-25)
-------------------

0.11.3 (2024-11-25)
-------------------

0.11.2 (2023-07-18)
-------------------

0.11.1 (2022-04-26)
-------------------

0.11.0 (2022-04-08)
-------------------
* Exclude missing sample info fields when building rmw_connextddsmicro (`#79 <https://github.com/ros2/rmw_connextdds/issues/79>`_)
* Update launch_testing_ros output filter prefixes for Connext6 (`#80 <https://github.com/ros2/rmw_connextdds/issues/80>`_)
* Contributors: Andrea Sorbini, Ivan Santiago Paunovic

0.10.0 (2022-03-28)
-------------------
* Add support for user-specified content filters (`#68 <https://github.com/ros2/rmw_connextdds/issues/68>`_)
* add stub for content filtered topic (`#77 <https://github.com/ros2/rmw_connextdds/issues/77>`_)
* Contributors: Andrea Sorbini, Chen Lihui, Ivan Santiago Paunovic

0.9.0 (2022-03-01)
------------------
* Add rmw listener apis (`#44 <https://github.com/rticommunity/rmw_connextdds/issues/44>`_)
* Contributors: iRobot ROS

0.8.3 (2022-02-10)
------------------

0.8.2 (2022-01-14)
------------------

0.8.1 (2021-11-19)
------------------
* Add client/service QoS getters. (`#67 <https://github.com/rticommunity/rmw_connextdds/issues/67>`_)
* Contributors: mauropasse

0.8.0 (2021-09-15)
------------------

0.7.0 (2021-06-04)
------------------
* Add rmw_publisher_wait_for_all_acked support. (`#20 <https://github.com/rticommunity/rmw_connextdds/issues/20>`_)
* Contributors: Barry Xu

0.6.1 (2021-04-26)
------------------

0.6.0 (2021-04-11)
------------------
* Use rmw_qos_profile_unknown when adding entity to graph (`#28 <https://github.com/rticommunity/rmw_connextdds/issues/28>`_)
* Resolve issues identified while investigating `#21 <https://github.com/rticommunity/rmw_connextdds/issues/21>`_ (`#22 <https://github.com/rticommunity/rmw_connextdds/issues/22>`_)
* Use Rolling in README's Quick Start
* Improved implementation of client::is_service_available for Connext Pro
* Only add request header to typecode with Basic req/rep profile
* Remove commented/unused code
* Avoid topic name validation in get_info functions
* Reduce shutdown period to 10ms
* Pass HistoryQosPolicy to graph cache
* Reset error string after looking up type support
* Remove DDS-based WaitSet implementation
* Contributors: Andrea Sorbini, Ivan Santiago Paunovic

0.5.0 (2021-04-06)
------------------
* Merge pull request `#13 <https://github.com/rticommunity/rmw_connextdds/issues/13>`_ from Ericsson/unique_network_flows
* Refactor common API
* Update branch `master` to support Rolling only (`#15 <https://github.com/rticommunity/rmw_connextdds/issues/15>`_)
* Contributors: Ananya Muddukrishna, Andrea Sorbini, William Woodall

0.4.0 (2021-03-25)
------------------
* Add ability to override of endpoint qos settings based on topic name.
* Optimize QoS for reliable large data.
* Only trigger data condition if samples were loaned from reader.
* Alternative WaitSet implementation based on C++ std, selectable at
  compile-time.

0.3.1 (2021-03-15)
------------------

0.3.0 (2021-03-12)
------------------
* Add `<buildtool_export_depend>` for `ament_cmake`.
* Use default `dds.transport.UDPv4.builtin.ignore_loopback_interface`.

0.2.1 (2021-03-11)
------------------
* Renamed environment variables (``RMW_CONNEXT_USE_DEFAULT_PUBLISH_MODE``,
  ``RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE``).
* Support a list of initial peers via ``RMW_CONNEXT_INITIAL_PEERS``.

0.2.0 (2021-03-10)
------------------

0.1.1 (2021-03-10)
------------------

0.1.0 (2021-03-10)
------------------
* Initial release.
