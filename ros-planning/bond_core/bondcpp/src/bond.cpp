// Copyright (c) 2009, Willow Garage, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Willow Garage nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// Author: Stuart Glaser

#include <bondcpp/bond.hpp>
#include <bondcpp/createSafeCallback.hpp>

#ifdef _WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif

#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>

using namespace std::chrono_literals;

namespace bond
{
static std::string makeUUID()
{
#ifdef _WIN32
  UUID uuid;
  UuidCreate(&uuid);
  unsigned char * str;
  UuidToStringA(&uuid, &str);
  std::string return_string(reinterpret_cast<char *>(str));
  RpcStringFreeA(&str);
  return return_string;
#else
  uuid_t uuid;
  uuid_generate_random(uuid);
  char uuid_str[40];
  uuid_unparse(uuid, uuid_str);
  return std::string(uuid_str);
#endif
}

Bond::Bond(
  const std::string & topic, const std::string & id,
  rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base,
  rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr node_logging,
  rclcpp::node_interfaces::NodeParametersInterface::SharedPtr node_params,
  rclcpp::node_interfaces::NodeTimersInterface::SharedPtr node_timers,
  rclcpp::node_interfaces::NodeTopicsInterface::SharedPtr node_topics,
  EventCallback on_broken,
  EventCallback on_formed)
: node_base_(node_base),
  node_logging_(node_logging),
  node_params_(node_params),
  node_timers_(node_timers),
  node_topics_(node_topics),
  bondsm_(std::make_unique<BondSM>(this)),
  sm_(*bondsm_),
  topic_(topic),
  id_(id),
  instance_id_(makeUUID()),
  on_broken_(on_broken),
  on_formed_(on_formed),
  connect_timeout_(
    rclcpp::Duration::from_seconds(bond::msg::Constants::DEFAULT_CONNECT_TIMEOUT)),
  disconnect_timeout_(
    rclcpp::Duration::from_seconds(bond::msg::Constants::DEFAULT_DISCONNECT_TIMEOUT)),
  heartbeat_timeout_(
    rclcpp::Duration::from_seconds(bond::msg::Constants::DEFAULT_HEARTBEAT_TIMEOUT)),
  heartbeat_period_(
    rclcpp::Duration::from_seconds(bond::msg::Constants::DEFAULT_HEARTBEAT_PERIOD)),
  dead_publish_period_(
    rclcpp::Duration::from_seconds(bond::msg::Constants::DEAD_PUBLISH_PERIOD))
{
  if (!node_params_->has_parameter(bond::msg::Constants::DISABLE_HEARTBEAT_TIMEOUT_PARAM)) {
    node_params_->declare_parameter(
      bond::msg::Constants::DISABLE_HEARTBEAT_TIMEOUT_PARAM,
      rclcpp::ParameterValue(false));
  }

  disable_heartbeat_timeout_ =
    node_params_->get_parameter(bond::msg::Constants::DISABLE_HEARTBEAT_TIMEOUT_PARAM).as_bool();

  setupConnections();

  pub_ = rclcpp::create_publisher<bond::msg::Status>(
    node_params_,
    node_topics_,
    topic_,
    rclcpp::QoS(rclcpp::KeepLast(5)));
}

Bond::Bond(
  const std::string & topic, const std::string & id,
  rclcpp_lifecycle::LifecycleNode::SharedPtr nh,
  EventCallback on_broken,
  EventCallback on_formed)
: Bond(topic, id,
    nh->get_node_base_interface(),
    nh->get_node_logging_interface(),
    nh->get_node_parameters_interface(),
    nh->get_node_timers_interface(),
    nh->get_node_topics_interface(),
    on_broken, on_formed)
{
}

Bond::Bond(
  const std::string & topic, const std::string & id,
  rclcpp::Node::SharedPtr nh,
  EventCallback on_broken,
  EventCallback on_formed)
: Bond(topic, id,
    nh->get_node_base_interface(),
    nh->get_node_logging_interface(),
    nh->get_node_parameters_interface(),
    nh->get_node_timers_interface(),
    nh->get_node_topics_interface(),
    on_broken, on_formed)
{
}

Bond::~Bond()
{
  if (!started_) {
    return;
  }
  breakBond();
  if (rclcpp::ok() && !waitUntilBroken(rclcpp::Duration(100ms))) {
    RCLCPP_DEBUG(
      node_logging_->get_logger(), "Bond failed to break on destruction %s (%s)",
      id_.c_str(), instance_id_.c_str());
  }

  publishingTimerCancel();
  deadpublishingTimerCancel();
  connectTimerCancel();
  heartbeatTimerCancel();
  disconnectTimerCancel();
}

void Bond::setupConnections()
{
  setConnectTimeout(bond::msg::Constants::DEFAULT_CONNECT_TIMEOUT);
  setDisconnectTimeout(bond::msg::Constants::DEFAULT_DISCONNECT_TIMEOUT);
  setHeartbeatTimeout(bond::msg::Constants::DEFAULT_HEARTBEAT_TIMEOUT);
  setHeartbeatPeriod(bond::msg::Constants::DEFAULT_HEARTBEAT_PERIOD);
  setDeadPublishPeriod(bond::msg::Constants::DEAD_PUBLISH_PERIOD);
}

void Bond::setConnectTimeout(double dur)
{
  if (started_) {
    RCLCPP_ERROR(node_logging_->get_logger(), "Cannot set timeouts after calling start()");
    return;
  }
  connect_timeout_ = rclcpp::Duration::from_seconds(dur);
}

void Bond::connectTimerReset()
{
  // Callback function of connect timer
  auto connectTimerResetCallback =
    [this]() -> void
    {
      if (connect_timer_reset_flag_ && started_) {
        onConnectTimeout();
        connect_timer_->cancel();
        connect_timer_reset_flag_ = false;
      }    // flag is needed to have valid callback
    };
  // Connect timer started on node
  connect_timer_ = rclcpp::create_wall_timer(
    connect_timeout_.to_chrono<std::chrono::nanoseconds>(),
    connectTimerResetCallback,
    nullptr,
    node_base_.get(),
    node_timers_.get());
}

void Bond::connectTimerCancel()
{
  if (connect_timer_ && !connect_timer_->is_canceled()) {
    connect_timer_->cancel();
  }
}

void Bond::setDisconnectTimeout(double dur)
{
  if (started_) {
    RCLCPP_ERROR(node_logging_->get_logger(), "Cannot set timeouts after calling start()");
    return;
  }
  disconnect_timeout_ = rclcpp::Duration::from_seconds(dur);
}

void Bond::disconnectTimerReset()
{
  // Callback function of disconnect timer
  auto disconnectTimerResetCallback =
    [this]() -> void
    {
      if (disconnect_timer_reset_flag_ && started_) {
        onDisconnectTimeout();
        disconnect_timer_->cancel();
        disconnect_timer_reset_flag_ = false;
      }    // flag is needed to have valid callback
    };
  //  Disconnect timer started on node
  disconnect_timer_ = rclcpp::create_wall_timer(
    disconnect_timeout_.to_chrono<std::chrono::nanoseconds>(),
    disconnectTimerResetCallback,
    nullptr,
    node_base_.get(),
    node_timers_.get());
}

void Bond::disconnectTimerCancel()
{
  if (disconnect_timer_ && !disconnect_timer_->is_canceled()) {
    disconnect_timer_->cancel();
  }
}

void Bond::setHeartbeatTimeout(double dur)
{
  if (started_) {
    RCLCPP_ERROR(node_logging_->get_logger(), "Cannot set timeouts after calling start()");
    return;
  }

  heartbeat_timeout_ = rclcpp::Duration::from_seconds(dur);
}

void Bond::heartbeatTimerReset()
{
  //  Callback function of heartbeat timer
  auto heartbeatTimerResetCallback =
    [this]() -> void
    {
      if (!started_ || disable_heartbeat_timeout_) {
        return;
      }

      onHeartbeatTimeout();
    };
  //    heartbeat timer started on node
  heartbeat_timer_ = rclcpp::create_wall_timer(
    heartbeat_timeout_.to_chrono<std::chrono::nanoseconds>(),
    heartbeatTimerResetCallback,
    nullptr, node_base_.get(), node_timers_.get());
}

void Bond::heartbeatTimerCancel()
{
  if (heartbeat_timer_ && !heartbeat_timer_->is_canceled()) {
    heartbeat_timer_->cancel();
  }
}

void Bond::setHeartbeatPeriod(double dur)
{
  if (started_) {
    RCLCPP_ERROR(node_logging_->get_logger(), "Cannot set timeouts after calling start()");
    return;
  }

  heartbeat_period_ = rclcpp::Duration::from_seconds(dur);
}

void Bond::publishingTimerReset()
{
  //  Callback function of publishing timer
  auto publishingTimerResetCallback =
    [this]() -> void
    {
      doPublishing();
    };
  //  publishing timer started on node
  publishing_timer_ = rclcpp::create_wall_timer(
    heartbeat_period_.to_chrono<std::chrono::nanoseconds>(),
    publishingTimerResetCallback,
    nullptr,
    node_base_.get(),
    node_timers_.get());
}

void Bond::publishingTimerCancel()
{
  if (publishing_timer_ && !publishing_timer_->is_canceled()) {
    publishing_timer_->cancel();
  }
}

void Bond::setDeadPublishPeriod(double dur)
{
  if (started_) {
    RCLCPP_ERROR(node_logging_->get_logger(), "Cannot set timeouts after calling start()");
    return;
  }

  dead_publish_period_ = rclcpp::Duration::from_seconds(dur);
}

void Bond::deadpublishingTimerReset()
{
  //  callback function of dead publishing timer which will publish data when bond is broken
  auto deadpublishingTimerResetCallback =
    [this]() -> void
    {
      doPublishing();
    };

  //  dead publishing timer started on node
  deadpublishing_timer_ = rclcpp::create_wall_timer(
    dead_publish_period_.to_chrono<std::chrono::nanoseconds>(),
    deadpublishingTimerResetCallback,
    nullptr,
    node_base_.get(),
    node_timers_.get());
}

void Bond::deadpublishingTimerCancel()
{
  if (deadpublishing_timer_ && !deadpublishing_timer_->is_canceled()) {
    deadpublishing_timer_->cancel();
  }
}

void Bond::start()
{
  // create_subscription must be done outside of constructor
  // to allow usage of shared_from_this()

  // TBD: Should recreation of subscription be prevented?
  if (!started_) {
    sub_ = rclcpp::create_subscription<bond::msg::Status>(
      node_params_,
      node_topics_,
      topic_,
      rclcpp::QoS(100),
      createSafeSubscriptionMemFuncCallback(
        shared_from_this(),
        &Bond::bondStatusCB)
    );
  } else {
    RCLCPP_WARN(
      node_logging_->get_logger(),
      "start() already started skipping subscription recreation");
  }

  connect_timer_reset_flag_ = true;
  connectTimerReset();
  publishingTimerReset();
  disconnectTimerReset();
  started_ = true;
}

void Bond::setFormedCallback(EventCallback on_formed)
{
  std::unique_lock<std::mutex> lock(callbacks_mutex_);
  on_formed_ = on_formed;
}

void Bond::setBrokenCallback(EventCallback on_broken)
{
  std::unique_lock<std::mutex> lock(callbacks_mutex_);
  on_broken_ = on_broken;
}

bool Bond::waitUntilFormed(rclcpp::Duration timeout)
{
  rclcpp::Clock steady_clock(RCL_STEADY_TIME);
  rclcpp::Time deadline(steady_clock.now() + timeout);
  rclcpp::Rate r(100);

  bool formed = false;
  while (!formed) {
    if (!rclcpp::ok()) {
      break;
    }
    rclcpp::Duration wait_time = rclcpp::Duration(100ms);
    if (timeout >= rclcpp::Duration(0.0s)) {
      wait_time = std::min(wait_time, deadline - steady_clock.now());
    }
    if (wait_time <= rclcpp::Duration(0.0s)) {
      break;  //  The deadline has expired
    }
    r.sleep();

    if (!isStateWaitingForSister()) {
      formed = true;
    }
  }

  return formed;
}

bool Bond::waitUntilBroken(rclcpp::Duration timeout)
{
  rclcpp::Clock steady_clock(RCL_STEADY_TIME);
  rclcpp::Time deadline(steady_clock.now() + timeout);
  rclcpp::Rate r(100);

  bool broken = false;
  while (!broken) {
    if (!rclcpp::ok()) {
      break;
    }
    rclcpp::Duration wait_time = rclcpp::Duration(100ms);
    if (timeout >= rclcpp::Duration(0.0s)) {
      wait_time = std::min(wait_time, deadline - steady_clock.now());
    }
    if (wait_time <= rclcpp::Duration(0.0s)) {
      break;  //  The deadline has expired
    }
    r.sleep();

    if (isStateDead()) {
      broken = true;
    }
  }

  return broken;
}

bool Bond::isBroken()
{
  return isStateDead();
}

bool Bond::isStateAlive()
{
  std::unique_lock<std::mutex> lock(state_machine_mutex_);
  return sm_.getState().getId() == SM::Alive.getId();
}

bool Bond::isStateAwaitSisterDeath()
{
  std::unique_lock<std::mutex> lock(state_machine_mutex_);
  return sm_.getState().getId() == SM::AwaitSisterDeath.getId();
}

bool Bond::isStateDead()
{
  std::unique_lock<std::mutex> lock(state_machine_mutex_);
  return sm_.getState().getId() == SM::Dead.getId();
}

bool Bond::isStateWaitingForSister()
{
  std::unique_lock<std::mutex> lock(state_machine_mutex_);
  return sm_.getState().getId() == SM::WaitingForSister.getId();
}

void Bond::breakBond()
{
  if (!isStateDead()) {
    {
      std::unique_lock<std::mutex> lock(state_machine_mutex_);
      sm_.Die();
    }
    publishStatus(false);
  }
  flushPendingCallbacks();
}

void Bond::onConnectTimeout()
{
  {
    std::unique_lock<std::mutex> lock(state_machine_mutex_);
    sm_.ConnectTimeout();
  }
  flushPendingCallbacks();
}

void Bond::onHeartbeatTimeout()
{
  {
    std::unique_lock<std::mutex> lock(state_machine_mutex_);
    sm_.HeartbeatTimeout();
  }
  flushPendingCallbacks();
}

void Bond::onDisconnectTimeout()
{
  {
    std::unique_lock<std::mutex> lock(state_machine_mutex_);
    sm_.DisconnectTimeout();
  }
  flushPendingCallbacks();
}

void Bond::bondStatusCB(const bond::msg::Status & msg)
{
  if (!started_) {
    return;
  }

  //  Filters out messages from other bonds and messages from ourself
  if (msg.id == id_ && msg.instance_id != instance_id_) {
    if (sister_instance_id_.empty()) {
      sister_instance_id_ = msg.instance_id;
    }

    if (msg.active) {
      std::unique_lock<std::mutex> lock(state_machine_mutex_);
      sm_.SisterAlive();
    } else {
      {
        std::unique_lock<std::mutex> lock(state_machine_mutex_);
        sm_.SisterDead();
      }
      //  Immediate ack for sister's death notification
      if (sisterDiedFirst_) {
        publishStatus(false);
      }
    }
  }
  flushPendingCallbacks();
}

void Bond::doPublishing()
{
  if (isStateWaitingForSister() || isStateAlive()) {
    publishStatus(true);
  } else if (isStateAwaitSisterDeath()) {
    publishStatus(false);
  } else {  // SM::Dead
    publishingTimerCancel();
    deadpublishingTimerCancel();
  }
}

void Bond::publishStatus(bool active)
{
  auto msg = std::make_unique<bond::msg::Status>();
  rclcpp::Clock steady_clock(RCL_STEADY_TIME);
  rclcpp::Time now = steady_clock.now();
  msg->header.stamp = now;
  msg->id = id_;
  msg->instance_id = instance_id_;
  msg->active = active;
  msg->heartbeat_timeout = static_cast<float>(heartbeat_timeout_.seconds());
  msg->heartbeat_period = static_cast<float>(heartbeat_period_.seconds());
  if (pub_->get_subscription_count() < 1) {return;}
  pub_->publish(std::move(msg));
}

void Bond::flushPendingCallbacks()
{
  std::vector<EventCallback> callbacks;
  {
    // Check the availability of the mutex before locking
    // This is a temporary fix before https://github.com/ros/bond_core/pull/108 is merged
    if (callbacks_mutex_.try_lock()) {
      callbacks = pending_callbacks_;
      pending_callbacks_.clear();
      callbacks_mutex_.unlock();
    } else {
      return;
    }
  }

  for (size_t i = 0; i < callbacks.size(); ++i) {
    callbacks[i]();
  }
}

}  //  namespace bond


void BondSM::Connected()
{
  b->connectTimerCancel();
  if (b->on_formed_) {
    std::unique_lock<std::mutex> lock(b->callbacks_mutex_);
    b->pending_callbacks_.push_back(b->on_formed_);
  }
}

void BondSM::SisterDied()
{
  b->sisterDiedFirst_ = true;
}

void BondSM::Death()
{
  b->heartbeatTimerCancel();
  b->disconnectTimerCancel();
  if (b->on_broken_) {
    std::unique_lock<std::mutex> lock(b->callbacks_mutex_);
    b->pending_callbacks_.push_back(b->on_broken_);
  }
}

void BondSM::Heartbeat()
{
  b->heartbeatTimerReset();
}

void BondSM::StartDying()
{
  b->heartbeatTimerCancel();
  b->disconnect_timer_reset_flag_ = true;
  b->disconnect_timer_.reset();
  b->deadpublishingTimerReset();
}
