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

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <type_traits>

#include "rclcpp/strategies/allocator_memory_strategy.hpp"
#include "rclcpp/rclcpp.hpp"

#include "std_msgs/msg/u_int32.hpp"
#include "tlsf_cpp/tlsf.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

template<typename T = void>
using TLSFAllocator = tlsf_heap_allocator<T>;

using rclcpp::memory_strategies::allocator_memory_strategy::AllocatorMemoryStrategy;

class CLASSNAME (AllocatorTest, RMW_IMPLEMENTATION) : public ::testing::Test
{
protected:
  std::string test_name_;

  rclcpp::Node::SharedPtr node_;
  rclcpp::executors::SingleThreadedExecutor::SharedPtr executor_;
  rclcpp::memory_strategy::MemoryStrategy::SharedPtr memory_strategy_;
  rclcpp::Publisher<
    std_msgs::msg::UInt32, TLSFAllocator<void>>::SharedPtr publisher_;
  rclcpp::message_memory_strategy::MessageMemoryStrategy<
    std_msgs::msg::UInt32, TLSFAllocator<void>>::SharedPtr msg_memory_strategy_;
  std::shared_ptr<TLSFAllocator<void>> alloc;
  rclcpp::SubscriptionOptionsWithAllocator<TLSFAllocator<void>> subscription_options_;
  rclcpp::PublisherOptionsWithAllocator<TLSFAllocator<void>> publisher_options_;

  bool intra_process_;

  using UInt32Allocator = TLSFAllocator<std_msgs::msg::UInt32>;
  using UInt32Deleter = rclcpp::allocator::Deleter<UInt32Allocator, std_msgs::msg::UInt32>;

  void initialize(bool intra_process, const std::string & name)
  {
    test_name_ = name;
    intra_process_ = intra_process;

    auto context = rclcpp::contexts::get_global_default_context();
    auto options = rclcpp::NodeOptions()
      .context(context)
      .use_global_arguments(true)
      .use_intra_process_comms(intra_process);

    node_ = rclcpp::Node::make_shared(name, options);
    alloc = std::make_shared<TLSFAllocator<void>>();
    subscription_options_.allocator = alloc;
    publisher_options_.allocator = alloc;
    msg_memory_strategy_ = std::make_shared<
      rclcpp::message_memory_strategy::MessageMemoryStrategy<
        std_msgs::msg::UInt32, TLSFAllocator<void>>>(alloc);
    publisher_ = node_->create_publisher<std_msgs::msg::UInt32>(name, 10, publisher_options_);
    memory_strategy_ =
      std::make_shared<AllocatorMemoryStrategy<TLSFAllocator<void>>>(alloc);

    rclcpp::ExecutorOptions executor_options;
    executor_options.memory_strategy = memory_strategy_;
    executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>(executor_options);

    executor_->add_node(node_);
  }

  CLASSNAME(AllocatorTest, RMW_IMPLEMENTATION)() {
  }

  ~CLASSNAME(AllocatorTest, RMW_IMPLEMENTATION)() {
  }
};

TEST_F(CLASSNAME(AllocatorTest, RMW_IMPLEMENTATION), type_traits_test) {
  using UInt32TLSFAllocator = TLSFAllocator<std_msgs::msg::UInt32>;
  using UInt32TLSFDeleter = rclcpp::allocator::Deleter<UInt32TLSFAllocator, std_msgs::msg::UInt32>;

  auto cb_tlsf = [](std_msgs::msg::UInt32::UniquePtrWithDeleter<UInt32TLSFDeleter> msg) -> void
    {
      (void) msg;
    };
  static_assert(
    std::is_same<
      std_msgs::msg::UInt32,
      rclcpp::subscription_traits::has_message_type<decltype(cb_tlsf)>::type>::value,
    "tlsf unique ptr failed");

  using UInt32VoidAllocator = std::allocator<std_msgs::msg::UInt32>;
  using UInt32VoidDeleter = rclcpp::allocator::Deleter<UInt32VoidAllocator, std_msgs::msg::UInt32>;

  auto cb_void = [](std_msgs::msg::UInt32::UniquePtrWithDeleter<UInt32VoidDeleter> msg) -> void
    {
      (void) msg;
    };
  static_assert(
    std::is_same<
      std_msgs::msg::UInt32,
      rclcpp::subscription_traits::has_message_type<decltype(cb_void)>::type>::value,
    "void unique ptr failed");
}

/**
// TODO(wjwwood): re-enable this test when the allocator has been added back to the
//   intra-process manager.
//   See: https://github.com/ros2/realtime_support/pull/80#issuecomment-545419570
TEST_F(CLASSNAME(AllocatorTest, RMW_IMPLEMENTATION), allocator_unique_ptr) {
  initialize(true, "allocator_unique_ptr");
  size_t counter = 0;
  auto callback =
    [&counter](std::unique_ptr<std_msgs::msg::UInt32, UInt32Deleter> msg) -> void
    {
      EXPECT_EQ(counter, msg->data);
      counter++;
    };

  static_assert(
    std::is_same<
      std_msgs::msg::UInt32,
      rclcpp::subscription_traits::has_message_type<decltype(callback)>::type>::value,
    "passing a std::unique_ptr of test_msgs::msg::Empty has message type Empty");

  auto subscriber = node_->create_subscription<std_msgs::msg::UInt32>(
    "allocator_unique_ptr", 10, callback, subscription_options_, msg_memory_strategy_);

  TLSFAllocator<std_msgs::msg::UInt32> msg_alloc;

  // After test_initialization, global new should only be called from within TLSFAllocator.
  test_init = true;
  for (uint32_t i = 0; i < iterations; i++) {
    auto msg = std::unique_ptr<std_msgs::msg::UInt32, UInt32Deleter>(
      std::allocator_traits<UInt32Allocator>::allocate(msg_alloc, 1));
    msg->data = i;
    publisher_->publish(std::move(msg));
    rclcpp::sleep_for(std::chrono::milliseconds(1));
    executor_->spin_some();
  }
  test_init = false;
  EXPECT_FALSE(fail);
  fail = false;
}
*/

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
