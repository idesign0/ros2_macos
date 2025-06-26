// Copyright 2024 Open Robotics
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
//    * Neither the name of the Open Robotics nor the names of its
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

#include <geometric_shapes/obb.h>

#include <fcl/config.h>

#include <fcl/math/bv/OBB.h>
typedef fcl::Vector3d FCL_Vec3;
typedef fcl::OBB<double> FCL_OBB;

namespace bodies
{

class OBBPrivate : public FCL_OBB
{
public:
  using FCL_OBB::OBB;
};

OBB::OBB()
{
  obb_.reset(new OBBPrivate);
  // Initialize the OBB to position 0, with 0 extents and identity rotation
  // FCL 0.6+ does not zero-initialize the OBB.
  obb_->extent.setZero();
  obb_->To.setZero();
  obb_->axis.setIdentity();
}

OBB::OBB(const OBB& other)
{
  obb_.reset(new OBBPrivate(*other.obb_));
}

OBB::OBB(const Eigen::Isometry3d& pose, const Eigen::Vector3d& extents)
{
  obb_.reset(new OBBPrivate);
  setPoseAndExtents(pose, extents);
}

OBB& OBB::operator=(const OBB& other)
{
  *obb_ = *other.obb_;
  return *this;
}

void OBB::setPoseAndExtents(const Eigen::Isometry3d& pose, const Eigen::Vector3d& extents)
{
  const auto rotation = pose.linear();

  obb_->axis = rotation;

  obb_->To = pose.translation();

  obb_->extent = { extents[0] / 2.0, extents[1] / 2.0, extents[2] / 2.0 };
}

void OBB::getExtents(Eigen::Vector3d& extents) const
{
  extents = 2 * obb_->extent;
}

Eigen::Vector3d OBB::getExtents() const
{
  Eigen::Vector3d extents;
  getExtents(extents);
  return extents;
}

void OBB::getPose(Eigen::Isometry3d& pose) const
{
  pose = Eigen::Isometry3d::Identity();
  pose.translation() = obb_->To;
  pose.linear() = obb_->axis;
}

Eigen::Isometry3d OBB::getPose() const
{
  Eigen::Isometry3d pose;
  getPose(pose);
  return pose;
}

AABB OBB::toAABB() const
{
  AABB result;
  toAABB(result);
  return result;
}

void OBB::toAABB(AABB& aabb) const
{
  aabb.extendWithTransformedBox(getPose(), getExtents());
}

OBB* OBB::extendApprox(const OBB& box)
{
  if (this->getExtents() == Eigen::Vector3d::Zero())
  {
    *obb_ = *box.obb_;
    return this;
  }

  if (this->contains(box))
    return this;

  if (box.contains(*this))
  {
    *obb_ = *box.obb_;
    return this;
  }

  *this->obb_ += *box.obb_;
  return this;
}

bool OBB::contains(const Eigen::Vector3d& point) const
{
  return obb_->contain(point);
}

bool OBB::overlaps(const bodies::OBB& other) const
{
  return obb_->overlap(*other.obb_);
}

EigenSTL::vector_Vector3d OBB::computeVertices() const
{
  const Eigen::Vector3d e = getExtents() / 2;  // do not use auto type, Eigen would be inefficient
  // clang-format off
  EigenSTL::vector_Vector3d result = {
    { -e[0], -e[1], -e[2] },
    { -e[0], -e[1],  e[2] },
    { -e[0],  e[1], -e[2] },
    { -e[0],  e[1],  e[2] },
    {  e[0], -e[1], -e[2] },
    {  e[0], -e[1],  e[2] },
    {  e[0],  e[1], -e[2] },
    {  e[0],  e[1],  e[2] },
  };
  // clang-format on

  const auto pose = getPose();
  for (auto& v : result)
  {
    v = pose * v;
  }

  return result;
}

bool OBB::contains(const OBB& obb) const
{
  for (const auto& v : obb.computeVertices())
  {
    if (!contains(v))
      return false;
  }
  return true;
}

OBB::~OBB() = default;

}  // namespace bodies
