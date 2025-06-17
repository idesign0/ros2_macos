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

#include <string>

#include "rclcpp/rclcpp.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node_name = std::string("node_with_name");
  if (argc >= 2) {
    node_name += "_";
    node_name += argv[1];
  }
  printf("Starting node with name: %s\n", node_name.c_str());
  std::cout.flush();

  rclcpp::Node::SharedPtr node;
  try {
    node = rclcpp::Node::make_shared(node_name);
  } catch (rclcpp::exceptions::RCLError & e) {
    // test may pass and send SIGINT before node finishes initializing ros2/build_cop#153
    printf("Ignoring RCLError: %s\n", e.what());
  }

  if (node) {
    rclcpp::spin(node);
  }
  rclcpp::shutdown();
  return 0;
}
