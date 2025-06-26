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

/* Author: Martin Pecka */

#ifndef GEOMETRIC_SHAPES_OBB_H
#define GEOMETRIC_SHAPES_OBB_H

#include <memory>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <eigen_stl_containers/eigen_stl_containers.h>
#include <geometric_shapes/aabb.h>

namespace bodies
{
class OBBPrivate;

/** \brief Represents an oriented bounding box. */
class OBB
{
public:
  /** \brief Initialize an oriented bounding box at position 0, with 0 extents and
   *         identity orientation. */
  OBB();
  OBB(const OBB& other);
  OBB(const Eigen::Isometry3d& pose, const Eigen::Vector3d& extents);
  virtual ~OBB();

  OBB& operator=(const OBB& other);

  /**
   * \brief Set both the pose and extents of the OBB.
   * \param [in] pose New pose of the OBB.
   * \param [in] extents New extents of the OBB.
   */
  void setPoseAndExtents(const Eigen::Isometry3d& pose, const Eigen::Vector3d& extents);

  /**
   * \brief Get the extents of the OBB.
   * \return The extents.
   */
  Eigen::Vector3d getExtents() const;

  /**
   * \brief Get the extents of the OBB.
   * \param extents [out] The extents.
   */
  void getExtents(Eigen::Vector3d& extents) const;

  /**
   * \brief Get the pose of the OBB.
   * \return The pose.
   */
  Eigen::Isometry3d getPose() const;

  /**
   * \brief Get The pose of the OBB.
   * \param pose The pose.
   */
  void getPose(Eigen::Isometry3d& pose) const;

  /**
   * \brief Convert this OBB to an axis-aligned BB.
   * \return The AABB.
   */
  AABB toAABB() const;

  /**
   * \brief Convert this OBB to an axis-aligned BB.
   * \param aabb The AABB.
   */
  void toAABB(AABB& aabb) const;

  /**
   * \brief Add the other OBB to this one and compute an approximate enclosing OBB.
   * \param box The other box to add.
   * \return Pointer to this OBB after the update.
   */
  OBB* extendApprox(const OBB& box);

  /**
   * \brief Check if this OBB contains the given point.
   * \param point The point to check.
   * \return Whether the point is inside or not.
   */
  bool contains(const Eigen::Vector3d& point) const;

  /**
   * \brief Check whether this and the given OBBs have nonempty intersection.
   * \param other The other OBB to check.
   * \return Whether the OBBs overlap.
   */
  bool overlaps(const OBB& other) const;

  /**
   * \brief Check if this OBB contains whole other OBB.
   * \param point The point to check.
   * \return Whether the point is inside or not.
   */
  bool contains(const OBB& obb) const;

  /**
   * \brief Compute coordinates of the 8 vertices of this OBB.
   * \return The vertices.
   */
  EigenSTL::vector_Vector3d computeVertices() const;

protected:
  /** \brief PIMPL pointer */
  std::unique_ptr<OBBPrivate> obb_;
};
}  // namespace bodies

#endif  // GEOMETRIC_SHAPES_OBB_H
