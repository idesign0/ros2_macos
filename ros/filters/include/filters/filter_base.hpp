/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FILTERS__FILTER_BASE_HPP_
#define FILTERS__FILTER_BASE_HPP_

#include <string>
#include <typeinfo>
#include <vector>

#include "rcl_interfaces/msg/parameter_descriptor.hpp"
#include "rcl_interfaces/msg/parameter_type.hpp"
#include "rclcpp/rclcpp.hpp"

namespace filters
{

namespace impl
{

inline std::string normalize_param_prefix(std::string prefix)
{
  if (!prefix.empty()) {
    if ('.' != prefix.back()) {
      prefix += '.';
    }
  }
  return prefix;
}

}  // namespace impl

/**
 * \brief A Base filter class to provide a standard interface for all filters
 */
template<typename T>
class FilterBase
{
public:
  /**
   * \brief Default constructor used by Filter Factories
   */
  FilterBase()
  : configured_(false) {}

  /**
   * \brief Virtual Destructor
   */
  virtual ~FilterBase() = default;

  /**
   * \brief Configure the filter from the parameter server
   * \param The parameter from which to read the configuration
   * \param node_handle The optional node handle, useful if operating in a different namespace.
   */
  bool configure(
    const std::string & param_prefix,
    const std::string & filter_name,
    const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logger,
    const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr & node_params)
  {
    if (configured_) {
      RCLCPP_WARN(
        node_logger->get_logger(),
        "Filter %s already being reconfigured",
        filter_name_.c_str());
    }
    if (!node_params) {
      throw std::runtime_error("Need a parameters interface to function.");
    }

    configured_ = false;

    filter_name_ = filter_name;
    param_prefix_ = impl::normalize_param_prefix(param_prefix);
    params_interface_ = node_params;
    logging_interface_ = node_logger;

    configured_ = configure();
    return configured_;
  }

  /**
   * \brief Update the filter and return the data seperately
   * This is an inefficient way to do this and can be overridden in the derived class
   * \param data_in A reference to the data to be input to the filter
   * \param data_out A reference to the data output location
   */
  virtual bool update(const T & data_in, T & data_out) = 0;

  /**
 * \brief reconfigureCB
 * can be overridden in the derived class
 * \param parameters A vector parameters to be reconfigured
 */
  virtual rcl_interfaces::msg::SetParametersResult reconfigureCB(
    std::vector<rclcpp::Parameter> parameters)
  {
    // Avoid unused parameter warning
    (void)parameters;
    auto result = rcl_interfaces::msg::SetParametersResult();
    result.successful = true;
    return result;
  }

  /**
   * \brief Get the name of the filter as a string
   */
  inline const std::string & getName() {return filter_name_;}

  /**
 * \brief Get the parameter_prefix of the filter as a string
 */
  inline const std::string & getParamPrefix() {return param_prefix_;}

private:
  template<typename PT>
  bool getParamImpl(
    const std::string & name, const uint8_t type, PT default_value, PT & value_out,
    bool read_only)
  {
    std::string param_name = param_prefix_ + name;

    if (!params_interface_->has_parameter(param_name)) {
      // Declare parameter
      rclcpp::ParameterValue default_parameter_value(default_value);
      rcl_interfaces::msg::ParameterDescriptor desc;
      desc.name = name;
      desc.type = type;
      desc.read_only = read_only;

      if (name.empty()) {
        throw std::runtime_error("Parameter must have a name");
      }

      params_interface_->declare_parameter(param_name, default_parameter_value, desc);
    }

    value_out = params_interface_->get_parameter(param_name).get_parameter_value().get<PT>();
    // TODO(sloretz) seems to be no way to tell if parameter was initialized or not
    return true;
  }

protected:
  /**
   * \brief Pure virtual function for the sub class to configure the filter
   * This function must be implemented in the derived class.
   */
  virtual bool configure() = 0;

  /**
   * \brief Get a filter parameter as a string
   * \param name The name of the parameter
   * \param value The string to set with the value
   * \param default_value The default value to use if the parameter is not set
   * \return Whether or not the parameter of name/type was set */
  bool getParam(
    const std::string & name, std::string & value, bool read_only = true,
    std::string default_value = std::string())
  {
    return getParamImpl(
      name, rcl_interfaces::msg::ParameterType::PARAMETER_STRING, default_value, value, read_only);
  }

  /**
   * \brief Get a filter parameter as a boolean
   * \param name The name of the parameter
   * \param value The boolean to set with the value
   * \param default_value The default value to use if the parameter is not set
   * \return Whether or not the parameter of name/type was set */
  bool getParam(
    const std::string & name, bool & value,
    bool read_only = true, bool default_value = false)
  {
    return getParamImpl(
      name, rcl_interfaces::msg::ParameterType::PARAMETER_BOOL, default_value,
      value, read_only);
  }

  /**
   * \brief Get a filter parameter as a double
   * \param name The name of the parameter
   * \param value The double to set with the value
   * \param default_value The default value to use if the parameter is not set
   * \return Whether or not the parameter of name/type was set */
  bool getParam(
    const std::string & name, double & value,
    bool read_only = true, double default_value = 0.0)
  {
    return getParamImpl(
      name, rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE, default_value,
      value, read_only);
  }

  /**
   * \brief Get a filter parameter as a int
   * \param name The name of the parameter
   * \param value The int to set with the value
   * \param default_value The default value to use if the parameter is not set
   * \return Whether or not the parameter of name/type was set */
  bool getParam(
    const std::string & name, int & value,
    bool read_only = true, int default_value = 0)
  {
    return getParamImpl(
      name, rcl_interfaces::msg::ParameterType::PARAMETER_INTEGER, default_value,
      value, read_only);
  }

  /**
   * \brief Get a filter parameter as an unsigned int
   * \param name The name of the parameter
   * \param value The int to set with the value
   * \return Whether or not the parameter of name/type was set */
  bool getParam(const std::string & name, unsigned int & value)
  {
    int signed_value;
    if (!getParam(name, signed_value)) {
      return false;
    }
    if (signed_value < 0) {
      return false;
    }
    value = signed_value;
    return true;
  }

  /**
   * \brief Get a filter parameter as a size_t
   * \param name The name of the parameter
   * \param value The int to set with the value
   * \return Whether or not the parameter of name/type was set */
  bool getParam(const std::string & name, size_t & value)
  {
    int signed_value;
    if (!getParam(name, signed_value)) {
      return false;
    }
    if (signed_value < 0) {
      return false;
    }
    value = signed_value;
    return true;
  }

  /**
   * \brief Get a filter parameter as a std::vector<double>
   * \param name The name of the parameter
   * \param value The std::vector<double> to set with the value
   * \param default_value The default value to use if the parameter is not set
   * \return Whether or not the parameter of name/type was set */
  bool getParam(
    const std::string & name, std::vector<double> & value,
    bool read_only = true,
    std::vector<double> default_value = {})
  {
    return getParamImpl(
      name, rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE_ARRAY, default_value, value,
      read_only);
  }

  /**
   * \brief Get a filter parameter as a std::vector<string>
   * \param name The name of the parameter
   * \param value The std::vector<sgring> to set with the value
   * \param default_value The default value to use if the parameter is not set
   * \return Whether or not the parameter of name/type was set */
  bool getParam(
    const std::string & name, std::vector<std::string> & value,
    bool read_only = true,
    std::vector<std::string> default_value = {})
  {
    return getParamImpl(
      name, rcl_interfaces::msg::ParameterType::PARAMETER_STRING_ARRAY, default_value, value,
      read_only);
  }

  /// The name of the filter
  std::string filter_name_;
  /// Whether the filter has been configured.
  bool configured_;

  std::string param_prefix_;

  rclcpp::node_interfaces::NodeParametersInterface::SharedPtr params_interface_;
  rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr logging_interface_;
};


template<typename T>
class MultiChannelFilterBase : public FilterBase<T>
{
public:
  MultiChannelFilterBase()
  : number_of_channels_(0)
  {
  }

  /**
   * \brief Virtual Destructor
   */
  virtual ~MultiChannelFilterBase() = default;

  virtual bool configure()
  {
    return true;
  }

  /**
   * \brief Configure the filter from the parameter server
   * \param number_of_channels How many parallel channels the filter will process
   * \param The parameter from which to read the configuration
   * \param node_handle The optional node handle, useful if operating in a different namespace.
   */
  bool configure(
    size_t number_of_channels,
    const std::string & param_prefix,
    const std::string & filter_name,
    const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logger,
    const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr & node_params)
  {
    number_of_channels_ = number_of_channels;

    return FilterBase<T>::configure(param_prefix, filter_name, node_logger, node_params);
  }

  /**
   * \brief Update the filter and return the data seperately
   * \param data_in A reference to the data to be input to the filter
   * \param data_out A reference to the data output location
   * This funciton must be implemented in the derived class.
   */
  virtual bool update(const std::vector<T> & data_in, std::vector<T> & data_out) = 0;

  virtual bool update(const T & /*data_in*/, T & /*data_out*/)
  {
    RCLCPP_ERROR(
      this->logging_interface_->get_logger(),
      "THIS IS A MULTI FILTER DON'T CALL SINGLE FORM OF UPDATE");
    return false;
  }

protected:
  /// How many parallel inputs for which the filter is to be configured
  size_t number_of_channels_;
};

}  // namespace filters

#endif  // FILTERS__FILTER_BASE_HPP_
