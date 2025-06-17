/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
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

/* Author: Wim Meeussen */

#ifndef URDF__MODEL_H_
#define URDF__MODEL_H_

#include <memory>
#include <string>

#include "urdf_model/model.h"

#include "urdf/urdfdom_compatibility.h"
#include "urdf/visibility_control.hpp"

namespace urdf
{

// PIMPL Forward Declaration
class ModelImplementation;

/// \brief Populates itself based on a robot descripton
///
/// This class uses `urdf_parser_plugin` to parse the given robot description.
/// The chosen plugin is the one that reports the most confident score.
/// There is no way to override this choice except by uninstalling undesirable
/// parser plugins.
class Model : public ModelInterface
{
public:
  URDF_EXPORT
  Model();

  URDF_EXPORT
  ~Model();

  URDF_EXPORT Model(const Model & other);
  URDF_EXPORT Model & operator=(const Model & other);
  URDF_EXPORT Model(Model && other) noexcept;
  URDF_EXPORT Model & operator=(Model && other)noexcept;

  /// \brief Load Model given a filename
  URDF_EXPORT bool initFile(const std::string & filename);

  /// \brief Load Model from a XML-string
  URDF_EXPORT bool initString(const std::string & xmlstring);

private:
  std::unique_ptr<ModelImplementation> impl_;
};

// shared_ptr declarations moved to urdf/urdfdom_compatibility.h to allow for
// std::shared_ptrs in latest version

}  // namespace urdf

#endif  // URDF__MODEL_H_
