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
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "rclcpp/exceptions.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "rcpputils/scope_exit.hpp"

#include "test_msgs/action/fibonacci.hpp"
#include "test_msgs/action/nested_message.hpp"

using namespace std::chrono_literals;

template<typename ActionT>
struct ActionClientTest
{
  typename ActionT::Goal goal;
  std::function<bool(typename ActionT::Result::SharedPtr)> result_is_valid;
  std::function<bool(typename ActionT::Feedback::ConstSharedPtr)> feedback_is_valid;
};

template<typename ActionT>
int
send_goals(
  rclcpp::Node::SharedPtr node,
  const std::string & action_type_name,
  const std::vector<ActionClientTest<ActionT>> & goal_tests)
{
  auto action_client =
    rclcpp_action::create_client<ActionT>(node, "test/action/" + action_type_name);
  auto logger = node->get_logger();

  if (!action_client->wait_for_action_server(20s)) {
    RCLCPP_ERROR(logger, "requester service not available after waiting");
    return 1;
  }

  size_t test_index = 0;
  bool invalid_feedback = false;
  auto start = std::chrono::steady_clock::now();
  RCPPUTILS_SCOPE_EXIT(
  {
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = (end - start);
    RCLCPP_INFO(logger, "sent goals for %f seconds\n", diff.count());
  });

  while (rclcpp::ok() && test_index < goal_tests.size()) {
    RCLCPP_INFO(logger, "sending goal #%zu", test_index + 1);

    // on feedback, check the feedback is valid
    auto feedback_callback =
      [&](auto, const auto & feedback) {
        RCLCPP_INFO(logger, "received feedback");
        if (!goal_tests[test_index].feedback_is_valid(feedback)) {
          RCLCPP_ERROR(logger, "invalid feedback");
          invalid_feedback = true;
        }
      };

    // send the request
    auto send_goal_options = typename rclcpp_action::Client<ActionT>::SendGoalOptions();
    send_goal_options.feedback_callback = feedback_callback;
    auto goal_handle_future =
      action_client->async_send_goal(goal_tests[test_index].goal, send_goal_options);

    using rclcpp::FutureReturnCode;
    // wait for the sent goal to be accepted
    auto status = rclcpp::spin_until_future_complete(node, goal_handle_future, 1000s);
    if (status != FutureReturnCode::SUCCESS) {
      RCLCPP_ERROR(logger, "send goal call failed");
      return 1;
    }

    // wait for the result (feedback may be received in the meantime)
    auto result_future = action_client->async_get_result(goal_handle_future.get());
    status = rclcpp::spin_until_future_complete(node, result_future, 1000s);
    if (status != FutureReturnCode::SUCCESS) {
      RCLCPP_ERROR(logger, "failed to receive a goal result in time");
      return 1;
    }

    if (!goal_tests[test_index].result_is_valid(result_future.get().result)) {
      RCLCPP_ERROR(logger, "invalid goal result");
      return 1;
    }
    RCLCPP_INFO(logger, "received goal #%zu of %zu", test_index + 1, goal_tests.size());
    test_index++;
  }

  if (test_index != goal_tests.size()) {
    return 1;
  }

  if (invalid_feedback) {
    return 1;
  }

  return 0;
}

std::vector<ActionClientTest<test_msgs::action::Fibonacci>>
generate_fibonacci_goal_tests()
{
  std::vector<ActionClientTest<test_msgs::action::Fibonacci>> result;

  constexpr size_t order = 10;
  static_assert(order > 0, "order needs to be non-zero");

  std::vector<int32_t> valid_fibo_seq;
  valid_fibo_seq.push_back(0);
  valid_fibo_seq.push_back(1);
  for (size_t i = 1; i < order; ++i) {
    valid_fibo_seq.push_back(valid_fibo_seq[i] + valid_fibo_seq[i - 1]);
  }

  {
    ActionClientTest<test_msgs::action::Fibonacci> test;
    size_t order = 10;
    test.goal.order = static_cast<int32_t>(order);
    test.result_is_valid =
      [order, valid_fibo_seq](auto result) -> bool {
        if (result->sequence.size() != (order + 1)) {
          fprintf(stderr, "result sequence not equal to goal order\n");
          return false;
        }
        for (size_t i = 0; i < order; ++i) {
          if (valid_fibo_seq[i] != result->sequence[i]) {
            fprintf(
              stderr,
              "result sequence not correct, expected %d but got %d for order %zu\n",
              valid_fibo_seq[i], result->sequence[i], i);
            return false;
          }
        }
        return true;
      };
    test.feedback_is_valid =
      [order, valid_fibo_seq](auto feedback) -> bool {
        if (feedback->sequence.size() > (order + 1)) {
          fprintf(stderr, "feedback sequence greater than the goal order\n");
          return false;
        }
        for (size_t i = 0; i < feedback->sequence.size(); ++i) {
          if (valid_fibo_seq[i] != feedback->sequence[i]) {
            fprintf(
              stderr,
              "feedback sequence not correct, expected %d but got %d for order %zu\n",
              valid_fibo_seq[i], feedback->sequence[i], i);
            return false;
          }
        }
        return true;
      };
    result.push_back(test);
  }

  return result;
}

std::vector<ActionClientTest<test_msgs::action::NestedMessage>>
generate_nested_message_goal_tests()
{
  std::vector<ActionClientTest<test_msgs::action::NestedMessage>> result;

  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(1, 12345);
  const int32_t initial_value = distribution(generator);
  const int32_t expected_feedback_value = 2 * initial_value;
  const int32_t expected_result_value = 4 * initial_value;

  {
    ActionClientTest<test_msgs::action::NestedMessage> test;
    test.goal.nested_field_no_pkg.duration_value.sec = initial_value;
    test.result_is_valid =
      [initial_value, expected_result_value](auto result) -> bool {
        if (result->nested_field.int32_value != expected_result_value) {
          fprintf(
            stderr, "expected result %d but got %d for initial value %d\n",
            expected_result_value, result->nested_field.int32_value, initial_value);
          return false;
        }
        return true;
      };
    test.feedback_is_valid =
      [initial_value, expected_feedback_value](auto feedback) -> bool {
        if (feedback->nested_different_pkg.sec != expected_feedback_value) {
          fprintf(
            stderr, "expected feedback %d but got %d for initial value %d\n",
            expected_feedback_value, feedback->nested_different_pkg.sec, initial_value);
          return false;
        }
        return true;
      };
    result.push_back(test);
  }

  return result;
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  RCPPUTILS_SCOPE_EXIT(
  {
    rclcpp::shutdown();
  });
  if (argc != 3) {
    fprintf(stderr, "Wrong number of arguments, pass an action type and a namespace\n");
    return 1;
  }

  std::string action = argv[1];
  std::string namespace_ = argv[2];
  auto node = rclcpp::Node::make_shared("test_action_client_" + action, namespace_);

  int rc;
  if (action == "Fibonacci") {
    rc = send_goals<test_msgs::action::Fibonacci>(node, action, generate_fibonacci_goal_tests());
  } else if (action == "NestedMessage") {
    rc = send_goals<test_msgs::action::NestedMessage>(
      node, action, generate_nested_message_goal_tests());
  } else {
    fprintf(stderr, "Unknown action type '%s'\n", action.c_str());
    return 1;
  }
  return rc;
}
