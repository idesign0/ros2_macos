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

#include <memory>
#include <utility>

#include "rclcpp/rclcpp.hpp"

#include "test_rclcpp/srv/add_two_ints.hpp"

void handle_add_two_ints_noreqid(
  const std::shared_ptr<test_rclcpp::srv::AddTwoInts::Request> request,
  std::shared_ptr<test_rclcpp::srv::AddTwoInts::Response> response)
{
  response->sum = request->a + request->b;
}

void handle_add_two_ints_reqid(
  const std::shared_ptr<rmw_request_id_t> request_header,
  const std::shared_ptr<test_rclcpp::srv::AddTwoInts::Request> request,
  std::shared_ptr<test_rclcpp::srv::AddTwoInts::Response> response)
{
  (void)request_header;
  response->sum = request->a + request->b;
}

class DeferedCbServiceWrapper : public std::enable_shared_from_this<DeferedCbServiceWrapper>
{
public:
  DeferedCbServiceWrapper() = default;

  // we need two steps initialization because of shared_from_this()
  void create_service(rclcpp::Node & node)
  {
    impl_ = node.create_service<test_rclcpp::srv::AddTwoInts>(
      "add_two_ints_defered_cb",
      [this](
        const std::shared_ptr<rmw_request_id_t> request_header,
        const std::shared_ptr<test_rclcpp::srv::AddTwoInts::Request> request)
      {
        if (handle_defered_response_thread_.joinable()) {
          throw std::runtime_error{"expected the callback to be called only once"};
        }
        auto shared_this = this->shared_from_this();
        handle_defered_response_thread_ = std::thread(
          [
            me = std::move(shared_this),
            request = std::move(request),
            request_header = std::move(request_header)]()
          {
            test_rclcpp::srv::AddTwoInts::Response response;
            response.sum = request->a + request->b;
            me->impl_->send_response(*request_header, response);
          });
      });
  }

  ~DeferedCbServiceWrapper()
  {
    if (handle_defered_response_thread_.joinable()) {
      handle_defered_response_thread_.join();
    }
  }

private:
  std::shared_ptr<rclcpp::Service<test_rclcpp::srv::AddTwoInts>> impl_;
  std::thread handle_defered_response_thread_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("add_two_ints_server");

  auto service_noreqid = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "add_two_ints_noreqid", handle_add_two_ints_noreqid);

  auto service_reqid = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "add_two_ints_reqid", handle_add_two_ints_reqid);

  auto service_return_req = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "add_two_ints_reqid_return_request", handle_add_two_ints_reqid);

  auto defered_cb = std::make_shared<DeferedCbServiceWrapper>();
  defered_cb->create_service(*node);

  rclcpp::TimerBase::SharedPtr timer;
  auto derefed_cb_with_handle = node->create_service<test_rclcpp::srv::AddTwoInts>(
    "add_two_ints_defered_cb_with_handle",
    [node, &timer](
      const std::shared_ptr<rclcpp::Service<test_rclcpp::srv::AddTwoInts>> handle,
      const std::shared_ptr<rmw_request_id_t> request_header,
      const std::shared_ptr<test_rclcpp::srv::AddTwoInts::Request> request)
    {
      // We defer handling the response in another callback, for example a timer.
      timer = node->create_wall_timer(
        std::chrono::nanoseconds{0},
        [
          handle = std::move(handle),
          header = std::move(request_header),
          request = std::move(request)
        ]()
        {
          test_rclcpp::srv::AddTwoInts::Response response;
          response.sum = request->a + request->b;
          handle->send_response(*header, response);
        });
    });

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
