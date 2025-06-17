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
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "rclcpp/exceptions.hpp"
#include "rclcpp/rclcpp.hpp"

#include "test_msgs/service_fixtures.hpp"

using namespace std::chrono_literals;

template<typename T>
int request(
  rclcpp::Node::SharedPtr node,
  const std::string & service_type,
  std::vector<
    std::pair<
      typename T::Request::SharedPtr,
      typename T::Response::SharedPtr
    >
  > services,
  size_t number_of_cycles = 10)
{
  int rc = 0;
  auto requester = node->create_client<T>(std::string("test/service/") + service_type);
  if (!requester->wait_for_service(20s)) {
    throw std::runtime_error("requester service not available after waiting");
  }

  rclcpp::WallRate cycle_rate(10);
  auto wait_between_services = std::chrono::milliseconds(100);
  size_t cycle_index = 0;
  size_t service_index = 0;
  auto start = std::chrono::steady_clock::now();
  // publish the first request up to number_of_cycles times, longer sleep between each cycle
  // publish all requests one by one, shorter sleep between each request
  while (rclcpp::ok() && cycle_index < number_of_cycles && service_index < services.size()) {
    printf("publishing request #%zu\n", service_index + 1);
    auto f = requester->async_send_request(services[service_index].first);

    auto wait_for_response_until = std::chrono::steady_clock::now() + wait_between_services;
    std::future_status status;
    do {
      rclcpp::spin_some(node);
      status = f.wait_for(std::chrono::milliseconds(10));
    } while (
      status != std::future_status::ready && rclcpp::ok() &&
      std::chrono::steady_clock::now() < wait_for_response_until
    );

    if (std::future_status::ready == status) {
      if (*f.get() == *services[service_index].second) {
        printf("received reply #%zu of %zu\n", service_index + 1, services.size());
        ++service_index;
      } else {
        fprintf(
          stderr, "received reply for request #%zu does not match the expected reply\n",
          service_index + 1);
        rc = 2;
        break;
      }
    } else {
      if (service_index == 0) {
        // retry first request up to number_of_cycles times
        ++cycle_index;
        cycle_rate.sleep();
      } else {
        // for not-first requests the reply must arrive without retrying
        rc = 3;
        break;
      }
    }
  }

  if (service_index != services.size()) {
    rc = 1;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("requested for %f seconds\n", diff.count());

  return rc;
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  if (argc != 3) {
    fprintf(stderr, "Wrong number of arguments, pass one service type\n");
    return 1;
  }

  std::string service = argv[1];
  std::string namespace_ = argv[2];
  auto node = rclcpp::Node::make_shared(
    std::string("test_requester_") + service,
    namespace_);

  int rc;
  if (service == "Empty") {
    rc = request<test_msgs::srv::Empty>(
      node, service, get_services_empty());
  } else if (service == "BasicTypes") {
    rc = request<test_msgs::srv::BasicTypes>(
      node, service, get_services_basic_types());
  } else if (service == "Arrays") {
    rc = request<test_msgs::srv::Arrays>(
      node, service, get_services_arrays());
  } else {
    fprintf(stderr, "Unknown service argument '%s'\n", service.c_str());
    rclcpp::shutdown();
    return 1;
  }
  rclcpp::shutdown();
  return rc;
}
