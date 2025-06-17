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
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "rcpputils/scope_exit.hpp"

#include "test_msgs/action/fibonacci.hpp"
#include "test_msgs/action/nested_message.hpp"

template<typename ActionT>
struct ExpectedGoalRequest
{
  std::function<bool(std::shared_ptr<const typename ActionT::Goal>)> goal_is_expected;
  std::function<bool(std::shared_ptr<const typename ActionT::Goal>)> goal_is_valid;
  std::function<void(std::shared_ptr<rclcpp_action::ServerGoalHandle<ActionT>>)> execute_goal;
};

bool should_fail = false;

template<typename ActionT>
typename rclcpp_action::Server<ActionT>::SharedPtr
receive_goals(
  rclcpp::Node::SharedPtr node,
  const std::string & action_type_name,
  const std::vector<ExpectedGoalRequest<ActionT>> & expected_goal_requests
)
{
  auto logger = node->get_logger();

  auto goal_callback =
    [expected_goal_requests, logger](auto /* goal_id */, auto goal) {
      // make sure this goal was expected
      for (const auto & expected_goal_request : expected_goal_requests) {
        if (expected_goal_request.goal_is_expected(goal)) {
          if (!expected_goal_request.goal_is_valid(goal)) {
            RCLCPP_ERROR(logger, "goal request is invalid");
            should_fail = true;
            return rclcpp_action::GoalResponse::REJECT;
          }
          return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        }
      }
      RCLCPP_ERROR(logger, "unexpected goal request");
      should_fail = true;
      return rclcpp_action::GoalResponse::REJECT;
    };

  auto cancel_callback =
    [logger](auto /* goal_handle */) {
      RCLCPP_ERROR(logger, "received unexpected request to cancel goal");
      return rclcpp_action::CancelResponse::ACCEPT;
    };

  auto execute_callback =
    [expected_goal_requests, logger](auto goal_handle) {
      auto goal = goal_handle->get_goal();
      bool known_goal = false;
      size_t index = 1;
      for (const auto & expected_goal_request : expected_goal_requests) {
        if (expected_goal_request.goal_is_expected(goal)) {
          RCLCPP_INFO(logger, "executing goal #%zu of %zu", index, expected_goal_requests.size());
          known_goal = true;
          expected_goal_request.execute_goal(goal_handle);
          break;
        }
        index++;
      }
      if (!known_goal) {
        RCLCPP_ERROR(logger, "unexpected goal which was accepted (should not happen)");
        should_fail = true;
      }
    };

  return rclcpp_action::create_server<ActionT>(
    node,
    std::string("test/action/") + action_type_name,
    goal_callback,
    cancel_callback,
    execute_callback);
}

std::vector<ExpectedGoalRequest<test_msgs::action::Fibonacci>>
generate_expected_fibonacci_goals(rclcpp::Logger logger)
{
  std::vector<ExpectedGoalRequest<test_msgs::action::Fibonacci>> result;

  {
    ExpectedGoalRequest<test_msgs::action::Fibonacci> expected_goal;

    expected_goal.goal_is_expected =
      [](auto goal) {
        if (goal && goal->order == 10) {
          return true;
        }
        return false;
      };

    expected_goal.goal_is_valid =
      [](auto) {
        return true;
      };

    expected_goal.execute_goal =
      [logger](auto goal_handle) {
        const auto goal = goal_handle->get_goal();
        rclcpp::Rate loop_rate(10);

        auto feedback = std::make_shared<test_msgs::action::Fibonacci::Feedback>();
        feedback->sequence.push_back(0);
        feedback->sequence.push_back(1);

        auto result = std::make_shared<test_msgs::action::Fibonacci::Result>();

        if (goal->order <= 0) {
          RCLCPP_ERROR(logger, "expected a goal > 0, got %d", goal->order);
          return;
        }
        size_t order_size = goal->order;

        for (size_t i = 1; i < order_size; ++i) {
          if (!rclcpp::ok()) {
            return;
          }
          // Check if the goal was canceled.
          if (goal_handle->is_canceling()) {
            result->sequence = feedback->sequence;
            goal_handle->canceled(result);
            RCLCPP_INFO(logger, "goal was canceled");
            return;
          }
          // Update the sequence.
          feedback->sequence.push_back(
            feedback->sequence[i] + feedback->sequence[i - 1]);
          // Publish the current state as feedback.
          goal_handle->publish_feedback(feedback);
          RCLCPP_INFO(logger, "publishing feedback for goal");

          loop_rate.sleep();
        }

        result->sequence = feedback->sequence;
        goal_handle->succeed(result);
        RCLCPP_INFO(logger, "goal succeeded");
      };

    result.push_back(expected_goal);
  }

  return result;
}

std::vector<ExpectedGoalRequest<test_msgs::action::NestedMessage>>
generate_expected_nested_message_goals(rclcpp::Logger logger)
{
  std::vector<ExpectedGoalRequest<test_msgs::action::NestedMessage>> result;

  {
    ExpectedGoalRequest<test_msgs::action::NestedMessage> expected_goal;

    expected_goal.goal_is_expected =
      [](auto goal) {
        if (goal && goal->nested_field_no_pkg.duration_value.sec > 0) {
          return true;
        }
        return false;
      };

    expected_goal.goal_is_valid =
      [](auto) {
        return true;
      };

    expected_goal.execute_goal =
      [logger](auto goal_handle) {
        const auto goal = goal_handle->get_goal();
        rclcpp::Rate loop_rate(10);
        const int32_t initial_value = goal->nested_field_no_pkg.duration_value.sec;
        const int32_t feedback_value = 2 * initial_value;
        const int32_t result_value = 4 * initial_value;

        auto feedback = std::make_shared<test_msgs::action::NestedMessage::Feedback>();

        auto result = std::make_shared<test_msgs::action::NestedMessage::Result>();

        if (initial_value <= 0) {
          RCLCPP_ERROR(logger, "expected a goal > 0, got %d", initial_value);
          return;
        }
        const size_t num_feedback = 10;

        for (size_t i = 1; i < num_feedback; ++i) {
          if (!rclcpp::ok()) {
            return;
          }
          // Check if the goal was canceled.
          if (goal_handle->is_canceling()) {
            result->nested_field.int32_value = result_value;
            goal_handle->canceled(result);
            RCLCPP_INFO(logger, "goal was canceled");
            return;
          }
          // Update the feedback;
          feedback->nested_different_pkg.sec = feedback_value;
          // Publish the current state as feedback.
          goal_handle->publish_feedback(feedback);
          RCLCPP_INFO(logger, "publishing feedback for goal");

          loop_rate.sleep();
        }

        result->nested_field.int32_value = result_value;
        goal_handle->succeed(result);
        RCLCPP_INFO(logger, "goal succeeded");
      };

    result.push_back(expected_goal);
  }

  return result;
}

int main(int argc, char ** argv)
{
  if (argc != 3) {
    fprintf(stderr, "Wrong number of arguments, pass one action type and a namespace\n");
    return 1;
  }
  rclcpp::init(argc, argv);
  RCPPUTILS_SCOPE_EXIT(
  {
    rclcpp::shutdown();
  });

  auto start = std::chrono::steady_clock::now();

  std::string action = argv[1];
  std::string namespace_ = argv[2];
  auto node = rclcpp::Node::make_shared(std::string("test_action_server_") + action, namespace_);

  rclcpp_action::ServerBase::SharedPtr server;

  if (action == "Fibonacci") {
    server = receive_goals<test_msgs::action::Fibonacci>(
      node, action, generate_expected_fibonacci_goals(node->get_logger()));
  } else if (action == "NestedMessage") {
    server = receive_goals<test_msgs::action::NestedMessage>(
      node, action, generate_expected_nested_message_goals(node->get_logger()));
  } else {
    fprintf(stderr, "Unknown action type '%s'\n", action.c_str());
    return 1;
  }
  rclcpp::spin(node);

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> diff = (end - start);
  printf("replied for %f seconds\n", diff.count());

  return should_fail ? 1 : 0;
}
