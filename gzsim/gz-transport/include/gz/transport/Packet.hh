/*
 * Copyright (C) 2014 Open Source Robotics Foundation
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

#ifndef GZ_TRANSPORT_PACKET_HH_
#define GZ_TRANSPORT_PACKET_HH_

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "gz/transport/config.hh"
#include "gz/transport/Export.hh"
#include "gz/transport/Publisher.hh"

// This whole file is deprecated in version 8 of Ignition Transport. Please
// remove this file in Version 9.

namespace ignition
{
  namespace transport
  {
    // Inline bracket to help doxygen filtering.
    inline namespace IGNITION_TRANSPORT_VERSION_NAMESPACE {
    //
    // Message types.
    static const uint8_t Uninitialized  = 0;
    static const uint8_t AdvType        = 1;
    static const uint8_t SubType        = 2;
    static const uint8_t UnadvType      = 3;
    static const uint8_t HeartbeatType  = 4;
    static const uint8_t ByeType        = 5;
    static const uint8_t NewConnection  = 6;
    static const uint8_t EndConnection  = 7;

    // Flag set when a discovery message is relayed.
    static const uint16_t FlagRelay   = 0b000000000000'0001;
    // Flag set when we want to avoid to relay a discovery message.
    // This is used to avoid loops.
    static const uint16_t FlagNoRelay = 0b000000000000'0010;

    /// \brief Used for debugging the message type received/send.
    static const std::vector<std::string> MsgTypesStr =
    {
      "UNINITIALIZED", "ADVERTISE", "SUBSCRIBE", "UNADVERTISE", "HEARTBEAT",
      "BYE", "NEW_CONNECTION", "END_CONNECTION"
    };
    }
  }
}

#endif
