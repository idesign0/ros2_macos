/*
 * Copyright (c) 2009, Willow Garage, Inc.
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

#ifndef FILTERS__MEAN_HPP_
#define FILTERS__MEAN_HPP_

#include <memory>
#include <vector>

#include "filters/filter_base.hpp"
#include "filters/realtime_circular_buffer.hpp"

namespace filters
{

/**
 * \brief A mean filter which works on doubles.
 */
template<typename T>
class MeanFilter : public FilterBase<T>
{
public:
  /**
   * \brief Construct the filter with the expected width and height
   */
  MeanFilter();

  /**
   * \brief Destructor to clean up
   */
  ~MeanFilter() override;

  bool configure() override;

  /**
   * \brief Update the filter and return the data seperately
   * \param data_in T array with length width
   * \param data_out T array with length width
   */
  bool update(const T & data_in, T & data_out) override;

protected:
  std::unique_ptr<RealtimeCircularBuffer<T>> data_storage_;  ///< Storage for data between updates
  uint32_t last_updated_row_;  ///< The last row to have been updated by the filter
  T temp_;  // Temporary storage
  uint32_t number_of_observations_;  ///< Number of observations over which to filter
};


template<typename T>
MeanFilter<T>::MeanFilter()
: number_of_observations_(0)
{
}

template<typename T>
MeanFilter<T>::~MeanFilter()
{
}

template<typename T>
bool MeanFilter<T>::configure()
{
  if (!FilterBase<T>::getParam("number_of_observations", number_of_observations_)) {
    RCLCPP_ERROR(
      this->logging_interface_->get_logger(),
      "MeanFilter did not find param number_of_observations");
    return false;
  }

  data_storage_.reset(new RealtimeCircularBuffer<T>(number_of_observations_, temp_));

  return true;
}

template<typename T>
bool MeanFilter<T>::update(const T & data_in, T & data_out)
{
  // update active row
  if (last_updated_row_ >= number_of_observations_ - 1) {
    last_updated_row_ = 0;
  } else {
    ++last_updated_row_;
  }

  data_storage_->push_back(data_in);

  size_t length = data_storage_->size();

  data_out = 0;
  for (size_t row = 0; row < length; ++row) {
    data_out += data_storage_->at(row);
  }
  data_out /= length;

  return true;
}

/**
 * \brief A mean filter which works on double arrays.
 */
template<typename T>
class MultiChannelMeanFilter : public MultiChannelFilterBase<T>
{
public:
  /**
   * \brief Construct the filter with the expected width and height
   */
  MultiChannelMeanFilter();

  /**
   * \brief Destructor to clean up
   */
  ~MultiChannelMeanFilter() override;

  bool configure() override;

  /**
   * \brief Update the filter and return the data separately
   * \param data_in T array with length width
   * \param data_out T array with length width
   */
  bool update(const std::vector<T> & data_in, std::vector<T> & data_out) override;

protected:
  /// Storage for data between updates
  std::unique_ptr<RealtimeCircularBuffer<std::vector<T>>> data_storage_;
  uint32_t last_updated_row_;  ///< The last row to have been updated by the filter
  std::vector<T> temp;  // used for preallocation and copying from non vector source

  uint32_t number_of_observations_;  ///< Number of observations over which to filter
  using MultiChannelFilterBase<T>::number_of_channels_;  ///< Number of elements per observation
};


template<typename T>
MultiChannelMeanFilter<T>::MultiChannelMeanFilter()
: number_of_observations_(0)
{
}

template<typename T>
MultiChannelMeanFilter<T>::~MultiChannelMeanFilter()
{
}

template<typename T>
bool MultiChannelMeanFilter<T>::configure()
{
  if (!FilterBase<T>::getParam("number_of_observations", number_of_observations_)) {
    RCLCPP_ERROR(
      this->logging_interface_->get_logger(),
      "MultiChannelMeanFilter did not find param number_of_observations");
    return false;
  }

  temp.resize(number_of_channels_);
  data_storage_.reset(new RealtimeCircularBuffer<std::vector<T>>(number_of_observations_, temp));

  return true;
}

template<typename T>
bool MultiChannelMeanFilter<T>::update(const std::vector<T> & data_in, std::vector<T> & data_out)
{
  if (data_in.size() != number_of_channels_ || data_out.size() != number_of_channels_) {
    RCLCPP_ERROR(
      this->logging_interface_->get_logger(),
      "Configured with wrong size config: %ld, in: %ld out: %ld",
      number_of_channels_, data_in.size(), data_out.size());
    return false;
  }

  // update active row
  if (last_updated_row_ >= number_of_observations_ - 1) {
    last_updated_row_ = 0;
  } else {
    ++last_updated_row_;
  }

  data_storage_->push_back(data_in);

  size_t length = data_storage_->size();

  // Return each value
  for (size_t i = 0; i < number_of_channels_; ++i) {
    data_out[i] = 0;
    for (size_t row = 0; row < length; ++row) {
      data_out[i] += data_storage_->at(row)[i];
    }
    data_out[i] /= length;
  }

  return true;
}

}  // namespace filters

#endif  // FILTERS__MEAN_HPP_
