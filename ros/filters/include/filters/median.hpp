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

#ifndef FILTERS__MEDIAN_HPP_
#define FILTERS__MEDIAN_HPP_

#include <memory>
#include <utility>
#include <vector>

#include "filters/filter_base.hpp"

#include "filters/realtime_circular_buffer.hpp"


/*********************************************************************/
/*
 * Algorithm from N. Wirth's book, implementation by N. Devillard.
 * This code in public domain.
 */

namespace filters
{

/*---------------------------------------------------------------------------
  Function : kth_smallest()
  In : array of elements, # of elements in the array, rank k
  Out : one element
  Job : find the kth smallest element in the array
  Notice : use the median() macro defined below to get the median.
  Reference:
  Author: Wirth, Niklaus
  Title: Algorithms + data structures = programs
  Publisher: Englewood Cliffs: Prentice-Hall, 1976
  Physical description: 366 p.
  Series: Prentice-Hall Series in Automatic Computation
  ---------------------------------------------------------------------------*/
template<typename elem_type>
elem_type kth_smallest(elem_type * a, int n, int k)
{
  int i, j, l, m;
  elem_type x;
  l = 0; m = n - 1;
  while (l < m) {
    x = a[k];
    i = l;
    j = m;
    do {
      while (a[i] < x) {i++;}
      while (x < a[j]) {j--;}
      if (i <= j) {
        std::swap(a[i], a[j]);
        i++; j--;
      }
    } while (i <= j);
    if (j < k) {l = i;}
    if (k < i) {m = j;}
  }
  return a[k];
}

template<typename elem_type>
elem_type median(elem_type * a, int n)
{
  return kth_smallest(a, n, (((n) & 1) ? ((n) / 2) : (((n) / 2) - 1)));
}

/**
 * \brief A median filter which works on arrays.
 */
template<typename T>
class MedianFilter : public filters::FilterBase<T>
{
public:
  /**
   * \brief Construct the filter with the expected width and height
   */
  MedianFilter();

  /**
   * \brief Destructor to clean up
   */
  ~MedianFilter() override;

  bool configure() override;

  /**
   * \brief Update the filter and return the data seperately
   * \param data_in double array with length width
   * \param data_out double array with length width
   */
  bool update(const T & data_in, T & data_out) override;

protected:
  std::vector<T> temp_storage_;  ///< Preallocated storage for the list to sort
  std::unique_ptr<RealtimeCircularBuffer<T>> data_storage_;  ///< Storage for data between updates

  T temp;  // used for preallocation and copying from non vector source

  size_t number_of_observations_;  ///< Number of observations over which to filter
};

template<typename T>
MedianFilter<T>::MedianFilter()
: number_of_observations_(0)
{
}

template<typename T>
MedianFilter<T>::~MedianFilter()
{
}


template<typename T>
bool MedianFilter<T>::configure()
{
  if (!FilterBase<T>::getParam("number_of_observations", number_of_observations_)) {
    RCLCPP_ERROR(this->logging_interface_->get_logger(), "MedianFilter was not given params.\n");
    return false;
  }

  data_storage_.reset(new RealtimeCircularBuffer<T>(number_of_observations_, temp));
  temp_storage_.resize(number_of_observations_);

  return true;
}

template<typename T>
bool MedianFilter<T>::update(const T & data_in, T & data_out)
{
  if (!FilterBase<T>::configured_) {
    return false;
  }

  data_storage_->push_back(data_in);

  size_t length = data_storage_->size();
  for (size_t row = 0; row < length; ++row) {
    temp_storage_[row] = (*data_storage_)[row];
  }
  data_out = median(&temp_storage_[0], length);

  return true;
}

/**
 * \brief A median filter which works on arrays.
 */
template<typename T>
class MultiChannelMedianFilter : public filters::MultiChannelFilterBase<T>
{
public:
  /**
   * \brief Construct the filter with the expected width and height
   */
  MultiChannelMedianFilter();

  /**
   * \brief Destructor to clean up
   */
  ~MultiChannelMedianFilter() override;

  bool configure() override;

  /**
   * \brief Update the filter and return the data seperately
   * \param data_in T array with length width
   * \param data_out T array with length width
   */
  bool update(const std::vector<T> & data_in, std::vector<T> & data_out) override;

protected:
  std::vector<T> temp_storage_;  ///< Preallocated storage for the list to sort
  /// Storage for data between updates
  std::unique_ptr<RealtimeCircularBuffer<std::vector<T>>> data_storage_;

  std::vector<T> temp;  // used for preallocation and copying from non vector source

  size_t number_of_observations_;  ///< Number of observations over which to filter
};

template<typename T>
MultiChannelMedianFilter<T>::MultiChannelMedianFilter()
: number_of_observations_(0)
{
}

template<typename T>
MultiChannelMedianFilter<T>::~MultiChannelMedianFilter()
{
}

template<typename T>
bool MultiChannelMedianFilter<T>::configure()
{
  if (!FilterBase<T>::getParam("number_of_observations", number_of_observations_)) {
    RCLCPP_ERROR(
      this->logging_interface_->get_logger(), "MultiChannelMedianFilter was not given params.\n");
    return false;
  }

  temp.resize(this->number_of_channels_);
  data_storage_.reset(new RealtimeCircularBuffer<std::vector<T>>(number_of_observations_, temp));
  temp_storage_.resize(number_of_observations_);

  return true;
}

template<typename T>
bool MultiChannelMedianFilter<T>::update(const std::vector<T> & data_in, std::vector<T> & data_out)
{
  if (data_in.size() != this->number_of_channels_ || data_out.size() != this->number_of_channels_) {
    return false;
  }
  if (!FilterBase<T>::configured_) {
    return false;
  }

  data_storage_->push_back(data_in);

  size_t length = data_storage_->size();
  for (size_t i = 0; i < this->number_of_channels_; ++i) {
    for (size_t row = 0; row < length; ++row) {
      temp_storage_[row] = (*data_storage_)[row][i];
    }
    data_out[i] = median(&temp_storage_[0], length);
  }

  return true;
}

}  // namespace filters

#endif  // FILTERS__MEDIAN_HPP_
