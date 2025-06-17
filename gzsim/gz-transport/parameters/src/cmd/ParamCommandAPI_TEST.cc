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

#include <iostream>
#include <sstream>

#include "gz/transport/parameters/Registry.hh"
#include "ParamCommandAPI.hh"

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/stringmsg.pb.h>

#include "gtest/gtest.h"

using namespace ignition;
using namespace ignition::transport;
using namespace ignition::transport::parameters;

class ParametersClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
    registry_.DeclareParameter("parameter1", std::make_unique<msgs::Boolean>());
    registry_.DeclareParameter(
      "parameter2", std::make_unique<msgs::StringMsg>());
    auto anotherStrMsg = std::make_unique<msgs::StringMsg>();
    anotherStrMsg->set_data("asd");
    registry_.DeclareParameter("parameter3", std::move(anotherStrMsg));
    registry_other_ns_.DeclareParameter(
      "another_param1", std::make_unique<msgs::Boolean>());
    anotherStrMsg = std::make_unique<msgs::StringMsg>();
    anotherStrMsg->set_data("bsd");
    registry_other_ns_.DeclareParameter(
      "another_param2", std::move(anotherStrMsg));
  }

  void TearDown() override {}

  ParametersRegistry registry_{""};
  ParametersRegistry registry_other_ns_{"/ns"};
};

class CaptureStreamScoped {
public:
  explicit CaptureStreamScoped(std::ostream & stream)
  : streamToCapture{stream},
    originalCerrStreamBuf{stream.rdbuf()}
  {
    stream.rdbuf(capturedBuffer.rdbuf());
  }

  ~CaptureStreamScoped()
  {
    streamToCapture.rdbuf(originalCerrStreamBuf);
  }

  std::string str()
  {
    return capturedBuffer.str();
  }

private:
  std::ostream & streamToCapture;
  std::streambuf * originalCerrStreamBuf;
  std::ostringstream capturedBuffer;
};

class CaptureCerrScoped : public CaptureStreamScoped {
public:
  CaptureCerrScoped()
  : CaptureStreamScoped(std::cerr)
  {}
};

class CaptureCoutScoped : public CaptureStreamScoped {
public:
  CaptureCoutScoped()
  : CaptureStreamScoped(std::cout)
  {}
};

//////////////////////////////////////////////////
TEST_F(ParametersClientTest, cmdParameterGet)
{
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParameterGet("", "parameter1");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("ign_msgs.Boolean"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParameterGet("", "parameter2");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("ign_msgs.StringMsg"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParameterGet("/ns", "another_param2");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("ign_msgs.StringMsg"));
    EXPECT_NE(std::string::npos, output.find("bsd"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCerrScoped cerrCapture;
    cmdParameterGet("", "paramaaaterDoesntExist");
    auto output = cerrCapture.str();
    EXPECT_NE(std::string::npos, output.find("Failed to get parameter:"));
  }
}

//////////////////////////////////////////////////
TEST_F(ParametersClientTest, SetParameter)
{
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParameterSet("", "parameter1", "ign_msgs.Boolean", "data: true");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("successfully"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParameterSet("", "parameter2", "ign_msgs.StringMsg", "data: \"foobar\"");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("successfully"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParameterSet("/ns", "another_param1", "ign_msgs.Boolean", "data: true");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("successfully"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCerrScoped cerrCapture;
    cmdParameterSet("", "parameter2", "ign_msgs.Boolean", "data: true");
    auto output = cerrCapture.str();
    EXPECT_NE(std::string::npos, output.find("Failed to set parameter"));
    EXPECT_NE(std::string::npos, output.find("type"));
  }
  {
    CaptureCerrScoped cerrCapture;
    cmdParameterSet(
      "", "parameter2", "ign_msgs.StringMsg", "not_a_field: \"foo\"");
    auto output = cerrCapture.str();
    EXPECT_NE(
      std::string::npos, output.find("string representation is invalid"));
  }
  {
    CaptureCerrScoped cerrCapture;
    cmdParameterSet("", "parameter2", "ign_msgs.NotAValidType", "");
    auto output = cerrCapture.str();
    EXPECT_NE(std::string::npos, output.find("message type is invalid"));
  }
}

//////////////////////////////////////////////////
TEST_F(ParametersClientTest, ListParameters)
{
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParametersList("");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("parameter1"));
    EXPECT_NE(std::string::npos, output.find("parameter2"));
    EXPECT_NE(std::string::npos, output.find("parameter3"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
  {
    CaptureCoutScoped coutCapture;
    CaptureCerrScoped cerrCapture;
    cmdParametersList("/ns");
    auto output = coutCapture.str();
    EXPECT_NE(std::string::npos, output.find("another_param1"));
    EXPECT_NE(std::string::npos, output.find("another_param2"));
    EXPECT_EQ(cerrCapture.str(), "");
  }
}
