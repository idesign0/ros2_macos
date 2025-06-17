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

#include "gz/transport/parameters/Client.hh"

#include <memory>
#include <sstream>
#include <string>

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/parameter_error.pb.h>
#include <ignition/msgs/parameter_name.pb.h>
#include <ignition/msgs/parameter_value.pb.h>

#include "gz/transport/parameters/result.hh"

#include "Utils.hh"

using namespace ignition;
using namespace transport;
using namespace parameters;

struct ignition::transport::parameters::ParametersClientPrivate
{
  ParametersClientPrivate(
    const std::string & _serverNamespace,
    unsigned int _timeoutMs)
  : serverNamespace{_serverNamespace},
    timeoutMs{_timeoutMs}
  {}
  std::string serverNamespace;
  mutable ignition::transport::Node node;
  unsigned int timeoutMs;
};

//////////////////////////////////////////////////
ParametersClient::~ParametersClient() = default;

//////////////////////////////////////////////////
ParametersClient::ParametersClient(ParametersClient &&) = default;

//////////////////////////////////////////////////
ParametersClient & ParametersClient::operator=(ParametersClient &&) = default;

//////////////////////////////////////////////////
ParametersClient::ParametersClient(
  const std::string & _serverNamespace,
  unsigned int _timeoutMs)
: dataPtr{std::make_unique<ParametersClientPrivate>(
    _serverNamespace,
    _timeoutMs)}
{}

//////////////////////////////////////////////////
static ParameterResult
getParameterCommon(
  const ParametersClientPrivate & _dataPtr,
  const std::string & _parameterName,
  msgs::ParameterValue & _parameterValue)
{
  bool result{false};
  const std::string service{_dataPtr.serverNamespace + "/get_parameter"};

  msgs::ParameterName req;

  req.set_name(_parameterName);

  if (!_dataPtr.node.Request(
    service, req, _dataPtr.timeoutMs, _parameterValue, result))
  {
    return ParameterResult{ParameterResultType::ClientTimeout, _parameterName};
  }
  if (!result)
  {
    return ParameterResult{ParameterResultType::NotDeclared, _parameterName};
  }
  return ParameterResult{ParameterResultType::Success};
}

//////////////////////////////////////////////////
ParameterResult
ParametersClient::Parameter(
  const std::string & _parameterName,
  google::protobuf::Message & _parameter) const
{
  msgs::ParameterValue res;
  auto ret = getParameterCommon(*this->dataPtr, _parameterName, res);
  auto ignTypeOpt = getIgnTypeFromAnyProto(res.data());
  if (!ignTypeOpt) {
    return ParameterResult{
      ParameterResultType::Unexpected,
      _parameterName};
  }
  auto ignType = *ignTypeOpt;
  if (ignType != _parameter.GetDescriptor()->name()) {
    return ParameterResult{
      ParameterResultType::InvalidType, _parameterName, ignType};
  }
  if (!res.data().UnpackTo(&_parameter)) {
    return ParameterResult{
      ParameterResultType::Unexpected, _parameterName, ignType};
  }
  return ParameterResult{ParameterResultType::Success};
}

//////////////////////////////////////////////////
ParameterResult
ParametersClient::Parameter(
  const std::string & _parameterName,
  std::unique_ptr<google::protobuf::Message> & _parameter) const
{
  msgs::ParameterValue res;
  auto ret = getParameterCommon(*this->dataPtr, _parameterName, res);
  auto ignTypeOpt = getIgnTypeFromAnyProto(res.data());
  if (!ignTypeOpt) {
    return ParameterResult{
      ParameterResultType::Unexpected,
      _parameterName};
  }
  auto ignType = *ignTypeOpt;
  _parameter = ignition::msgs::Factory::New(ignType);
  if (!_parameter) {
    return ParameterResult{
      ParameterResultType::Unexpected, _parameterName, ignType};
  }
  if (!res.data().UnpackTo(_parameter.get())) {
    return ParameterResult{
      ParameterResultType::Unexpected, _parameterName, ignType};
  }
  return ParameterResult{ParameterResultType::Success};
}

//////////////////////////////////////////////////
ParameterResult
ParametersClient::SetParameter(
  const std::string & _parameterName,
  const google::protobuf::Message & _msg)
{
  bool result{false};
  const std::string service{dataPtr->serverNamespace + "/set_parameter"};

  msgs::Parameter req;
  msgs::ParameterError res;

  req.set_name(_parameterName);
  req.mutable_value()->PackFrom(_msg);

  if (!dataPtr->node.Request(service, req, dataPtr->timeoutMs, res, result))
  {
    return ParameterResult{ParameterResultType::ClientTimeout, _parameterName};
  }
  if (!result)
  {
    return ParameterResult{ParameterResultType::Unexpected, _parameterName};
  }
  if (res.data() == msgs::ParameterError::SUCCESS) {
    return ParameterResult{ParameterResultType::Success};
  }
  if (res.data() == msgs::ParameterError::NOT_DECLARED) {
    return ParameterResult{ParameterResultType::NotDeclared, _parameterName};
  }
  if (res.data() == msgs::ParameterError::INVALID_TYPE) {
    return ParameterResult{
      ParameterResultType::InvalidType,
      _parameterName};
  }
  return ParameterResult{ParameterResultType::Unexpected, _parameterName};
}

//////////////////////////////////////////////////
ParameterResult
ParametersClient::DeclareParameter(
  const std::string & _parameterName,
  const google::protobuf::Message & _msg)
{
  bool result{false};
  const std::string service{dataPtr->serverNamespace + "/declare_parameter"};

  msgs::Parameter req;
  msgs::ParameterError res;

  req.set_name(_parameterName);
  req.mutable_value()->PackFrom(_msg);

  if (!dataPtr->node.Request(service, req, dataPtr->timeoutMs, res, result))
  {
    return ParameterResult{ParameterResultType::ClientTimeout, _parameterName};
  }
  if (!result)
  {
    return ParameterResult{ParameterResultType::Unexpected, _parameterName};
  }
  if (res.data() == msgs::ParameterError::SUCCESS) {
    return ParameterResult{ParameterResultType::Success};
  }
  if (res.data() == msgs::ParameterError::ALREADY_DECLARED) {
    return ParameterResult{
      ParameterResultType::AlreadyDeclared, _parameterName};
  }
  if (res.data() == msgs::ParameterError::INVALID_TYPE) {
    return ParameterResult{
      ParameterResultType::InvalidType,
      _parameterName,
      std::string(_msg.GetDescriptor()->name())};
  }
  return ParameterResult{ParameterResultType::Unexpected, _parameterName};
}

//////////////////////////////////////////////////
msgs::ParameterDeclarations
ParametersClient::ListParameters() const
{
  bool result{false};
  const std::string service{dataPtr->serverNamespace + "/list_parameters"};

  msgs::Empty req;
  msgs::ParameterDeclarations res;

  if (!dataPtr->node.Request(service, req, dataPtr->timeoutMs, res, result))
  {
    throw std::runtime_error{
      "ParametersClient::ListParameters(): request timed out"};
  }
  if (!result)
  {
    throw std::runtime_error {
      "ParametersClient::ListParameters(): unexpected error"};
  }
  return res;
}
