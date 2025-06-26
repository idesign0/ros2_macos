^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package py_binding_tools
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.0.2 (2025-05-21)
------------------
* Replace deprecated ament_target_dependencies()
* Add type caster for rclpy.Time <-> rclcpp::Time (`#2 <https://github.com/ros-planning/py_binding_tools/issues/2>`_)
* Contributors: Robert Haschke

2.0.1 (2024-07-12)
------------------
* rclcpp.init(None) will resort to sys.argv as with rclpy.init(None)
* Contributors: Robert Haschke

2.0.0 (2024-05-25)
------------------
* ROS2 migration
* Contributors: Robert Haschke

1.0.0 (2024-03-05)
------------------
* Initial release factored out from https://github.com/ros-planning/moveit/pull/2910
