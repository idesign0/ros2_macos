/*
 * Copyright (C) 2022 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef IGNITION_TRANSPORT_PARAMETERS_UTILS_HH_
#define IGNITION_TRANSPORT_PARAMETERS_UTILS_HH_

#include <optional>
#include <string>

#include "gz/transport/config.hh"
#include "gz/transport/parameters/Export.hh"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4251)  // Missing dll-interface
#endif  // defined(_MSC_VER)
#include <google/protobuf/any.pb.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif  // defined(_MSC_VER)

namespace ignition
{
  namespace transport
  {
    namespace parameters
    {
      // Inline bracket to help doxygen filtering.
      inline namespace IGNITION_TRANSPORT_VERSION_NAMESPACE {

      /// \brief Return the protobuf type prefixed with "ign_msgs."
      /// \param ignType Type name to be prefixed.
      /// \return The protobuf type with the prefix added.
      IGNITION_TRANSPORT_PARAMETERS_VISIBLE
      std::string addIgnMsgsPrefix(const std::string & ignType);

      /// \brief Get the ignition message type from a protobuf message.
      /// \param any Message to get the type.
      /// \return A string with the ignition protobuf type,
      ///   or nullopt if it fails.
      IGNITION_TRANSPORT_PARAMETERS_VISIBLE
      std::optional<std::string> getIgnTypeFromAnyProto(
        const google::protobuf::Any & any);
      }
    }
  }
}

#endif  // IGNITION_TRANSPORT_PARAMETERS_UTILS_HH_
