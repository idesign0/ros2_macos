# Joint State Publisher

This contains a package for publishing `sensor_msgs/msg/JointState` messages for a robot described with URDF.
Given a URDF (either passed on the command-line or via the `/robot_description` topic), this node
will continually publish values for all of the movable joints in the URDF to the `/joint_states` topic.
In combination with `robot_state_publisher`, this ensures that there is a valid transform for all joints
even when the joint doesn't have encoder data.

Published Topics
----------------
* `/joint_states` (`sensor_msgs/msg/JointState`) - The state of all of the movable joints in the system.

Subscribed Topics
-----------------
* (optional) `/robot_description` (`std_msgs/msg/String`) - If no URDF is given on the command-line, then this node will listen on the `/robot_description` topic for the URDF to be published.  Once it has been received at least once, this node will start to publish joint values to `/joint_states`.
* (optional) `/any_topic` (`sensor_msgs/msg/JointState`) - If the `sources_list` parameter is not empty (see Parameters below), then every named topic in this parameter will be subscribed to for joint state updates.  Do *not* add the default `/joint_states` topic to this list, as it will end up in an endless loop!

Parameters
----------
* `rate` (int) - The rate at which to publish updates to the `/joint_states` topic.  Defaults to 10.
* `publish_default_positions` (bool) - Whether to publish a default position for each movable joint to the `/joint_states` topic.  Defaults to True.
* `publish_default_velocities` (bool) - Whether to publish a default velocity for each movable joint to the `/joint_states` topic.  Defaults to False.
* `publish_default_efforts` (bool) - Whether to publish a default effort for each movable joint to the `/joint_states` topic.  Defaults to False.
* `use_mimic_tags` (bool) - Whether to honor `<mimic>` tags in the URDF.  Defaults to True.
* `use_smallest_joint_limits` (bool) - Whether to honor `<safety_controller>` tags in the URDF.  Defaults to True.
* `source_list` (array of strings) - Each string in this array represents a topic name.  For each string, create a subscription to the named topic of type `sensor_msgs/msg/JointStates`.  Publication to that topic will update the joints named in the message.  Defaults to an empty array.
* `delta` (double) - How much to automatically move joints during each iteration.  Defaults to 0.0.

#### Mapped Parameters

These parameters map from joint_names to values. The format to use these parameters is `<parameter>.<key>:=<value>`, where a new parameter is defined for each key. See below for examples.

* `zeros` (map from string -> float) - A map of joint_names to initial starting values for the joint. For example, in Eloquent and beyond, this parameter can be used from the command-line with `ros2 run joint_state_publisher joint_state_publisher --ros-args --param zeros.joint_name1:=value1 --param zeros.joint_name2:=value2`. This parameter is not set by default, so all joints start at zero. For joints where zero isn't within the joint range, it uses the range's (max + min) / 2.
* `dependent_joints` (map from string -> map from 'parent', 'factor', 'offset' -> float) - A map of joint_names to the joints that they mimic; compare to the `<mimic>` tag in URDF.  A joint listed here will mimic the movements of the 'parent' joint, subject to the 'factor' and 'offset' provided.  The 'parent' name must be provided, while the 'factor' and 'offset' parameters are optional (they default to 1.0 and 0.0, respectively).  For example, in Eloquent and beyond, this parameter can be used from the command-line with `ros2 run joint_state_publisher joint_state_publisher --ros-args --param dependent_joints.left_leg.parent:=right_leg --param dependent_joints.left_leg.offset:=0.0 --param dependent_joints.left_leg.factor:=2.0`. This parameter is not set by default, in which case only joints that are marked as `<mimic>` in the URDF are mimiced.
