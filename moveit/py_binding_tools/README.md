## Python binding tools for C++

This package provides some tools to facilitate the generation of python bindings for C++ code, based on [pybind11](https://github.com/pybind/pybind11).

### Automatic type conversion for ROS message types

Conversion between native C++ and python types is performed via ROS message serialization and deserialization, which is implemented via C++ templates. It suffices to include:

```cpp
#include <py_binding_tools/ros_msg_typecasters.h>
```

The `PoseStamped` message from the `geometry_msgs` package also accepts a single string as an argument.
In this case, the string is interpreted as the `header.frame_id` field of the message and the pose becomes the identity transform. To use this extension, include the following header instead:

```cpp
#include <py_binding_tools/geometry_msg_typecasters.h>
```

### C++ ROS initialization

C++ and Python use their own RCL implementations (`rclpy` and `rclcpp`).
Thus, it is necessary to initialize ROS in the C++ domain additionally to the Python domain before calling any ROS-related functions from wrapped C++ functions or classes.
To this end, the package provides the python function `rclcpp.init()` and the C++ class `RCLInitializer`. The latter is intended to be used as a base class for your python wrapper classes:

```cpp
class FooWrapper : protected RCLInitializer, public Foo {
	// ...
};
```

to ensure that the ROS infrastructure is initialized before usage in the wrapped C++ class. Ensure to list `RCLInitializer` as the _first_ base class, if ROS functionality is required in the constructor already!
