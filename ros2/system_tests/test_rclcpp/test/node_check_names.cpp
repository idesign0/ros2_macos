// Copyright 2019 Open Source Robotics Foundation, Inc.
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

  if (argc < 2) {
    fprintf(
      stderr, "Must pass at least one argument with the expected number of nodes\n");
    return 1;
  }

  int num_nodes = ::strtol(argv[1], nullptr, 10);
  std::string node_to_look_for("/node_with_name");
  printf("Waiting for %d nodes with name: node_with_name_N\n", num_nodes);
  std::cout.flush();

  auto node_name = std::string("node_check_names");
  auto node = rclcpp::Node::make_shared(node_name);

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(node);

  int counter = 0;
  const std::chrono::steady_clock::time_point max_runtime =
    std::chrono::steady_clock::now() + std::chrono::seconds(10);

  while (rclcpp::ok()) {
    auto names = node->get_node_graph_interface()->get_node_names();
    for (auto it : names) {
      if (it.compare(0, node_to_look_for.length(), node_to_look_for) == 0) {
        counter++;
      }
    }
    if (counter >= num_nodes) {
      break;
    }
    std::cout.flush();
    if (std::chrono::steady_clock::now() >= max_runtime) {
      break;
    }
    exec.spin_once(std::chrono::milliseconds(250));
  }

  exec.remove_node(node);
  rclcpp::shutdown();
  if (counter < num_nodes) {
    fprintf(stderr, "Did not find all %d nodes\n", num_nodes);
    fprintf(stderr, "Found %d nodes\n", counter);
    return 1;
  }
  return 0;
}
