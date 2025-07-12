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

/** \author Stuart Glaser */

#ifndef BONDCPP__BOND_HPP_
#define BONDCPP__BOND_HPP_

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "bond/msg/constants.hpp"
#include "bond/msg/status.hpp"

#include "bondcpp/BondSM_sm.hpp"
#include "bondcpp/visibility_control.hpp"

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>

namespace bond
{
/** \brief Forms a bond to monitor another process.
 *
 * The bond::Bond class implements a bond, allowing you to monitor
 * another process and be notified when it dies.  In turn, it will be
 * notified when you die.
 */
class Bond : public std::enable_shared_from_this<Bond>
{
public:
  using EventCallback = std::function<void (void)>;

  /** \brief Constructor to delegate common functionality
   *
   * \param topic The topic used to exchange the bond status messages.
   * \param id The ID of the bond, which should match the ID used on
   *           the sister's end.
   * \param node_base base node interface
   * \param node_logging logging node interface
   * \param node_timers timers node interface
   * \param on_broken callback that will be called when the bond is broken.
   * \param on_formed callback that will be called when the bond is formed.
   */
  BONDCPP_PUBLIC
  Bond(
    const std::string & topic, const std::string & id,
    rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base,
    rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr node_logging,
    rclcpp::node_interfaces::NodeParametersInterface::SharedPtr node_params,
    rclcpp::node_interfaces::NodeTimersInterface::SharedPtr node_timers,
    rclcpp::node_interfaces::NodeTopicsInterface::SharedPtr node_topics,
    EventCallback on_broken = EventCallback(),
    EventCallback on_formed = EventCallback());

  /** \brief Constructs a bond, but does not connect
   *
   * \param topic The topic used to exchange the bond status messages.
   * \param id The ID of the bond, which should match the ID used on
   *           the sister's end.
   * \param nh Lifecycle Node shared ptr.
   * \param on_broken callback that will be called when the bond is broken.
   * \param on_formed callback that will be called when the bond is formed.
   */
  BONDCPP_PUBLIC
  Bond(
    const std::string & topic, const std::string & id,
    rclcpp_lifecycle::LifecycleNode::SharedPtr nh,
    EventCallback on_broken = EventCallback(),
    EventCallback on_formed = EventCallback());

  /** \brief Constructs a bond, but does not connect
   *
   * \param topic The topic used to exchange the bond status messages.
   * \param id The ID of the bond, which should match the ID used on
   *           the sister's end.
   * \param nh Node shared ptr.
   * \param on_broken callback that will be called when the bond is broken.
   * \param on_formed callback that will be called when the bond is formed.
   */
  BONDCPP_PUBLIC
  Bond(
    const std::string & topic, const std::string & id,
    rclcpp::Node::SharedPtr nh,
    EventCallback on_broken = EventCallback(),
    EventCallback on_formed = EventCallback());

  /** \brief Destructs the object, breaking the bond if it is still formed.
   */
  BONDCPP_PUBLIC
  ~Bond();

  BONDCPP_PUBLIC
  void setupConnections();

  BONDCPP_PUBLIC
  double getConnectTimeout() const {return connect_timeout_.seconds();}
  BONDCPP_PUBLIC
  void setConnectTimeout(double dur);
  BONDCPP_PUBLIC
  void connectTimerReset();
  BONDCPP_PUBLIC
  void connectTimerCancel();

  BONDCPP_PUBLIC
  double getDisconnectTimeout() const {return disconnect_timeout_.seconds();}
  BONDCPP_PUBLIC
  void setDisconnectTimeout(double dur);
  BONDCPP_PUBLIC
  void disconnectTimerReset();
  BONDCPP_PUBLIC
  void disconnectTimerCancel();

  BONDCPP_PUBLIC
  double getHeartbeatTimeout() const {return heartbeat_timeout_.seconds();}
  BONDCPP_PUBLIC
  void setHeartbeatTimeout(double dur);
  BONDCPP_PUBLIC
  void heartbeatTimerReset();
  BONDCPP_PUBLIC
  void heartbeatTimerCancel();

  BONDCPP_PUBLIC
  double getHeartbeatPeriod() const {return heartbeat_period_.seconds();}
  BONDCPP_PUBLIC
  void setHeartbeatPeriod(double dur);
  BONDCPP_PUBLIC
  void publishingTimerReset();
  BONDCPP_PUBLIC
  void publishingTimerCancel();

  BONDCPP_PUBLIC
  double getDeadPublishPeriod() const {return dead_publish_period_.seconds();}
  BONDCPP_PUBLIC
  void setDeadPublishPeriod(double dur);
  BONDCPP_PUBLIC
  void deadpublishingTimerReset();
  BONDCPP_PUBLIC
  void deadpublishingTimerCancel();

  /** \brief Starts the bond and connects to the sister process.
   */
  BONDCPP_PUBLIC
  void start();

  /** \brief Sets the formed callback.
   */
  BONDCPP_PUBLIC
  void setFormedCallback(EventCallback on_formed);

  /** \brief Sets the broken callback
   */
  BONDCPP_PUBLIC
  void setBrokenCallback(EventCallback on_broken);

  /** \brief Blocks until the bond is formed for at most 'duration'.
   *    Assumes the node to be spinning in the background
   *
   * \param timeout Maximum duration to wait.  If -1 then this call will not timeout.
   * \return true iff the bond has been formed.
   */
  BONDCPP_PUBLIC
  bool waitUntilFormed(rclcpp::Duration timeout = rclcpp::Duration(std::chrono::seconds(-1)));

  /** \brief Blocks until the bond is broken for at most 'duration'.
   *    Assumes the node to be spinning in the background
   *
   * \param timeout Maximum duration to wait.  If -1 then this call will not timeout.
   * \return true iff the bond has been broken, even if it has never been formed.
   */
  BONDCPP_PUBLIC
  bool waitUntilBroken(rclcpp::Duration timeout = rclcpp::Duration(std::chrono::seconds(-1)));

  /** \brief Indicates if the bond is broken.
   */
  BONDCPP_PUBLIC
  bool isBroken();

  /** \brief Breaks the bond, notifying the other process.
   */
  BONDCPP_PUBLIC
  void breakBond();

  BONDCPP_PUBLIC
  std::string getTopic() const {return topic_;}
  BONDCPP_PUBLIC
  std::string getId() const {return id_;}
  BONDCPP_PUBLIC
  std::string getInstanceId() const {return instance_id_;}

private:
  friend struct ::BondSM;

  void onConnectTimeout();
  void onHeartbeatTimeout();
  void onDisconnectTimeout();

  void bondStatusCB(const bond::msg::Status & msg);

  void doPublishing();
  void publishStatus(bool active);

  void flushPendingCallbacks();

  bool isStateAlive();
  bool isStateAwaitSisterDeath();
  bool isStateDead();
  bool isStateWaitingForSister();


  rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base_;
  rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr node_logging_;
  rclcpp::node_interfaces::NodeParametersInterface::SharedPtr node_params_;
  rclcpp::node_interfaces::NodeTimersInterface::SharedPtr node_timers_;
  rclcpp::node_interfaces::NodeTopicsInterface::SharedPtr node_topics_;

  rclcpp::TimerBase::SharedPtr connect_timer_;
  rclcpp::TimerBase::SharedPtr disconnect_timer_;
  rclcpp::TimerBase::SharedPtr heartbeat_timer_;
  rclcpp::TimerBase::SharedPtr publishing_timer_;
  rclcpp::TimerBase::SharedPtr deadpublishing_timer_;

  std::mutex state_machine_mutex_;
  std::unique_ptr<BondSM> bondsm_;
  BondSMContext sm_;

  std::string topic_;
  std::string id_;
  std::string instance_id_;
  std::string sister_instance_id_;

  std::mutex callbacks_mutex_;
  std::vector<EventCallback> pending_callbacks_;
  EventCallback on_broken_;
  EventCallback on_formed_;

  bool sisterDiedFirst_ {false};
  bool started_ {false};
  bool connect_timer_reset_flag_ {false};
  bool disconnect_timer_reset_flag_ {false};
  bool deadpublishing_timer_reset_flag_ {false};
  bool disable_heartbeat_timeout_ {false};

  rclcpp::Duration connect_timeout_;
  rclcpp::Duration disconnect_timeout_;
  rclcpp::Duration heartbeat_timeout_;
  rclcpp::Duration heartbeat_period_;
  rclcpp::Duration dead_publish_period_;

  rclcpp::CallbackGroup::SharedPtr sub_callback_group_;
  rclcpp::Subscription<bond::msg::Status>::SharedPtr sub_;
  rclcpp::Publisher<bond::msg::Status>::SharedPtr pub_;
};
}  // namespace bond

// Internal use only
struct BondSM
{
  explicit BondSM(bond::Bond * b_)
  : b(b_) {}

  void Connected();
  void SisterDied();
  void Death();
  void Heartbeat();
  void StartDying();

private:
  bond::Bond * b;
};

#endif  // BONDCPP__BOND_HPP_
