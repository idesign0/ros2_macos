// Copyright 2015-2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RMW_CONNEXTDDS__SCOPE_EXIT_HPP_
#define RMW_CONNEXTDDS__SCOPE_EXIT_HPP_

/**
 * This header file is only needed when building the RMW with a version of
 * ROS equal or older than Foxy, since those versions don't include
 * rcpputils/scope_exit.hpp
 */

#include <utility>

namespace rcpputils
{

template<typename CallableT>
struct scope_exit final
{
  explicit scope_exit(CallableT && callable)
  : callable_(std::forward<CallableT>(callable))
  {
  }

  scope_exit(const scope_exit &) = delete;
  scope_exit(scope_exit &&) = default;

  scope_exit & operator=(const scope_exit &) = delete;
  scope_exit & operator=(scope_exit &&) = default;

  ~scope_exit()
  {
    if (!cancelled_) {
      callable_();
    }
  }

  void cancel()
  {
    cancelled_ = true;
  }

private:
  CallableT callable_;
  bool cancelled_{false};
};

template<typename CallableT>
scope_exit<CallableT>
make_scope_exit(CallableT && callable)
{
  return scope_exit<CallableT>(std::forward<CallableT>(callable));
}

}  // namespace rcpputils

#endif  // RMW_CONNEXTDDS__SCOPE_EXIT_HPP_
