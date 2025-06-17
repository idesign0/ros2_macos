/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2020, Open Source Robotics Foundation, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#include <tinyxml2.h>
#include <urdf_parser/urdf_parser.h>
#include <urdf_parser_plugin/parser.h>

#include <string>

namespace urdf
{
class URDFXMLParser final : public urdf::URDFParser
{
public:
  URDFXMLParser() = default;

  ~URDFXMLParser() = default;

  urdf::ModelInterfaceSharedPtr parse(const std::string & xml_string) override;

  size_t might_handle(const std::string & data) override;
};

urdf::ModelInterfaceSharedPtr URDFXMLParser::parse(const std::string & xml_string)
{
  return urdf::parseURDF(xml_string);
}

size_t URDFXMLParser::might_handle(const std::string & data)
{
  tinyxml2::XMLDocument doc;
  const tinyxml2::XMLError error = doc.Parse(data.c_str());
  if (error == tinyxml2::XML_SUCCESS) {
    // Since it's an XML document it must have `<robot>` as the first tag
    const tinyxml2::XMLElement * root = doc.RootElement();
    if (std::string("robot") != root->Name()) {
      return data.size();
    }
  }

  // Possiblities:
  //  1) It is not an XML based robot description
  //  2) It is an XML based robot description, but there's an XML syntax error
  //  3) It is a URDF XML with correct XML syntax
  return data.find("<robot");
}
}  // namespace urdf

#include <pluginlib/class_list_macros.hpp>  // NOLINT
PLUGINLIB_EXPORT_CLASS(urdf::URDFXMLParser, urdf::URDFParser)
