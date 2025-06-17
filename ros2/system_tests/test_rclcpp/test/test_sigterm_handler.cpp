// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include <iostream>
#include <memory>

#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

void sigterm_handler(int signum)
{
  (void)signum;
  RCLCPP_INFO(rclcpp::get_logger("test_sigterm_handler"), "Custom sigterm handler called.");
}

int main(int argc, char ** argv)
{
  // This exectuable is used in combination with a script that interrupts it at different times.
  // Its purpose is to test that a user-defined signal handler gets called even when the rclcpp
  // signal handler is/has been in use.

  // Override default sigterm handler
  signal(SIGTERM, sigterm_handler);
  RCLCPP_INFO(rclcpp::get_logger("test_sigterm_handler"), "Registered custom signal handler.");

  rclcpp::init(argc, argv);
  RCLCPP_INFO(rclcpp::get_logger("test_sigterm_handler"), "Called rclcpp::init.");

  RCLCPP_INFO(
    rclcpp::get_logger("test_sigterm_handler"),
    "Waiting to give an opportunity for interrupt...");
  std::this_thread::sleep_for(5s);

  RCLCPP_INFO(rclcpp::get_logger("test_sigterm_handler"), "Calling rclcpp::shutdown...");
  rclcpp::shutdown();
  RCLCPP_INFO(rclcpp::get_logger("test_sigterm_handler"), "Called rclcpp::shutdown.");

  RCLCPP_INFO(
    rclcpp::get_logger("test_sigterm_handler"),
    "Waiting to give an opportunity for interrupt...");
  std::this_thread::sleep_for(5s);

  RCLCPP_INFO(rclcpp::get_logger("test_sigterm_handler"), "Exiting.");
  return 0;
}
