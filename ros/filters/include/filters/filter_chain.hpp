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

#ifndef FILTERS__FILTER_CHAIN_HPP_
#define FILTERS__FILTER_CHAIN_HPP_

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "pluginlib/class_loader.hpp"
#include "rcl_interfaces/msg/parameter_descriptor.hpp"
#include "rcl_interfaces/msg/parameter_type.hpp"
#include <rclcpp/parameter_value.hpp>
#include "rclcpp/rclcpp.hpp"

#include "filters/filter_base.hpp"

namespace filters
{

namespace impl
{

struct FoundFilter
{
  std::string name;
  std::string type;
  std::string param_prefix;
};

/**
 * \brief Declare a string param of the given name. If it already exists, just get the value.
 */
inline bool auto_declare_string(
  const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logger,
  const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr & node_params,
  const std::string & param_name,
  std::string & param_out)
{
  rcl_interfaces::msg::ParameterDescriptor param_desc;
  param_desc.name = param_name;
  param_desc.type = rcl_interfaces::msg::ParameterType::PARAMETER_STRING;
  param_desc.read_only = true;
  param_desc.dynamic_typing = false;

  rclcpp::ParameterValue param_value;
  try {
    param_value = node_params->declare_parameter(
      param_desc.name, rclcpp::ParameterType::PARAMETER_STRING, param_desc);
  } catch (rclcpp::exceptions::ParameterAlreadyDeclaredException &) {
    rclcpp::Parameter param = node_params->get_parameter(param_name);
    param_value = param.get_parameter_value();
  } catch (rclcpp::exceptions::InvalidParametersException &) {
    RCLCPP_ERROR(
      node_logger->get_logger(), "Tried to declare param with invalid name: %s",
      param_name.c_str());
    return false;
  }

  try {
    param_out = param_value.get<std::string>();
  } catch (rclcpp::exceptions::InvalidParameterTypeException &) {
    RCLCPP_ERROR(
      node_logger->get_logger(), "Parameter %s of invalid type (must be a string)",
      param_name.c_str());
    return false;
  }
  return true;
}

/**
 * \brief Read params and figure out what filters to load
 */
inline bool
load_chain_config(
  const std::string & param_prefix,
  const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logger,
  const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr & node_params,
  std::vector<struct FoundFilter> & found_filters)
{
  // TODO(sloretz) error if someone tries to do filter0
  const std::string norm_param_prefix = impl::normalize_param_prefix(param_prefix);

  const auto & param_overrides = node_params->get_parameter_overrides();

  // Read parameters for filter1..filterN
  for (size_t filter_num = 1; true; ++filter_num) {
    // Parameters in chain are prefixed with 'filterN.'
    const std::string name_of_name_param = norm_param_prefix + "filter" +
      std::to_string(filter_num) + ".name";
    const std::string name_of_type_param = norm_param_prefix + "filter" +
      std::to_string(filter_num) + ".type";
    const std::string name_of_param_prefix_param = norm_param_prefix + "filter" +
      std::to_string(filter_num) + ".params";

    if (param_overrides.find(name_of_name_param) == param_overrides.end()) {
      break;
    }

    std::string filter_name;
    if (!auto_declare_string(node_logger, node_params, name_of_name_param, filter_name)) {
      return false;
    }

    std::string filter_type;
    if (!auto_declare_string(node_logger, node_params, name_of_type_param, filter_type)) {
      return false;
    }

    if (std::find_if(
        found_filters.begin(), found_filters.end(),
        [&](const FoundFilter & f) {return f.name == filter_name;}) != found_filters.end())
    {
      RCLCPP_FATAL(
        node_logger->get_logger(),
        "A filter with the name %s already exists", filter_name.c_str());
      return false;
    }

    // Make sure 'type' is formated as 'package_name/filtername'
    if (1 != std::count(filter_type.cbegin(), filter_type.cend(), '/')) {
      RCLCPP_FATAL(
        node_logger->get_logger(),
        "%s must be of form <package_name>/<filter_name>", filter_type.c_str());
      return false;
    }

    // Seems ok; store it for now; it will be loaded further down.
    found_filters.push_back({filter_name, filter_type, name_of_param_prefix_param});
  }
  return true;
}

}  // namespace impl


/**
 * \brief A class which will construct and sequentially call Filters according to xml
 * This is the primary way in which users are expected to interact with Filters
 */
template<typename T>
class FilterChain
{
public:
  /**
   * \brief Create the filter chain object
   */
  explicit FilterChain(std::string data_type)
  : loader_("filters", "filters::FilterBase<" + data_type + ">"),
    configured_(false)
  {
  }

  ~FilterChain()
  {
    clear();
  }

  /**
   * \brief process data through each of the filters added sequentially
   */
  bool update(const T & data_in, T & data_out)
  {
    bool result;
    size_t list_size = reference_pointers_.size();
    if (list_size == 0) {
      data_out = data_in;
      result = true;
    } else if (list_size == 1) {
      result = reference_pointers_[0]->update(data_in, data_out);
    } else if (list_size == 2) {
      result = reference_pointers_[0]->update(data_in, buffer0_);
      if (result == false) {return false;}  // don't keep processing on failure
      result = result && reference_pointers_[1]->update(buffer0_, data_out);
    } else {
      result = reference_pointers_[0]->update(data_in, buffer0_);  // first copy in
      for (size_t i = 1; i < reference_pointers_.size() - 1 && result; ++i) {
        // all but first and last (never called if size=2)
        if (i % 2 == 1) {
          result = result && reference_pointers_[i]->update(buffer0_, buffer1_);
        } else {
          result = result && reference_pointers_[i]->update(buffer1_, buffer0_);
        }
      }
      if (list_size % 2 == 1) {  // odd number last deposit was in buffer1
        result = result && reference_pointers_.back()->update(buffer1_, data_out);
      } else {
        result = result && reference_pointers_.back()->update(buffer0_, data_out);
      }
    }
    return result;
  }

  /**
   * \brief Clear all filters from this chain
   */
  bool clear()
  {
    configured_ = false;
    reference_pointers_.clear();
    return true;
  }

  /**
   * \brief Configure the filter chain and all filters which have been addedj
   * \param param_prefix The parameter name prefix of the filter chain to load
   * \param node_logger node logging interface to use
   * \param node_params node parameter interface to use
   */
  bool configure(
    const std::string & param_prefix,
    const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logger,
    const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr & node_params)
  {
    if (configured_) {
      RCLCPP_ERROR(logging_interface_->get_logger(), "Filter chain is already configured");
      return false;
    }
    logging_interface_ = node_logger;
    params_interface_ = node_params;
    on_set_parameters_callback_handle_ = params_interface_->add_on_set_parameters_callback(
      std::bind(&FilterChain<T>::reconfigureCB, this, std::placeholders::_1));

    std::vector<struct impl::FoundFilter> found_filters;
    if (!impl::load_chain_config(
        param_prefix, logging_interface_, params_interface_, found_filters))
    {
      // Assume load_chain_config() console logged something sensible
      return false;
    }

    std::vector<pluginlib::UniquePtr<filters::FilterBase<T>>> loaded_filters;
    for (const auto & filter : found_filters) {
      // Try to load the filters that were described by parameters
      pluginlib::UniquePtr<filters::FilterBase<T>> loaded_filter;
      try {
        loaded_filter = loader_.createUniqueInstance(filter.type);
      } catch (const pluginlib::LibraryLoadException & e) {
        RCLCPP_FATAL(
          logging_interface_->get_logger(),
          "Could not load library for %s: %s", filter.type.c_str(), e.what());
        return false;
      } catch (const pluginlib::CreateClassException & e) {
        RCLCPP_FATAL(
          logging_interface_->get_logger(),
          "Could not construct class %s: %s", filter.type.c_str(), e.what());
        return false;
      }

      // Try to configure the filter
      if (!loaded_filter || !loaded_filter->configure(
          filter.param_prefix, filter.name, logging_interface_, params_interface_))
      {
        RCLCPP_FATAL(
          logging_interface_->get_logger(),
          "Could not configure %s of type %s", filter.name.c_str(), filter.type.c_str());
        return false;
      }
      loaded_filters.emplace_back(std::move(loaded_filter));
    }

    // Everything went ok!
    reference_pointers_ = std::move(loaded_filters);
    configured_ = true;
    return true;
  }

  rcl_interfaces::msg::SetParametersResult reconfigureCB(std::vector<rclcpp::Parameter> parameters)
  {
    auto result = rcl_interfaces::msg::SetParametersResult();
    result.successful = true;

    for (auto & ref_ptr : reference_pointers_) {
      std::vector<rclcpp::Parameter> parameters_subset;
      for (auto parameter : parameters) {
        if (parameter.get_name().find(ref_ptr->getParamPrefix()) != std::string::npos) {
          parameters_subset.push_back(parameter);
        }
      }
      if (!parameters_subset.empty() && !ref_ptr->reconfigureCB(parameters_subset).successful) {
        result.successful = false;
      }
    }
    return result;
  }

private:
  pluginlib::ClassLoader<filters::FilterBase<T>> loader_;

  /// A vector of pointers to currently constructed filters
  std::vector<pluginlib::UniquePtr<filters::FilterBase<T>>> reference_pointers_;

  T buffer0_;  ///< A temporary intermediate buffer
  T buffer1_;  ///< A temporary intermediate buffer
  bool configured_;  ///< whether the system is configured

  rclcpp::node_interfaces::NodeParametersInterface::SharedPtr params_interface_;
  rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr logging_interface_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr
    on_set_parameters_callback_handle_;
};

/**
 * \brief A class which will construct and sequentially call Filters according to xml
 *
 * This is the primary way in which users are expected to interact with Filters
 */
template<typename T>
class MultiChannelFilterChain
{
public:
  /**
   * \brief Create the filter chain object
   */
  explicit MultiChannelFilterChain(std::string data_type)
  : loader_("filters", "filters::MultiChannelFilterBase<" + data_type + ">"),
    configured_(false)
  {
  }

  /**
   * \brief process data through each of the filters added sequentially
   */
  bool update(const std::vector<T> & data_in, std::vector<T> & data_out)
  {
    bool result;
    size_t list_size = reference_pointers_.size();

    if (list_size == 0) {
      data_out = data_in;
      result = true;
    } else if (list_size == 1) {
      result = reference_pointers_[0]->update(data_in, data_out);
    } else if (list_size == 2) {
      result = reference_pointers_[0]->update(data_in, buffer0_);
      if (result) {  // don't keep processing on failure
        result = result && reference_pointers_[1]->update(buffer0_, data_out);
      }
    } else {
      result = reference_pointers_[0]->update(data_in, buffer0_);  // first copy in
      for (size_t i = 1; i < reference_pointers_.size() - 1 && result; ++i) {
        // all but first and last (never if size = 2)
        if (i % 2 == 1) {
          result = result && reference_pointers_[i]->update(buffer0_, buffer1_);
        } else {
          result = result && reference_pointers_[i]->update(buffer1_, buffer0_);
        }
      }
      if (list_size % 2 == 1) {  // odd number last deposit was in buffer1
        result = result && reference_pointers_.back()->update(buffer1_, data_out);
      } else {
        result = result && reference_pointers_.back()->update(buffer0_, data_out);
      }
    }
    return result;
  }

  ~MultiChannelFilterChain()
  {
    clear();
  }

  /**
   * \brief Clear all filters from this chain
   */
  bool clear()
  {
    configured_ = false;
    reference_pointers_.clear();
    buffer0_.clear();
    buffer1_.clear();
    return true;
  }

  /**
   * \brief Configure the filter chain
   *
   * This will call configure on all filters which have been added
   * as well as allocate the buffers
   */
  bool configure(
    size_t number_of_channels,
    const std::string & param_prefix,
    const rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr & node_logger,
    const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr & node_params)
  {
    if (configured_) {
      RCLCPP_ERROR(logging_interface_->get_logger(), "Filter chain is already configured");
      return false;
    }
    logging_interface_ = node_logger;
    params_interface_ = node_params;

    std::vector<impl::FoundFilter> found_filters;
    if (!impl::load_chain_config(
        param_prefix, logging_interface_, params_interface_, found_filters))
    {
      // Assume load_chain_config() console logged something sensible
      return false;
    }

    std::vector<pluginlib::UniquePtr<filters::MultiChannelFilterBase<T>>> loaded_filters;
    for (const auto & filter : found_filters) {
      // Try to load the filters that were described by parameters
      pluginlib::UniquePtr<filters::MultiChannelFilterBase<T>> loaded_filter;
      try {
        loaded_filter = loader_.createUniqueInstance(filter.type);
      } catch (const pluginlib::LibraryLoadException & e) {
        RCLCPP_FATAL(
          logging_interface_->get_logger(),
          "Could not load library for %s: %s", filter.type.c_str(), e.what());
        return false;
      } catch (const pluginlib::CreateClassException & e) {
        RCLCPP_FATAL(
          logging_interface_->get_logger(),
          "Could not construct class %s: %s", filter.type.c_str(), e.what());
        return false;
      }

      // Try to configure the filter
      if (!loaded_filter || !loaded_filter->configure(
          number_of_channels, filter.param_prefix, filter.name,
          logging_interface_, params_interface_))
      {
        RCLCPP_FATAL(
          logging_interface_->get_logger(),
          "Could not configure %s of type %s", filter.name.c_str(), filter.type.c_str());
        return false;
      }
      loaded_filters.emplace_back(std::move(loaded_filter));
    }

    // Everything went ok!
    reference_pointers_ = std::move(loaded_filters);
    on_set_parameters_callback_handle_ = params_interface_->add_on_set_parameters_callback(
      std::bind(&MultiChannelFilterChain<T>::reconfigureCB, this, std::placeholders::_1));
    // Allocate ahead of time
    buffer0_.resize(number_of_channels);
    buffer1_.resize(number_of_channels);
    configured_ = true;
    return true;
  }

  rcl_interfaces::msg::SetParametersResult reconfigureCB(std::vector<rclcpp::Parameter> parameters)
  {
    auto result = rcl_interfaces::msg::SetParametersResult();
    result.successful = true;

    for (auto & ref_ptr : reference_pointers_) {
      std::vector<rclcpp::Parameter> parameters_subset;
      for (auto parameter : parameters) {
        if (parameter.get_name().find(ref_ptr->getParamPrefix()) != std::string::npos) {
          parameters_subset.push_back(parameter);
        }
      }
      if (!parameters_subset.empty() && !ref_ptr->reconfigureCB(parameters_subset).successful) {
        result.successful = false;
      }
    }
    return result;
  }

private:
  pluginlib::ClassLoader<filters::MultiChannelFilterBase<T>> loader_;

  /// A vector of pointers to currently constructed filters
  std::vector<pluginlib::UniquePtr<filters::MultiChannelFilterBase<T>>> reference_pointers_;

  std::vector<T> buffer0_;  ///< A temporary intermediate buffer
  std::vector<T> buffer1_;  ///< A temporary intermediate buffer
  bool configured_;  ///< whether the system is configured

  rclcpp::node_interfaces::NodeParametersInterface::SharedPtr params_interface_;
  rclcpp::node_interfaces::NodeLoggingInterface::SharedPtr logging_interface_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr
    on_set_parameters_callback_handle_;
};

}  // namespace filters

#endif  // FILTERS__FILTER_CHAIN_HPP_
