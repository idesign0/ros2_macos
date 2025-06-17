// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rclcpp/rclcpp.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node_allow_undeclared = rclcpp::Node::make_shared(
    "test_parameters_server_allow_undeclared",
    "/",
    rclcpp::NodeOptions().allow_undeclared_parameters(true));

  auto node_must_declare = rclcpp::Node::make_shared(
    "test_parameters_server_must_declare",
    "/",
    rclcpp::NodeOptions().allow_undeclared_parameters(false));

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node_allow_undeclared);
  executor.add_node(node_must_declare);
  executor.spin();

  return 0;
}
