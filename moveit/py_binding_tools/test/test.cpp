#include <pybind11/pybind11.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <py_binding_tools/ros_msg_typecasters.h>

namespace py = pybind11;
namespace
{
struct PubAndSub
{
public:
  explicit PubAndSub(rclcpp::Node::SharedPtr node)
  {
    auto callback = [this](const std_msgs::msg::Int32& message) { value = message.data; };
    pub = node->create_publisher<std_msgs::msg::Int32>("/test", 1);
    sub = node->create_subscription<std_msgs::msg::Int32>("/test", 1, callback);
  }

  void publish(int value)
  {
    std_msgs::msg::Int32 message{};
    message.data = value;
    pub->publish(message);
  }

  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pub;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub;
  int value{ 0 };
};

PYBIND11_MODULE(py_binding_tools_test, m)
{
  py::class_<PubAndSub>(m, "PubAndSub")
      .def(py::init<rclcpp::Node::SharedPtr>(), py::arg("node"))
      .def("publish", &PubAndSub::publish, py::arg("value"))
      .def_readonly("value", &PubAndSub::value);

  auto increment = [](std_msgs::msg::Int32 msg) {
    msg.data++;
    return msg;
  };
  m.def("inc", increment, "increment data field of Int32 message");

  m.def("incTime", [](rclcpp::Time t) { return t + rclcpp::Duration(std::chrono::nanoseconds(1)); });
}

}  // namespace
