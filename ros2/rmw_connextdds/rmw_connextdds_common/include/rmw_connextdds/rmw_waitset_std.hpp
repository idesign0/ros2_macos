// Copyright 2020 Real-Time Innovations, Inc. (RTI)
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

#ifndef RMW_CONNEXTDDS__RMW_WAITSET_STD_HPP_
#define RMW_CONNEXTDDS__RMW_WAITSET_STD_HPP_

#include <condition_variable>
#include "rmw_connextdds/context.hpp"

/******************************************************************************
 * Alternative implementation of WaitSets and Conditions using C++ std
 ******************************************************************************/
class RMW_Connext_WaitSet
{
public:
  RMW_Connext_WaitSet() {}

  rmw_ret_t
  wait(
    rmw_subscriptions_t * const subs,
    rmw_guard_conditions_t * const gcs,
    rmw_services_t * const srvs,
    rmw_clients_t * const cls,
    rmw_events_t * const evs,
    const rmw_time_t * const wait_timeout);

protected:
  void
  attach(
    rmw_subscriptions_t * const subs,
    rmw_guard_conditions_t * const gcs,
    rmw_services_t * const srvs,
    rmw_clients_t * const cls,
    rmw_events_t * const evs,
    bool & already_active);

  void
  detach(
    rmw_subscriptions_t * const subs,
    rmw_guard_conditions_t * const gcs,
    rmw_services_t * const srvs,
    rmw_clients_t * const cls,
    rmw_events_t * const evs,
    size_t & active_conditions);

  bool
  on_condition_active(
    rmw_subscriptions_t * const subs,
    rmw_guard_conditions_t * const gcs,
    rmw_services_t * const srvs,
    rmw_clients_t * const cls,
    rmw_events_t * const evs);

  bool waiting{false};
  std::mutex mutex_internal;
  std::condition_variable condition;
};

class RMW_Connext_Condition
{
public:
  RMW_Connext_Condition()
  : mutex_internal(),
    waitset_mutex(nullptr),
    waitset_condition(nullptr)
  {}

  template<typename FunctorT>
  void
  attach(
    std::mutex * const waitset_mutex,
    std::condition_variable * const waitset_condition,
    bool & already_active,
    FunctorT && check_trigger)
  {
    std::lock_guard<std::mutex> lock(this->mutex_internal);
    already_active = check_trigger();
    if (!already_active) {
      this->waitset_mutex = waitset_mutex;
      this->waitset_condition = waitset_condition;
    }
  }

  template<typename FunctorT>
  void
  detach(FunctorT && on_detached)
  {
    std::lock_guard<std::mutex> lock(this->mutex_internal);
    this->waitset_mutex = nullptr;
    this->waitset_condition = nullptr;
    on_detached();
  }

  virtual bool owns(DDS_Condition * const cond) = 0;

  template<typename FunctorT>
  void
  update_state(FunctorT && update_condition, const bool notify)
  {
    std::lock_guard<std::mutex> internal_lock(this->mutex_internal);

    if (nullptr != this->waitset_mutex) {
      std::lock_guard<std::mutex> lock(*this->waitset_mutex);
      update_condition();
    } else {
      update_condition();
    }

    if (notify && nullptr != this->waitset_condition) {
      this->waitset_condition->notify_one();
    }
  }

  template<typename FunctorT, typename FunctorA>
  void
  perform_action_and_update_state(FunctorA && action, FunctorT && update_condition)
  {
    std::lock_guard<std::mutex> internal_lock(this->mutex_internal);

    action();

    if (nullptr != this->waitset_mutex) {
      std::lock_guard<std::mutex> lock(*this->waitset_mutex);
      update_condition();
    } else {
      update_condition();
    }
  }

protected:
  std::mutex mutex_internal;
  std::mutex * waitset_mutex;
  std::condition_variable * waitset_condition;

  static rmw_ret_t
  _attach(
    DDS_WaitSet * const waitset,
    DDS_Condition * const dds_condition)
  {
    if (DDS_RETCODE_OK != DDS_WaitSet_attach_condition(waitset, dds_condition)) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to attach condition to waitset")
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  static rmw_ret_t
  _detach(
    DDS_WaitSet * const waitset,
    DDS_Condition * const dds_condition)
  {
    // detach_condition() returns BAD_PARAMETER if the condition is not attached
    DDS_ReturnCode_t rc = DDS_WaitSet_detach_condition(waitset, dds_condition);
    if (DDS_RETCODE_OK != rc &&
      DDS_RETCODE_BAD_PARAMETER != rc &&
      DDS_RETCODE_PRECONDITION_NOT_MET != rc)
    {
      RMW_CONNEXT_LOG_ERROR_A_SET(
        "failed to detach condition from waitset: %d", rc)
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  friend class RMW_Connext_WaitSet;
  friend class RMW_Connext_Event;
};

class RMW_Connext_GuardCondition : public RMW_Connext_Condition
{
public:
  explicit RMW_Connext_GuardCondition(const bool internal)
  : trigger_value(false),
    internal(internal),
    gcond(nullptr)
  {
    if (this->internal) {
      this->gcond = DDS_GuardCondition_new();
      if (nullptr == this->gcond) {
        RMW_CONNEXT_LOG_ERROR_SET("failed to allocate dds guard condition")
      }
    }
  }

  virtual ~RMW_Connext_GuardCondition()
  {
    if (nullptr != this->gcond) {
      DDS_GuardCondition_delete(this->gcond);
    }
  }

  DDS_GuardCondition *
  guard_condition() const
  {
    return this->gcond;
  }

  rmw_ret_t
  trigger()
  {
    if (internal) {
      if (DDS_RETCODE_OK !=
        DDS_GuardCondition_set_trigger_value(
          this->gcond, DDS_BOOLEAN_TRUE))
      {
        RMW_CONNEXT_LOG_ERROR_SET("failed to trigger internal guard condition")
        return RMW_RET_ERROR;
      }

      return RMW_RET_OK;
    }

    update_state(
      [this]() {
        this->trigger_value = true;
      }, true /* notify */);

    return RMW_RET_OK;
  }

  virtual rmw_ret_t _attach(DDS_WaitSet * const waitset)
  {
    return RMW_Connext_Condition::_attach(
      waitset, DDS_GuardCondition_as_condition(this->gcond));
  }
  virtual rmw_ret_t _detach(DDS_WaitSet * const waitset)
  {
    return RMW_Connext_Condition::_detach(
      waitset, DDS_GuardCondition_as_condition(this->gcond));
  }

  virtual bool
  owns(DDS_Condition * const cond)
  {
    return cond == DDS_GuardCondition_as_condition(this->gcond);
  }

protected:
  bool trigger_value;
  bool internal;
  DDS_GuardCondition * gcond;

  friend class RMW_Connext_WaitSet;
};

class RMW_Connext_StatusCondition : public RMW_Connext_Condition
{
public:
  explicit RMW_Connext_StatusCondition(
    DDS_Entity * const entity)
  : scond(DDS_Entity_get_statuscondition(entity))
  {
    this->scond = DDS_Entity_get_statuscondition(entity);
    if (nullptr == this->scond) {
      RMW_CONNEXT_LOG_ERROR_SET("failed to get DDS entity's condition")
      throw new std::runtime_error("failed to get DDS entity's condition");
    }
  }

  rmw_ret_t
  reset_statuses()
  {
    if (DDS_RETCODE_OK !=
      DDS_StatusCondition_set_enabled_statuses(
        this->scond, DDS_STATUS_MASK_NONE))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to reset status condition's statuses")
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  rmw_ret_t
  enable_statuses(const DDS_StatusMask statuses)
  {
    DDS_StatusMask current_statuses =
      DDS_StatusCondition_get_enabled_statuses(this->scond);
    current_statuses |= statuses;
    if (DDS_RETCODE_OK !=
      DDS_StatusCondition_set_enabled_statuses(this->scond, current_statuses))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to enable status condition's statuses")
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  rmw_ret_t
  disable_statuses(const DDS_StatusMask statuses)
  {
    DDS_StatusMask current_statuses =
      DDS_StatusCondition_get_enabled_statuses(this->scond);
    current_statuses &= ~statuses;
    if (DDS_RETCODE_OK !=
      DDS_StatusCondition_set_enabled_statuses(this->scond, current_statuses))
    {
      RMW_CONNEXT_LOG_ERROR_SET("failed to disable status condition's statuses")
      return RMW_RET_ERROR;
    }
    return RMW_RET_OK;
  }

  virtual rmw_ret_t _attach(DDS_WaitSet * const waitset)
  {
    return RMW_Connext_Condition::_attach(
      waitset, DDS_StatusCondition_as_condition(this->scond));
  }
  virtual rmw_ret_t _detach(DDS_WaitSet * const waitset)
  {
    return RMW_Connext_Condition::_detach(
      waitset, DDS_StatusCondition_as_condition(this->scond));
  }

  virtual bool
  owns(DDS_Condition * const cond)
  {
    return cond == DDS_StatusCondition_as_condition(this->scond);
  }

  DDS_Condition *
  dds_condition() const
  {
    return DDS_StatusCondition_as_condition(this->scond);
  }

  virtual rmw_ret_t
  get_status(const rmw_event_type_t event_type, void * const event_info) = 0;

  // No-op to match API in DDS-based implementation.
  inline void
  invalidate() {}

  inline rmw_ret_t
  attach_data()
  {
    return RMW_RET_OK;
  }

  virtual bool
  has_status(const rmw_event_type_t event_type) = 0;

  void
  notify_new_event(rmw_event_type_t event_type)
  {
    std::unique_lock<std::mutex> lock_mutex(new_event_mutex_);
    if (new_event_cb_[event_type]) {
      new_event_cb_[event_type](user_data_[event_type], 1);
    } else {
      unread_events_count_[event_type]++;
    }
  }

  void
  set_new_event_callback(
    rmw_event_type_t event_type,
    rmw_event_callback_t callback,
    const void * user_data)
  {
    std::unique_lock<std::mutex> lock_mutex(new_event_mutex_);

    if (callback) {
      // Push events arrived before setting the executor's callback
      if (unread_events_count_[event_type] > 0) {
        callback(user_data, unread_events_count_[event_type]);
        unread_events_count_[event_type] = 0;
      }
      user_data_[event_type] = user_data;
      new_event_cb_[event_type] = callback;
    } else {
      user_data_[event_type] = nullptr;
      new_event_cb_[event_type] = nullptr;
    }
  }

  void
  on_inconsistent_topic(const struct DDS_InconsistentTopicStatus * status);

  void
  update_status_inconsistent_topic(const struct DDS_InconsistentTopicStatus * status);

protected:
  DDS_StatusCondition * scond;
  std::mutex new_event_mutex_;
  rmw_event_callback_t new_event_cb_[RMW_EVENT_INVALID] = {};
  const void * user_data_[RMW_EVENT_INVALID] = {};
  uint64_t unread_events_count_[RMW_EVENT_INVALID] = {0};

  bool triggered_inconsistent_topic{false};

  struct DDS_InconsistentTopicStatus status_inconsistent_topic;
};

void
RMW_Connext_DataWriterListener_offered_deadline_missed(
  void * listener_data,
  DDS_DataWriter * writer,
  const struct DDS_OfferedDeadlineMissedStatus * status);

void
RMW_Connext_DataWriterListener_offered_incompatible_qos(
  void * listener_data,
  DDS_DataWriter * writer,
  const struct DDS_OfferedIncompatibleQosStatus * status);

void
RMW_Connext_DataWriterListener_liveliness_lost(
  void * listener_data,
  DDS_DataWriter * writer,
  const struct DDS_LivelinessLostStatus * status);

class RMW_Connext_PublisherStatusCondition : public RMW_Connext_StatusCondition
{
public:
  explicit RMW_Connext_PublisherStatusCondition(DDS_DataWriter * const writer);

  virtual bool
  has_status(const rmw_event_type_t event_type);

  virtual rmw_ret_t
  get_status(const rmw_event_type_t event_type, void * const event_info);

  friend
  void
  RMW_Connext_DataWriterListener_offered_deadline_missed(
    void * listener_data,
    DDS_DataWriter * writer,
    const struct DDS_OfferedDeadlineMissedStatus * status);

  friend
  void
  RMW_Connext_DataWriterListener_offered_incompatible_qos(
    void * listener_data,
    DDS_DataWriter * writer,
    const struct DDS_OfferedIncompatibleQosStatus * status);

  friend
  void
  RMW_Connext_DataWriterListener_liveliness_lost(
    void * listener_data,
    DDS_DataWriter * writer,
    const struct DDS_LivelinessLostStatus * status);

  rmw_ret_t
  install(RMW_Connext_Publisher * const pub);

  void
  on_offered_deadline_missed(
    const DDS_OfferedDeadlineMissedStatus * const status);

  void
  on_offered_incompatible_qos(
    const DDS_OfferedIncompatibleQosStatus * const status);

  void
  on_liveliness_lost(
    const DDS_LivelinessLostStatus * const status);

  // Helper functions to retrieve status information
  inline rmw_ret_t
  get_liveliness_lost_status(rmw_liveliness_lost_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_liveliness = false;

        status->total_count = this->status_liveliness.total_count;
        status->total_count_change = this->status_liveliness.total_count_change;

        this->status_liveliness.total_count_change = 0;
        this->status_liveliness_last = this->status_liveliness;
      }, false /* notify */);

    return RMW_RET_OK;
  }

  inline rmw_ret_t
  get_offered_deadline_missed_status(
    rmw_offered_deadline_missed_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_deadline = false;

        status->total_count = this->status_deadline.total_count;
        status->total_count_change = this->status_deadline.total_count_change;

        this->status_deadline.total_count_change = 0;
        this->status_deadline_last = this->status_deadline;
      }, false /* notify */);

    return RMW_RET_OK;
  }

  inline rmw_ret_t
  get_offered_qos_incompatible_status(
    rmw_offered_qos_incompatible_event_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_qos = false;

        status->total_count = this->status_qos.total_count;
        status->total_count_change = this->status_qos.total_count_change;
        status->last_policy_kind =
        dds_qos_policy_to_rmw_qos_policy(this->status_qos.last_policy_id);

        this->status_qos.total_count_change = 0;
        this->status_qos_last = this->status_qos;
      }, false /* notify */);

    return RMW_RET_OK;
  }

protected:
  void update_status_deadline(
    const DDS_OfferedDeadlineMissedStatus * const status);

  void update_status_liveliness(
    const DDS_LivelinessLostStatus * const status);

  void update_status_qos(
    const DDS_OfferedIncompatibleQosStatus * const status);

  bool triggered_deadline{false};
  bool triggered_liveliness{false};
  bool triggered_qos{false};

  DDS_OfferedDeadlineMissedStatus status_deadline;
  DDS_OfferedIncompatibleQosStatus status_qos;
  DDS_LivelinessLostStatus status_liveliness;

  DDS_OfferedDeadlineMissedStatus status_deadline_last;
  DDS_OfferedIncompatibleQosStatus status_qos_last;
  DDS_LivelinessLostStatus status_liveliness_last;

  RMW_Connext_Publisher * pub;
};

void
RMW_Connext_DataReaderListener_requested_deadline_missed(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_RequestedDeadlineMissedStatus * status);

void
RMW_Connext_DataReaderListener_requested_incompatible_qos(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_RequestedIncompatibleQosStatus * status);

void
RMW_Connext_DataReaderListener_liveliness_changed(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_LivelinessChangedStatus * status);

void
RMW_Connext_DataReaderListener_sample_lost(
  void * listener_data,
  DDS_DataReader * reader,
  const struct DDS_SampleLostStatus * status);

void
RMW_Connext_DataReaderListener_on_data_available(
  void * listener_data,
  DDS_DataReader * reader);

class RMW_Connext_SubscriberStatusCondition : public RMW_Connext_StatusCondition
{
public:
  RMW_Connext_SubscriberStatusCondition(
    DDS_DataReader * const reader,
    const bool ignore_local,
    const bool internal);

  virtual ~RMW_Connext_SubscriberStatusCondition();

  virtual bool
  has_status(const rmw_event_type_t event_type);

  virtual rmw_ret_t
  get_status(const rmw_event_type_t event_type, void * const event_info);

  friend
  void
  RMW_Connext_DataReaderListener_requested_deadline_missed(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_RequestedDeadlineMissedStatus * status);

  friend
  void
  RMW_Connext_DataReaderListener_requested_incompatible_qos(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_RequestedIncompatibleQosStatus * status);

  friend
  void
  RMW_Connext_DataReaderListener_liveliness_changed(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_LivelinessChangedStatus * status);

  friend
  void
  RMW_Connext_DataReaderListener_sample_lost(
    void * listener_data,
    DDS_DataReader * reader,
    const struct DDS_SampleLostStatus * status);

  friend
  void
  RMW_Connext_DataReaderListener_on_data_available(
    void * listener_data,
    DDS_DataReader * reader);

  rmw_ret_t
  install(RMW_Connext_Subscriber * const sub);

  void
  on_requested_deadline_missed(
    const DDS_RequestedDeadlineMissedStatus * const status);

  void
  on_requested_incompatible_qos(
    const DDS_RequestedIncompatibleQosStatus * const status);

  void
  on_liveliness_changed(const DDS_LivelinessChangedStatus * const status);

  void
  on_sample_lost(const DDS_SampleLostStatus * const status);

  const bool ignore_local;
  const DDS_InstanceHandle_t participant_handle;

  // Methods to match the DDS-based implementation
  rmw_ret_t
  set_data_available(const bool available)
  {
    update_state(
      [this, available]() {
        this->triggered_data = available;
      }, true /* notify */);

    if (nullptr != this->loan_guard_condition) {
      if (DDS_RETCODE_OK !=
        DDS_GuardCondition_set_trigger_value(this->loan_guard_condition, available))
      {
        RMW_CONNEXT_LOG_ERROR_SET("failed to set internal reader condition's trigger")
        return RMW_RET_ERROR;
      }
    }
    return RMW_RET_OK;
  }

  virtual bool
  owns(DDS_Condition * const cond)
  {
    return RMW_Connext_StatusCondition::owns(cond) ||
           (nullptr != this->loan_guard_condition &&
           cond == DDS_GuardCondition_as_condition(this->loan_guard_condition));
  }

  virtual rmw_ret_t _attach(DDS_WaitSet * const waitset)
  {
    rmw_ret_t rc = RMW_Connext_StatusCondition::_attach(waitset);
    if (RMW_RET_OK != rc) {
      return rc;
    }
    if (nullptr != this->loan_guard_condition) {
      return RMW_Connext_Condition::_attach(
        waitset, DDS_GuardCondition_as_condition(this->loan_guard_condition));
    }
    return RMW_RET_OK;
  }
  virtual rmw_ret_t _detach(DDS_WaitSet * const waitset)
  {
    rmw_ret_t rc = RMW_Connext_StatusCondition::_detach(waitset);
    if (RMW_RET_OK != rc) {
      return rc;
    }
    if (nullptr != this->loan_guard_condition) {
      return RMW_Connext_Condition::_detach(
        waitset, DDS_GuardCondition_as_condition(this->loan_guard_condition));
    }
    return RMW_RET_OK;
  }

  // Helper functions to retrieve status information
  inline rmw_ret_t
  get_liveliness_changed_status(rmw_liveliness_changed_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_liveliness = false;

        status->alive_count = this->status_liveliness.alive_count;
        status->alive_count_change = this->status_liveliness.alive_count_change;
        status->not_alive_count = this->status_liveliness.not_alive_count;
        status->not_alive_count_change = this->status_liveliness.not_alive_count_change;

        this->status_liveliness.alive_count_change = 0;
        this->status_liveliness.not_alive_count_change = 0;
        this->status_liveliness_last = this->status_liveliness;
      }, false /* notify */);

    return RMW_RET_OK;
  }

  inline rmw_ret_t
  get_requested_deadline_missed_status(
    rmw_requested_deadline_missed_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_deadline = false;

        status->total_count = this->status_deadline.total_count;
        status->total_count_change = this->status_deadline.total_count_change;

        this->status_deadline.total_count_change = 0;
        this->status_deadline_last = this->status_deadline;
      }, false /* notify */);

    return RMW_RET_OK;
  }


  inline rmw_ret_t
  get_requested_qos_incompatible_status(
    rmw_requested_qos_incompatible_event_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_qos = false;

        status->total_count = this->status_qos.total_count;
        status->total_count_change = this->status_qos.total_count_change;
        status->last_policy_kind =
        dds_qos_policy_to_rmw_qos_policy(this->status_qos.last_policy_id);

        this->status_qos.total_count_change = 0;
        this->status_qos_last = this->status_qos;
      }, false /* notify */);

    return RMW_RET_OK;
  }

  inline rmw_ret_t
  get_message_lost_status(rmw_message_lost_status_t * const status)
  {
    update_state(
      [this, status]() {
        this->triggered_sample_lost = false;

        status->total_count = this->status_sample_lost.total_count;
        status->total_count_change = this->status_sample_lost.total_count_change;

        this->status_sample_lost.total_count_change = 0;
        this->status_sample_lost_last = this->status_sample_lost;
      }, false /* notify */);

    return RMW_RET_OK;
  }

  void set_on_new_data_callback(
    const rmw_event_callback_t callback,
    const void * const user_data)
  {
    std::unique_lock<std::mutex> lock(new_data_event_mutex_);
    if (callback) {
      if (unread_data_events_count_ > 0) {
        callback(user_data, unread_data_events_count_);
        unread_data_events_count_ = 0;
      }
      new_data_event_cb_ = callback;
      data_event_user_data_ = user_data;
    } else {
      new_data_event_cb_ = nullptr;
      data_event_user_data_ = nullptr;
    }
  }

  void notify_new_data();

protected:
  void update_status_deadline(
    const DDS_RequestedDeadlineMissedStatus * const status);

  void update_status_liveliness(
    const DDS_LivelinessChangedStatus * const status);

  void update_status_qos(
    const DDS_RequestedIncompatibleQosStatus * const status);

  void update_status_sample_lost(
    const DDS_SampleLostStatus * const status);

  DDS_GuardCondition * const loan_guard_condition;

  bool triggered_deadline{false};
  bool triggered_liveliness{false};
  bool triggered_qos{false};
  bool triggered_sample_lost{false};
  bool triggered_data{false};

  DDS_RequestedDeadlineMissedStatus status_deadline;
  DDS_RequestedIncompatibleQosStatus status_qos;
  DDS_LivelinessChangedStatus status_liveliness;
  DDS_SampleLostStatus status_sample_lost;

  DDS_RequestedDeadlineMissedStatus status_deadline_last;
  DDS_RequestedIncompatibleQosStatus status_qos_last;
  DDS_LivelinessChangedStatus status_liveliness_last;
  DDS_SampleLostStatus status_sample_lost_last;

  RMW_Connext_Subscriber * sub;

  std::mutex new_data_event_mutex_;
  rmw_event_callback_t new_data_event_cb_{nullptr};
  const void * data_event_user_data_{nullptr};
  uint64_t unread_data_events_count_ = 0;

  friend class RMW_Connext_WaitSet;
};

/******************************************************************************
 * Event support
 ******************************************************************************/
class RMW_Connext_Event
{
public:
  static
  rmw_ret_t
  enable(rmw_event_t * const event);

  static
  rmw_ret_t
  disable(rmw_event_t * const event);

  static
  DDS_Condition *
  condition(const rmw_event_t * const event);

  static
  bool
  writer_event(const rmw_event_t * const event)
  {
    return !ros_event_for_reader(event->event_type);
  }

  static
  bool
  reader_event(const rmw_event_t * const event)
  {
    return ros_event_for_reader(event->event_type);
  }

  static
  RMW_Connext_Publisher *
  publisher(const rmw_event_t * const event)
  {
    return reinterpret_cast<RMW_Connext_Publisher *>(event->data);
  }

  static
  RMW_Connext_Subscriber *
  subscriber(const rmw_event_t * const event)
  {
    return reinterpret_cast<RMW_Connext_Subscriber *>(event->data);
  }
};
#endif  // RMW_CONNEXTDDS__RMW_WAITSET_STD_HPP_
