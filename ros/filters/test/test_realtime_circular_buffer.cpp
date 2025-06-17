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

#include <gtest/gtest.h>
#include <sys/time.h>

#include <vector>
#include <utility>
#include "filters/realtime_circular_buffer.hpp"

TEST(RealtimeCircularBuffer, InitializationScalar)
{
  filters::RealtimeCircularBuffer<double> buf(3, 0);
  for (size_t i = 0; i < buf.size(); ++i) {
    EXPECT_EQ(buf[i], 0);
  }
}

TEST(RealtimeCircularBuffer, InitializationVector)
{
  std::vector<double> init_vector;
  for (size_t i = 0; i < 100; ++i) {
    init_vector.push_back(i);
  }

  filters::RealtimeCircularBuffer<std::vector<double>> vec_buf(3, init_vector);
  for (size_t i = 0; i < vec_buf.size(); ++i) {
    for (size_t j = 0; j < 100; ++j) {
      EXPECT_EQ(vec_buf[i][j], j);
    }
  }
}

TEST(RealtimeCircularBuffer, RangeBasedLoop)
{
  filters::RealtimeCircularBuffer<int> vec_buf(100, 1);
  int sum = 0;
  for (const auto & i : vec_buf) {
    sum += i;
  }
  EXPECT_EQ(sum, 100);
}
