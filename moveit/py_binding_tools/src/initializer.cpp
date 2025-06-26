/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2020, Bielefeld University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Bielefeld University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include <memory>
#include <thread>
#include <mutex>

#include <py_binding_tools/initializer.h>
#include <rclcpp/node.hpp>
#include <rclcpp/executors.hpp>

namespace py_binding_tools
{
namespace
{
static const rclcpp::Logger LOGGER = rclcpp::get_logger("InitProxy");

/// singleton class to initialize ROS C++ (once) from Python
class InitProxy
{
public:
  static void init(const std::vector<std::string>& args = {});
  static void reset(bool shutdown);
  static void add_node(const rclcpp::Node::SharedPtr& node)
  {
    if (singleton_instance_)
      singleton_instance_->executor_->add_node(node);
  }

  ~InitProxy()
  {
    stop_ = true;
    spinner_.join();
    rclcpp::shutdown();
  }

private:
  InitProxy(const std::vector<std::string>& args)
  {
    usage_ = 0;
    stop_ = false;
    if (!rclcpp::ok())
    {
      std::vector<const char*> raw_args;
      raw_args.reserve(args.size());
      for (auto& arg : args)
        raw_args.push_back(arg.c_str());
      rclcpp::init(raw_args.size(), raw_args.data());
    }
    else
      RCLCPP_WARN_STREAM(LOGGER, "rclcpp::init was already called by other means.");

    // create an executor to spin nodes
    rclcpp::ExecutorOptions eo;
    eo.context = rclcpp::contexts::get_global_default_context();
    executor_ = std::make_unique<rclcpp::executors::SingleThreadedExecutor>(eo);

    spinner_ = std::thread([this]() {
      while (!stop_ && rclcpp::ok())
        executor_->spin_all(std::chrono::milliseconds(100));
    });
  }

  static std::mutex mutex_;
  static std::unique_ptr<InitProxy> singleton_instance_;
  unsigned int usage_;
  bool stop_;
  rclcpp::executors::SingleThreadedExecutor::UniquePtr executor_;
  std::thread spinner_;
};
std::mutex InitProxy::mutex_;
std::unique_ptr<InitProxy> InitProxy::singleton_instance_;
}  // namespace

void InitProxy::init(const std::vector<std::string>& args)
{
  std::unique_lock<std::mutex> lock(mutex_);
  if (!singleton_instance_)
    singleton_instance_.reset(new InitProxy(args));
  ++singleton_instance_->usage_;
}

void InitProxy::reset(bool shutdown)
{
  std::unique_lock<std::mutex> lock(mutex_);
  if (!singleton_instance_ || singleton_instance_->usage_ == 0)
    RCLCPP_ERROR(LOGGER, "Called rclcpp.shutdown() without a matching call to rclcpp.init()!");
  else if (--singleton_instance_->usage_ == 0 && shutdown)
    singleton_instance_.reset();
}

RCLInitializer::RCLInitializer()
{
  InitProxy::init();
}
RCLInitializer::~RCLInitializer()
{
  InitProxy::reset(false);
}

void init(const std::vector<std::string>& args)
{
  InitProxy::init(args);
}

void shutdown()
{
  InitProxy::reset(true);
}

void add_node(const rclcpp::Node::SharedPtr& node)
{
  InitProxy::add_node(node);
}

}  // namespace py_binding_tools
