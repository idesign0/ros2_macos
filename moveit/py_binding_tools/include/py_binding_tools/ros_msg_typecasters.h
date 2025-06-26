/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2021, Robert Haschke
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
 *   * The name of Robert Haschke may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
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

/* Author: Robert Haschke */

#pragma once

#include <pybind11/pybind11.h>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/serialization.hpp>
#include <rclcpp/duration.hpp>

/** Provide pybind11 type converters for ROS types */

namespace py_binding_tools
{
PYBIND11_EXPORT pybind11::object createMessageClass(const std::string& ros_msg_name);
PYBIND11_EXPORT bool convertible(const pybind11::handle& h, const std::string& ros_msg_name);
}  // namespace py_binding_tools

namespace pybind11
{
namespace detail
{
/// Convert rclcpp::Duration to/from float
template <>
struct type_caster<rclcpp::Duration>
{
  // C++ -> Python
  static handle cast(rclcpp::Duration&& src, return_value_policy /* policy */, handle /* parent */)
  {
    return PyFloat_FromDouble(src.seconds());
  }

  // Python -> C++
  bool load(handle src, bool convert)
  {
    if (hasattr(src, "nanoseconds"))
    {
      value = rclcpp::Duration(std::chrono::nanoseconds(src.attr("nanoseconds")().cast<int64_t>()));
    }
    else if (convert)
    {
      value = rclcpp::Duration(std::chrono::duration<double>(src.cast<double>()));
    }
    else
      return false;
    return true;
  }

  PYBIND11_TYPE_CASTER(rclcpp::Duration, _("Duration"));
};

/// Convert rclcpp::Time to/from rclpy::Time
template <>
struct type_caster<rclcpp::Time>
{
  // convert from rclpy::Time to rclcpp::Time
  bool load(handle src, bool /* convert */)
  {
    // Extract values for constructing the rclcpp::Time object
    int64_t nanoseconds = src.attr("nanoseconds").cast<int64_t>();
    int clock_type = src.attr("clock_type").cast<int>();

    // Construct the rclcpp::Time object
    value = rclcpp::Time(nanoseconds, static_cast<rcl_clock_type_t>(clock_type));
    return true;
  }

  // convert from rclcpp::Time to rclpy::Time
  static handle cast(const rclcpp::Time& src, return_value_policy /* policy */, handle /* parent */)
  {
    object Time = module::import("rclpy.time").attr("Time");
    object ClockType = Time().attr("clock_type").attr("__class__");

    return Time(arg("nanoseconds") = src.nanoseconds(),
                arg("clock_type") = ClockType(static_cast<int>(src.get_clock_type())))
        .release();  // release the ownership of the object
  }

  PYBIND11_TYPE_CASTER(rclcpp::Time, _("Time"));
};

/// Base class for type conversion (C++ <-> python) of ROS message types
template <typename T>
struct RosMsgTypeCaster
{
  // C++ -> Python
  static handle cast(const T& src, return_value_policy /* policy */, handle /* parent */)
  {
    // serialize src
    rclcpp::Serialization<T> serializer;
    rclcpp::SerializedMessage serialized_msg;
    serializer.serialize_message(&src, &serialized_msg);
    bytes buf = bytes(reinterpret_cast<const char*>(serialized_msg.get_rcl_serialized_message().buffer),
                      serialized_msg.get_rcl_serialized_message().buffer_length);

    // retrieve message type instance
    object cls = py_binding_tools::createMessageClass(rosidl_generator_traits::name<T>());

    // deserialize into python object
    module rclpy = module::import("rclpy.serialization");
    object msg = rclpy.attr("deserialize_message")(buf, cls);

    return msg.release();
  }

  // Python -> C++
  bool load(handle src, bool /*convert*/)
  {
    // check datatype of src
    if (!py_binding_tools::convertible(src, rosidl_generator_traits::name<T>()))
      return false;

    // serialize src into python buffer
    module rclpy = module::import("rclpy.serialization");
    bytes buf = rclpy.attr("serialize_message")(src);

    // deserialize into C++ object
    rcl_serialized_message_t rcl_serialized_msg = rmw_get_zero_initialized_serialized_message();
    char* serialized_buffer;
    Py_ssize_t length;
    if (PYBIND11_BYTES_AS_STRING_AND_SIZE(buf.ptr(), &serialized_buffer, &length))
      return false;

    rcl_serialized_msg.buffer_capacity = length;
    rcl_serialized_msg.buffer_length = length;
    rcl_serialized_msg.buffer = reinterpret_cast<uint8_t*>(serialized_buffer);
    rmw_ret_t rmw_ret =
        rmw_deserialize(&rcl_serialized_msg, rosidl_typesupport_cpp::get_message_type_support_handle<T>(), &value);
    return (rmw_ret == RMW_RET_OK);
  }

  PYBIND11_TYPE_CASTER(T, _<T>());
};

template <typename T>
struct type_caster<T, enable_if_t<rosidl_generator_traits::is_message<T>::value>> : RosMsgTypeCaster<T>
{
};

}  // namespace detail
}  // namespace pybind11
