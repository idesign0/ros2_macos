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

#include "ParamCommandAPI.hh"

#include <memory>
#include <string>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/empty.pb.h>
#include <ignition/msgs/parameter.pb.h>
#include <ignition/msgs/parameter_declarations.pb.h>
#include <ignition/msgs/parameter_name.pb.h>
#include <ignition/msgs/parameter_value.pb.h>

#include <gz/transport/parameters/Client.hh>

using namespace ignition;
using namespace transport;

//////////////////////////////////////////////////
extern "C" void cmdParametersList(const char * _ns)
{
  parameters::ParametersClient client{_ns};

  std::cout << std::endl << "Listing parameters, registry namespace [" << _ns
            << "]..." << std::endl << std::endl;

  auto res = client.ListParameters();
  if (!res.parameter_declarations_size()) {
    std::cout << "No parameters available" << std::endl;
    return;
  }
  for (const auto & decl : res.parameter_declarations()) {
    std::cout << decl.name() << "            [" << decl.type() << "]"
              << std::endl;
  }
}

//////////////////////////////////////////////////
extern "C" void cmdParameterGet(const char * _ns, const char *_paramName) {
  parameters::ParametersClient client{_ns};

  std::cout << std::endl << "Getting parameter [" << _paramName
            << "] for registry namespace [" << _ns << "]..." << std::endl;

  std::unique_ptr<google::protobuf::Message> value;
  auto ret = client.Parameter(_paramName, value);
  if (!ret) {
    std::cerr << "Failed to get parameter: " << ret << std::endl;
    return;
  }

  std::string msgType = "ign_msgs.";
  msgType += value->GetDescriptor()->name();
  std::cout << "Parameter type [" << msgType << "]" << std::endl << std::endl
            << "------------------------------------------------"
            << std::endl;
  {
    google::protobuf::io::OstreamOutputStream fos{&std::cout};
    if (!google::protobuf::TextFormat::Print(*value, &fos)) {
      std::cerr << "failed to convert the parameter value to a string"
                << std::endl;
      return;
    }
  }
  std::cout << "------------------------------------------------"
            << std::endl;
}

extern "C" void cmdParameterSet(
    const char * _ns, const char *_paramName, const char * _paramType,
    const char *_paramValue)
{
  parameters::ParametersClient client{_ns};

  std::cout << std::endl << "Setting parameter [" << _paramName
            << "] for registry namespace [" << _ns << "]..." << std::endl;

  auto msg = ignition::msgs::Factory::New(_paramType);
  if (!msg) {
    std::cerr << "Could not create a message of type [" << _paramType
              << "]. The message type is invalid."
              << std::endl;
    return;
  }
  if (!google::protobuf::TextFormat::ParseFromString(_paramValue, msg.get())) {
    std::cerr << "Could not create a message of type [" << _paramType
              << "]. The message string representation is invalid."
              << std::endl;
    return;
  }
  auto ret = client.SetParameter(_paramName, *msg);
  if (!ret) {
    std::cerr << "Failed to set parameter: " << ret << std::endl;
  }
  std::cout << "Parameter successfully set!" << std::endl;
}
