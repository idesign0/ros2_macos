/*
 * Copyright (c) 2018, Open Source Robotics Foundation, Inc.
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

#ifndef TEST_PLUGINLIB_FIXTURE__VISIBILITY_CONTROL_HPP_
#define TEST_PLUGINLIB_FIXTURE__VISIBILITY_CONTROL_HPP_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define TEST_PLUGINLIB_FIXTURE_EXPORT __attribute__ ((dllexport))
    #define TEST_PLUGINLIB_FIXTURE_IMPORT __attribute__ ((dllimport))
  #else
    #define TEST_PLUGINLIB_FIXTURE_EXPORT __declspec(dllexport)
    #define TEST_PLUGINLIB_FIXTURE_IMPORT __declspec(dllimport)
  #endif
  #ifdef TEST_PLUGINLIB_FIXTURE_BUILDING_LIBRARY
    #define TEST_PLUGINLIB_FIXTURE_PUBLIC TEST_PLUGINLIB_FIXTURE_EXPORT
  #else
    #define TEST_PLUGINLIB_FIXTURE_PUBLIC TEST_PLUGINLIB_FIXTURE_IMPORT
  #endif
  #define TEST_PLUGINLIB_FIXTURE_PUBLIC_TYPE TEST_PLUGINLIB_FIXTURE_PUBLIC
  #define TEST_PLUGINLIB_FIXTURE_LOCAL
#else
  #define TEST_PLUGINLIB_FIXTURE_EXPORT __attribute__ ((visibility("default")))
  #define TEST_PLUGINLIB_FIXTURE_IMPORT
  #if __GNUC__ >= 4
    #define TEST_PLUGINLIB_FIXTURE_PUBLIC __attribute__ ((visibility("default")))
    #define TEST_PLUGINLIB_FIXTURE_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define TEST_PLUGINLIB_FIXTURE_PUBLIC
    #define TEST_PLUGINLIB_FIXTURE_LOCAL
  #endif
  #define TEST_PLUGINLIB_FIXTURE_PUBLIC_TYPE
#endif

#endif  // TEST_PLUGINLIB_FIXTURE__VISIBILITY_CONTROL_HPP_
