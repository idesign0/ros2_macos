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
#include <future>
#include <memory>

#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif


class WaitableWithTimer : public rclcpp::Waitable
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS_NOT_COPYABLE(WaitableWithTimer)

  explicit WaitableWithTimer(rclcpp::Clock::SharedPtr clock)
  {
    size_t period_nanoseconds = 100000;

    timer_.reset(new rcl_timer_t);
    *timer_ = rcl_get_zero_initialized_timer();
    rcl_clock_t * clock_handle = clock->get_clock_handle();
    rcl_ret_t ret = rcl_timer_init(
      timer_.get(),
      clock_handle,
      rclcpp::contexts::get_global_default_context()->get_rcl_context().get(),
      period_nanoseconds,
      nullptr,
      rcl_get_default_allocator());
    if (RCL_RET_OK != ret) {
      throw std::runtime_error("failed to create timer");
    }
  }

  virtual
  ~WaitableWithTimer() = default;

  size_t
  get_number_of_ready_timers() override
  {
    return 1u;
  }

  void
  add_to_wait_set(rcl_wait_set_t * wait_set) override
  {
    rcl_ret_t ret = rcl_wait_set_add_timer(wait_set, timer_.get(), &timer_idx_);
    if (RCL_RET_OK != ret) {
      throw std::runtime_error("failed to add timer to wait set");
    }
  }

  bool
  is_ready(rcl_wait_set_t * wait_set) override
  {
    if (timer_idx_ < wait_set->size_of_timers) {
      return nullptr != wait_set->timers[timer_idx_];
    }
    return false;
  }

  std::shared_ptr<void>
  take_data() override
  {
    return nullptr;
  }

  void
  execute(std::shared_ptr<void> & data) override
  {
    (void)data;
    rcl_ret_t ret = rcl_timer_call(timer_.get());
    execute_promise_.set_value(RCL_RET_OK == ret);
  }

  std::shared_ptr<rcl_timer_t> timer_;
  size_t timer_idx_;
  std::promise<bool> execute_promise_;
};  // class WaitableWithTimer

class CLASSNAME (test_waitable, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {
    rclcpp::init(0, nullptr);
  }

  static void TearDownTestCase()
  {
    rclcpp::shutdown();
  }
};

TEST_F(CLASSNAME(test_waitable, RMW_IMPLEMENTATION), waitable_with_timer) {
  auto node = rclcpp::Node::make_shared("waitable_with_timer");
  auto waitable = WaitableWithTimer::make_shared(node->get_clock());
  auto group = node->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
  node->get_node_waitables_interface()->add_waitable(waitable, group);

  std::shared_future<bool> fut(waitable->execute_promise_.get_future());
  rclcpp::spin_until_future_complete(node, fut);

  EXPECT_TRUE(fut.get());
}
