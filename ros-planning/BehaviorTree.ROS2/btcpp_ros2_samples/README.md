# Sample Behaviors

For documentation on sample behaviors included in this package please see the BehaviorTree.CPP [ROS 2 Integration documentation](https://www.behaviortree.dev/docs/ros2_integration). Documentation of the derived class methods can methods for each ROS interface type can be found [here](../behaviortree_ros2/ros_behavior_wrappers.md).

# TreeExecutionServer Sample

Documentation on the TreeExecutionServer used in this example can be found [here](../behaviortree_ros2/tree_execution_server.md).

To start the sample Execution Server that load a list of plugins and BehaviorTrees from `yaml` file:
``` bash
ros2 launch btcpp_ros2_samples sample_bt_executor.launch.xml
```

> *NOTE:* For documentation on the `yaml` parameters please see [bt_executor_parameters.md](../behaviortree_ros2/bt_executor_parameters.md).

As the Server starts up it will print out the name of the ROS Action followed by the plugins and BehaviorTrees it was able to load.
```
[bt_action_server]: Starting Action Server: behavior_server
[bt_action_server]: Loaded Plugin: libdummy_nodes_dyn.so
[bt_action_server]: Loaded Plugin: libmovebase_node_dyn.so
[bt_action_server]: Loaded Plugin: libcrossdoor_nodes_dyn.so
[bt_action_server]: Loaded Plugin: libsleep_plugin.so
[bt_action_server]: Loaded BehaviorTree: door_closed.xml
[bt_action_server]: Loaded BehaviorTree: cross_door.xml
```

To call the Action Server from the command line:
``` bash
ros2 action send_goal /behavior_server btcpp_ros2_interfaces/action/ExecuteTree "{target_tree: CrossDoor}"
```

You can also try a Behavior that is a ROS Action or Service client itself.
```bash
ros2 action send_goal /behavior_server btcpp_ros2_interfaces/action/ExecuteTree "{target_tree: SleepActionSample}"
```
