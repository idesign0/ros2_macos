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

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "rclcpp/rclcpp.hpp"

#include "test_communication/msg/u_int32.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto start = std::chrono::steady_clock::now();

  auto node = rclcpp::Node::make_shared("test_subscription_valid_data");

  auto callback =
    [](const test_communication::msg::UInt32::ConstSharedPtr received_message) -> void
    {
      printf("received message #%u\n", received_message->data);
      if (received_message->data == 0) {
        fprintf(stderr, "received message data was never sent\n");
        rclcpp::shutdown();
        throw std::runtime_error("received message data was never sent");
      }
    };

  auto subscriber = node->create_subscription<test_communication::msg::UInt32>(
    "test_subscription_valid_data", 10, callback);

  rclcpp::WallRate message_rate(5);
  {
    auto publisher = node->create_publisher<test_communication::msg::UInt32>(
      "test_subscription_valid_data", 10);

    message_rate.sleep();

    uint32_t index = 1;
    // publish a few messages, all with data > 0
    while (rclcpp::ok() && index <= 5) {
      printf("publishing message #%u\n", index);
      auto msg = std::make_unique<test_communication::msg::UInt32>();
      msg->data = index;
      publisher->publish(std::move(msg));
      ++index;
      message_rate.sleep();
      rclcpp::spin_some(node);
    }

    message_rate.sleep();
    rclcpp::spin_some(node);
  }
  // the publisher goes out of scope and the subscriber should be not receive any callbacks anymore

  message_rate.sleep();
  rclcpp::spin_some(node);

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("published and subscribed for %f seconds\n", diff.count());

  rclcpp::shutdown();
  return 0;
}
