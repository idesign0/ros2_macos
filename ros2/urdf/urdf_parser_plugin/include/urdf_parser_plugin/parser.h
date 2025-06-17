// Copyright (c) 2013, Willow Garage, Inc.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Willow Garage nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/* Author: Ioan Sucan */

#ifndef URDF_PARSER_PLUGIN__PARSER_H_
#define URDF_PARSER_PLUGIN__PARSER_H_

#include <string>

#include "urdf_world/types.h"

namespace urdf
{

/** \brief Base class for URDF parsers */
class URDFParser
{
public:
  URDFParser()
  {
  }
  virtual ~URDFParser()
  {
  }

  /// \brief Load Model from string
  /// \return nullptr and write to stderr if the given string is invalid
  virtual urdf::ModelInterfaceSharedPtr parse(const std::string & data) = 0;

  /// \brief Indicate if data is meant to be parsed by this parser
  /// \return The position in the string that the plugin became confident the
  ///         data is intended to be parsed by it.
  ///         For example, the plugin parsing COLLADA files might return the
  ///         position in the string that the '<COLLADA>' xml tag was found.
  ///         Smaller values are interpretted as more confidence, and the
  ///         plugin with the smallest value is used to parse the data.
  ///         If a plugin believes data is not meant for it, then it should
  ///         return a value greater than or equal to data.size().
  virtual size_t might_handle(const std::string & data) = 0;
};

}  // namespace urdf

#endif  // URDF_PARSER_PLUGIN__PARSER_H_
