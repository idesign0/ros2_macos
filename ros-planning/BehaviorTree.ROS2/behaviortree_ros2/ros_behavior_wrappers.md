# ROS Behavior Wrappers

A base class is used to implement each Behavior type for a ROS Action, Service or Topic Publisher / Subscriber.

Users are expected to create a derived class and can implement the following methods.

# ROS Action Behavior Wrapper

### bool setGoal(Goal& goal)

Required callback that allows the user to set the goal message.
Return false if the request should not be sent. In that case, RosActionNode::onFailure(INVALID_GOAL) will be called.

### BT::NodeStatus onResultReceived(const WrappedResult& result)

Required callback invoked when the result is received by the server.
It is up to the user to define if the action returns SUCCESS or FAILURE.

### BT::NodeStatus onFeedback(const std::shared_ptr<const Feedback> feedback)

Optional callback invoked when the feedback is received.
It generally returns RUNNING, but the user can also use this callback to cancel the current action and return SUCCESS or FAILURE.

### BT::NodeStatus onFailure(ActionNodeErrorCode error)

Optional callback invoked when something goes wrong.
It must return either SUCCESS or FAILURE.

### void onHalt()

Optional callback executed when the node is halted.
Note that cancelGoal() is done automatically.

# ROS Service Behavior Wrapper

### setRequest(typename Request::SharedPtr& request)

Required callback that allows the user to set the request message (ServiceT::Request).

### BT::NodeStatus onResponseReceived(const typename Response::SharedPtr& response)

Required callback invoked when the response is received by the server.
It is up to the user to define if this returns SUCCESS or FAILURE.

### BT::NodeStatus onFailure(ServiceNodeErrorCode error)

Optional callback invoked when something goes wrong; you can override it.
It must return either SUCCESS or FAILURE.

# ROS Topic Publisher Wrapper

### bool setMessage(TopicT& msg)

Required callback invoked in tick to allow the user to pass the message to be published.

# ROS Topic Subscriber Wrapper

### NodeStatus onTick(const std::shared_ptr<TopicT>& last_msg)

Required callback invoked in the tick. You must return either SUCCESS of FAILURE.

### bool latchLastMessage()

Optional callback to latch the message that has been processed.
If returns false and no new message is received, before next call there will be no message to process.
If returns true, the next call will process the same message again, if no new message received.
