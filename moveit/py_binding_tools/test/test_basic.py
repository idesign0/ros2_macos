import pytest
import sys
import time

import rclpy
import rclcpp
from py_binding_tools_test import PubAndSub, inc, incTime

from std_msgs.msg import Int32, Bool


@pytest.fixture
def ros_fixture():
    rclcpp.init()
    rclpy.init()
    try:
        yield
    finally:
        rclpy.shutdown()
        rclcpp.shutdown()


@pytest.fixture
def node_py(ros_fixture):
    return rclpy.create_node("node_py")


@pytest.fixture
def node_cpp(ros_fixture):
    return rclcpp.Node("node_cpp", rclcpp.NodeOptions(use_global_arguments=False))


@pytest.fixture
def pub_and_sub(node_cpp):
    return PubAndSub(node_cpp)


def test_multiple_init():
    rclcpp.init()
    rclcpp.init()
    rclcpp.shutdown()
    rclcpp.shutdown()

    rclcpp.shutdown()  # prints an error


def test_cpp_to_python(node_py, pub_and_sub):
    value = 0

    def on_sub(message):
        nonlocal value
        value = message.data

    node_py.create_subscription(Int32, "/test", on_sub, 1)
    time.sleep(0.1)  # without that, published message is simply lost
    pub_and_sub.publish(42)
    rclpy.spin_once(node_py, timeout_sec=0.1)
    assert value == 42


def test_python_to_cpp(node_py, pub_and_sub):
    pub_py = node_py.create_publisher(Int32, "/test", 1)
    time.sleep(0.1)  # without that, published message is simply lost
    pub_py.publish(Int32(data=4711))
    time.sleep(0.1)  # some time to receive the message
    assert pub_and_sub.value == 4711


def test_msg_roundtrip():
    result = inc(Int32(data=42))
    assert result.data == 43


def test_msg_conversion_failure():
    with pytest.raises(TypeError):
        inc(Bool(data=False))
    with pytest.raises(TypeError):
        inc(42)


def test_time_conversion():
    result = incTime(rclpy.time.Time(seconds=1, nanoseconds=42))
    assert result.nanoseconds == 1000000043


if __name__ == "__main__":
    sys.exit(pytest.main(sys.argv))
