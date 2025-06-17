# System Tests

New system tests may be added when new functionality needs to be tested using the complete ROS 2 core.
Before contributing new system tests please discuss the tests you would like to add with the ROS 2 team.
If you're implementing new features that may benefit from additional system tests you can ask the reviewers of your pull requests whether system tests are desired.
If you would like to add system tests for existing features please open an issue explaining the proposed tests and rationale so it can be reviewed before you start work.

When implementing additional system tests on you could start by using existing tests as a template.

In the `test_rclcpp` package there is a test of a node with a single service.
This test includes a [server executable](test_rclcpp/test/test_services_server.cpp) and a [client executable](test_rclcpp/test/test_services_client.cpp).
The server executable is a "plain" ROS 2 node implementing a service and the client executable contains GTEST macros testing functionality.
These are included in the build from [this stanza](test_rclcpp/CMakeLists.txt#L281) of `CMakeLists.txt` and added to [this macro](test_rclcpp/CMakeLists.txt#207) of tests to be run for each rmw implementation.
