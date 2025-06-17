// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include <chrono>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "test_msgs/msg/empty.hpp"
#include "test_msgs/srv/empty.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  std::string node_name = "original_node_name";
  std::string namespace_ = "/original/namespace";
  auto node = rclcpp::Node::make_shared(node_name, namespace_);

  auto pub1 = node->create_publisher<test_msgs::msg::Empty>("~/private/name", 10);
  auto pub2 = node->create_publisher<test_msgs::msg::Empty>("relative/name", 10);
  auto pub3 = node->create_publisher<test_msgs::msg::Empty>("/fully/qualified/name", 10);

  auto do_nothing = [](
    const test_msgs::srv::Empty::Request::SharedPtr request,
    test_msgs::srv::Empty::Response::SharedPtr response) -> void
    {
      static_cast<void>(request);
      static_cast<void>(response);
    };

  auto srv1 = node->create_service<test_msgs::srv::Empty>("~/private/name", do_nothing);
  auto srv2 = node->create_service<test_msgs::srv::Empty>("relative/name", do_nothing);
  auto srv3 = node->create_service<test_msgs::srv::Empty>("/fully/qualified/name", do_nothing);

  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
