// Copyright 2008 Willow Garage, Inc.
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
//    * Neither the name of the Willow Garage, Inc. nor the names of its
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

#include "geometric_shapes/bodies.h"
#include "geometric_shapes/body_operations.h"
#include "geometric_shapes/check_isometry.h"

#include <console_bridge/console.h>

extern "C" {
#include <libqhull_r.h>
}

#include <boost/math/constants/constants.hpp>
#include <limits>
#include <cstdio>
#include <cmath>  // std::fmin, std::fmax
#include <algorithm>
#include <Eigen/Geometry>
#include <unordered_map>

namespace bodies
{
namespace detail
{
static const double ZERO = 1e-9;

/** \brief Compute the square of the distance between a ray and a point
    Note: this requires 'dir' to be normalized */
static inline double distanceSQR(const Eigen::Vector3d& p, const Eigen::Vector3d& origin, const Eigen::Vector3d& dir)
{
  Eigen::Vector3d a = p - origin;
  double d = dir.normalized().dot(a);
  return a.squaredNorm() - d * d;
}

// temp structure for intersection points (used for ordering them)
struct intersc
{
  intersc(const Eigen::Vector3d& _pt, const double _tm) : pt(_pt), time(_tm)
  {
  }

  Eigen::Vector3d pt;
  double time;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

// define order on intersection points
struct interscOrder
{
  bool operator()(const intersc& a, const intersc& b) const
  {
    return a.time < b.time;
  }
};

/**
 * \brief Take intersections points in ipts and add them to intersections, filtering duplicates.
 * \param ipts The source list of intersections (will be modified (sorted)).
 * \param intersections The output list of intersection points.
 * \param count The maximum count of returned intersection points. 0 = return all points.
 */
void filterIntersections(std::vector<detail::intersc>& ipts, EigenSTL::vector_Vector3d* intersections,
                         const size_t count)
{
  if (intersections == nullptr || ipts.empty())
    return;

  std::sort(ipts.begin(), ipts.end(), interscOrder());
  const auto n = count > 0 ? std::min<size_t>(count, ipts.size()) : ipts.size();

  for (const auto& p : ipts)
  {
    if (intersections->size() == n)
      break;
    if (!intersections->empty() && p.pt.isApprox(intersections->back(), ZERO))
      continue;
    intersections->push_back(p.pt);
  }
}
}  // namespace detail

inline Eigen::Vector3d normalize(const Eigen::Vector3d& dir)
{
  const double norm = dir.squaredNorm();
#if EIGEN_VERSION_AT_LEAST(3, 3, 0)
  return ((norm - 1) > 1e-9) ? (dir / Eigen::numext::sqrt(norm)) : dir;
#else  // used in kinetic
  return ((norm - 1) > 1e-9) ? (dir / sqrt(norm)) : dir;
#endif
}
}  // namespace bodies

bool bodies::Body::samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int max_attempts,
                                     Eigen::Vector3d& result) const
{
  BoundingSphere bs;
  computeBoundingSphere(bs);
  for (unsigned int i = 0; i < max_attempts; ++i)
  {
    result = Eigen::Vector3d(rng.uniformReal(bs.center.x() - bs.radius, bs.center.x() + bs.radius),
                             rng.uniformReal(bs.center.y() - bs.radius, bs.center.y() + bs.radius),
                             rng.uniformReal(bs.center.z() - bs.radius, bs.center.z() + bs.radius));
    if (containsPoint(result))
      return true;
  }
  return false;
}

bool bodies::Sphere::containsPoint(const Eigen::Vector3d& p, bool /* verbose */) const
{
  return (center_ - p).squaredNorm() <= radius2_;
}

void bodies::Sphere::useDimensions(const shapes::Shape* shape)  // radius
{
  radius_ = static_cast<const shapes::Sphere*>(shape)->radius;
}

std::vector<double> bodies::Sphere::getDimensions() const
{
  return { radius_ };
}

std::vector<double> bodies::Sphere::getScaledDimensions() const
{
  return { radiusU_ };
}

void bodies::Sphere::updateInternalData()
{
  const auto tmpRadiusU = radius_ * scale_ + padding_;
  if (tmpRadiusU < 0)
    throw std::runtime_error("Sphere radius must be non-negative.");
  radiusU_ = tmpRadiusU;
  radius2_ = radiusU_ * radiusU_;
  center_ = pose_.translation();
}

std::shared_ptr<bodies::Body> bodies::Sphere::cloneAt(const Eigen::Isometry3d& pose, double padding, double scale) const
{
  auto s = std::allocate_shared<Sphere>(Eigen::aligned_allocator<Sphere>());
  s->radius_ = radius_;
  s->padding_ = padding;
  s->scale_ = scale;
  s->pose_ = pose;
  s->updateInternalData();
  return s;
}

double bodies::Sphere::computeVolume() const
{
  return 4.0 * boost::math::constants::pi<double>() * radiusU_ * radiusU_ * radiusU_ / 3.0;
}

void bodies::Sphere::computeBoundingSphere(BoundingSphere& sphere) const
{
  sphere.center = center_;
  sphere.radius = radiusU_;
}

void bodies::Sphere::computeBoundingCylinder(BoundingCylinder& cylinder) const
{
  cylinder.pose = pose_;
  cylinder.radius = radiusU_;
  cylinder.length = 2.0 * radiusU_;
}

void bodies::Sphere::computeBoundingBox(bodies::AABB& bbox) const
{
  bbox.setEmpty();

  // it's a sphere, so we do not rotate the bounding box
  Eigen::Isometry3d transform = Eigen::Isometry3d::Identity();
  transform.translation() = getPose().translation();

  bbox.extendWithTransformedBox(transform, Eigen::Vector3d(2 * radiusU_, 2 * radiusU_, 2 * radiusU_));
}

void bodies::Sphere::computeBoundingBox(bodies::OBB& bbox) const
{
  // it's a sphere, so we do not rotate the bounding box
  Eigen::Isometry3d transform = Eigen::Isometry3d::Identity();
  transform.translation() = getPose().translation();

  bbox.setPoseAndExtents(transform, 2 * Eigen::Vector3d(radiusU_, radiusU_, radiusU_));
}

bool bodies::Sphere::samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int max_attempts,
                                       Eigen::Vector3d& result) const
{
  for (unsigned int i = 0; i < max_attempts; ++i)
  {
    const double minX = center_.x() - radiusU_;
    const double maxX = center_.x() + radiusU_;
    const double minY = center_.y() - radiusU_;
    const double maxY = center_.y() + radiusU_;
    const double minZ = center_.z() - radiusU_;
    const double maxZ = center_.z() + radiusU_;
    // we are sampling in a box; the probability of success after 20 attempts is 99.99996% given the ratio of box volume
    // to sphere volume
    for (int j = 0; j < 20; ++j)
    {
      result = Eigen::Vector3d(rng.uniformReal(minX, maxX), rng.uniformReal(minY, maxY), rng.uniformReal(minZ, maxZ));
      if (containsPoint(result))
        return true;
    }
  }
  return false;
}

bool bodies::Sphere::intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                                   EigenSTL::vector_Vector3d* intersections, unsigned int count) const
{
  // this is faster than always calling dir.normalized() in case the vector is already unit
  const Eigen::Vector3d dirNorm = normalize(dir);

  if (detail::distanceSQR(center_, origin, dirNorm) > radius2_)
    return false;

  bool result = false;

  Eigen::Vector3d cp = origin - center_;
  double dpcpv = cp.dot(dirNorm);

  Eigen::Vector3d w = cp - dpcpv * dirNorm;
  Eigen::Vector3d Q = center_ + w;
  double x = radius2_ - w.squaredNorm();

  if (fabs(x) < detail::ZERO)
  {
    w = Q - origin;
    double dpQv = w.dot(dirNorm);
    if (dpQv > detail::ZERO)
    {
      if (intersections)
        intersections->push_back(Q);
      result = true;
    }
  }
  else if (x > 0.0)
  {
    x = sqrt(x);
    w = dirNorm * x;
    Eigen::Vector3d A = Q - w;
    Eigen::Vector3d B = Q + w;
    w = A - origin;
    double dpAv = w.dot(dirNorm);
    w = B - origin;
    double dpBv = w.dot(dirNorm);

    if (dpAv > detail::ZERO)
    {
      result = true;
      if (intersections)
      {
        intersections->push_back(A);
        if (count == 1)
          return result;
      }
    }

    if (dpBv > detail::ZERO)
    {
      result = true;
      if (intersections)
        intersections->push_back(B);
    }
  }
  return result;
}

bodies::Sphere::Sphere(const bodies::BoundingSphere& sphere) : Body()
{
  type_ = shapes::SPHERE;
  shapes::Sphere shape(sphere.radius);
  setDimensionsDirty(&shape);

  Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  pose.translation() = sphere.center;
  setPose(pose);
}

bool bodies::Cylinder::containsPoint(const Eigen::Vector3d& p, bool /* verbose */) const
{
  Eigen::Vector3d v = p - center_;
  double pH = v.dot(normalH_);

  if (fabs(pH) > length2_)
    return false;

  double pB1 = v.dot(normalB1_);
  double remaining = radius2_ - pB1 * pB1;

  if (remaining < 0.0)
    return false;
  else
  {
    double pB2 = v.dot(normalB2_);
    return pB2 * pB2 <= remaining;
  }
}

void bodies::Cylinder::useDimensions(const shapes::Shape* shape)  // (length, radius)
{
  length_ = static_cast<const shapes::Cylinder*>(shape)->length;
  radius_ = static_cast<const shapes::Cylinder*>(shape)->radius;
}

std::vector<double> bodies::Cylinder::getDimensions() const
{
  return { radius_, length_ };
}

std::vector<double> bodies::Cylinder::getScaledDimensions() const
{
  return { radiusU_, 2 * length2_ };
}

void bodies::Cylinder::updateInternalData()
{
  const auto tmpRadiusU = radius_ * scale_ + padding_;
  if (tmpRadiusU < 0)
    throw std::runtime_error("Cylinder radius must be non-negative.");
  const auto tmpLength2 = scale_ * length_ / 2.0 + padding_;
  if (tmpLength2 < 0)
    throw std::runtime_error("Cylinder length must be non-negative.");
  radiusU_ = tmpRadiusU;
  length2_ = tmpLength2;
  radius2_ = radiusU_ * radiusU_;
  center_ = pose_.translation();
  radiusBSqr_ = length2_ * length2_ + radius2_;
  radiusB_ = sqrt(radiusBSqr_);

  ASSERT_ISOMETRY(pose_);
  Eigen::Matrix3d basis = pose_.linear();
  normalB1_ = basis.col(0);
  normalB2_ = basis.col(1);
  normalH_ = basis.col(2);

  double tmp = -normalH_.dot(center_);
  d1_ = tmp + length2_;
  d2_ = tmp - length2_;
}

bool bodies::Cylinder::samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int /* max_attempts */,
                                         Eigen::Vector3d& result) const
{
  // sample a point on the base disc of the cylinder
  double a = rng.uniformReal(-boost::math::constants::pi<double>(), boost::math::constants::pi<double>());
  double r = rng.uniformReal(-radiusU_, radiusU_);
  double x = cos(a) * r;
  double y = sin(a) * r;

  // sample e height
  double z = rng.uniformReal(-length2_, length2_);

  result = pose_ * Eigen::Vector3d(x, y, z);
  return true;
}

std::shared_ptr<bodies::Body> bodies::Cylinder::cloneAt(const Eigen::Isometry3d& pose, double padding,
                                                        double scale) const
{
  auto c = std::allocate_shared<Cylinder>(Eigen::aligned_allocator<Cylinder>());
  c->length_ = length_;
  c->radius_ = radius_;
  c->padding_ = padding;
  c->scale_ = scale;
  c->pose_ = pose;
  c->updateInternalData();
  return c;
}

double bodies::Cylinder::computeVolume() const
{
  return 2.0 * boost::math::constants::pi<double>() * radius2_ * length2_;
}

void bodies::Cylinder::computeBoundingSphere(BoundingSphere& sphere) const
{
  sphere.center = center_;
  sphere.radius = radiusB_;
}

void bodies::Cylinder::computeBoundingCylinder(BoundingCylinder& cylinder) const
{
  cylinder.pose = pose_;
  cylinder.radius = radiusU_;
  cylinder.length = 2 * length2_;
}

void bodies::Cylinder::computeBoundingBox(bodies::AABB& bbox) const
{
  bbox.setEmpty();

  // method taken from http://www.iquilezles.org/www/articles/diskbbox/diskbbox.htm

  const auto a = normalH_;
  const auto e = radiusU_ * (Eigen::Vector3d::Ones() - a.cwiseProduct(a) / a.dot(a)).cwiseSqrt();
  const auto pa = center_ + length2_ * normalH_;
  const auto pb = center_ - length2_ * normalH_;

  bbox.extend(pa - e);
  bbox.extend(pa + e);
  bbox.extend(pb - e);
  bbox.extend(pb + e);
}

void bodies::Cylinder::computeBoundingBox(bodies::OBB& bbox) const
{
  bbox.setPoseAndExtents(getPose(), 2 * Eigen::Vector3d(radiusU_, radiusU_, length2_));
}

bool bodies::Cylinder::intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                                     EigenSTL::vector_Vector3d* intersections, unsigned int count) const
{
  // this is faster than always calling dir.normalized() in case the vector is already unit
  const Eigen::Vector3d dirNorm = normalize(dir);

  if (detail::distanceSQR(center_, origin, dirNorm) > radiusBSqr_)
    return false;

  std::vector<detail::intersc> ipts;

  // intersect bases
  double tmp = normalH_.dot(dirNorm);
  if (fabs(tmp) > detail::ZERO)
  {
    double tmp2 = -normalH_.dot(origin);
    double t1 = (tmp2 - d1_) / tmp;

    if (t1 > 0.0)
    {
      Eigen::Vector3d p1(origin + dirNorm * t1);
      Eigen::Vector3d v1(p1 - center_);
      v1 = v1 - normalH_.dot(v1) * normalH_;
      if (v1.squaredNorm() < radius2_ + detail::ZERO)
      {
        if (intersections == nullptr)
          return true;

        detail::intersc ip(p1, t1);
        ipts.push_back(ip);
      }
    }

    double t2 = (tmp2 - d2_) / tmp;
    if (t2 > 0.0)
    {
      Eigen::Vector3d p2(origin + dirNorm * t2);
      Eigen::Vector3d v2(p2 - center_);
      v2 = v2 - normalH_.dot(v2) * normalH_;
      if (v2.squaredNorm() < radius2_ + detail::ZERO)
      {
        if (intersections == nullptr)
          return true;

        detail::intersc ip(p2, t2);
        ipts.push_back(ip);
      }
    }
  }

  if (ipts.size() < 2)
  {
    // intersect with infinite cylinder
    Eigen::Vector3d VD(normalH_.cross(dirNorm));
    Eigen::Vector3d ROD(normalH_.cross(origin - center_));
    double a = VD.squaredNorm();
    double b = 2.0 * ROD.dot(VD);
    double c = ROD.squaredNorm() - radius2_;
    double d = b * b - 4.0 * a * c;
    if (d >= 0.0 && fabs(a) > detail::ZERO)
    {
      d = sqrt(d);
      double e = -a * 2.0;
      double t1 = (b + d) / e;
      double t2 = (b - d) / e;

      if (t1 > 0.0)
      {
        Eigen::Vector3d p1(origin + dirNorm * t1);
        Eigen::Vector3d v1(center_ - p1);

        if (fabs(normalH_.dot(v1)) < length2_ + detail::ZERO)
        {
          if (intersections == nullptr)
            return true;

          detail::intersc ip(p1, t1);
          ipts.push_back(ip);
        }
      }

      if (t2 > 0.0)
      {
        Eigen::Vector3d p2(origin + dirNorm * t2);
        Eigen::Vector3d v2(center_ - p2);

        if (fabs(normalH_.dot(v2)) < length2_ + detail::ZERO)
        {
          if (intersections == nullptr)
            return true;
          detail::intersc ip(p2, t2);
          ipts.push_back(ip);
        }
      }
    }
  }

  if (ipts.empty())
    return false;

  // If a ray hits exactly the boundary between a side and a base, it is reported twice.
  // We want to only return the intersection once, thus we need to filter them.
  detail::filterIntersections(ipts, intersections, count);
  return true;
}

bodies::Cylinder::Cylinder(const bodies::BoundingCylinder& cylinder) : Body()
{
  type_ = shapes::CYLINDER;
  shapes::Cylinder shape(cylinder.radius, cylinder.length);
  setDimensionsDirty(&shape);
  setPose(cylinder.pose);
}

bool bodies::Box::samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int /* max_attempts */,
                                    Eigen::Vector3d& result) const
{
  result = pose_ * Eigen::Vector3d(rng.uniformReal(-length2_, length2_), rng.uniformReal(-width2_, width2_),
                                   rng.uniformReal(-height2_, height2_));
  return true;
}

bool bodies::Box::containsPoint(const Eigen::Vector3d& p, bool /* verbose */) const
{
  const Eigen::Vector3d aligned = (invRot_ * (p - center_)).cwiseAbs();
  return aligned[0] <= length2_ && aligned[1] <= width2_ && aligned[2] <= height2_;
}

void bodies::Box::useDimensions(const shapes::Shape* shape)  // (x, y, z) = (length, width, height)
{
  const double* size = static_cast<const shapes::Box*>(shape)->size;
  length_ = size[0];
  width_ = size[1];
  height_ = size[2];
}

std::vector<double> bodies::Box::getDimensions() const
{
  return { length_, width_, height_ };
}

std::vector<double> bodies::Box::getScaledDimensions() const
{
  return { 2 * length2_, 2 * width2_, 2 * height2_ };
}

void bodies::Box::updateInternalData()
{
  double s2 = scale_ / 2.0;
  const auto tmpLength2 = length_ * s2 + padding_;
  const auto tmpWidth2 = width_ * s2 + padding_;
  const auto tmpHeight2 = height_ * s2 + padding_;

  if (tmpLength2 < 0 || tmpWidth2 < 0 || tmpHeight2 < 0)
    throw std::runtime_error("Box dimensions must be non-negative.");

  length2_ = tmpLength2;
  width2_ = tmpWidth2;
  height2_ = tmpHeight2;

  center_ = pose_.translation();

  radius2_ = length2_ * length2_ + width2_ * width2_ + height2_ * height2_;
  radiusB_ = sqrt(radius2_);

  ASSERT_ISOMETRY(pose_);
  invRot_ = pose_.linear().transpose();

  // rotation is intentionally not applied, the corners are used in intersectsRay()
  const Eigen::Vector3d tmp(length2_, width2_, height2_);
  minCorner_ = center_ - tmp;
  maxCorner_ = center_ + tmp;
}

std::shared_ptr<bodies::Body> bodies::Box::cloneAt(const Eigen::Isometry3d& pose, double padding, double scale) const
{
  auto b = std::allocate_shared<Box>(Eigen::aligned_allocator<Box>());
  b->length_ = length_;
  b->width_ = width_;
  b->height_ = height_;
  b->padding_ = padding;
  b->scale_ = scale;
  b->pose_ = pose;
  b->updateInternalData();
  return b;
}

double bodies::Box::computeVolume() const
{
  return 8.0 * length2_ * width2_ * height2_;
}

void bodies::Box::computeBoundingSphere(BoundingSphere& sphere) const
{
  sphere.center = center_;
  sphere.radius = radiusB_;
}

void bodies::Box::computeBoundingCylinder(BoundingCylinder& cylinder) const
{
  double a, b;

  if (length2_ > width2_ && length2_ > height2_)
  {
    cylinder.length = length2_ * 2.0;
    a = width2_;
    b = height2_;
    Eigen::Isometry3d rot(Eigen::AngleAxisd(90.0f * (M_PI / 180.0f), Eigen::Vector3d::UnitY()));
    cylinder.pose = pose_ * rot;
  }
  else if (width2_ > height2_)
  {
    cylinder.length = width2_ * 2.0;
    a = height2_;
    b = length2_;
    cylinder.radius = sqrt(height2_ * height2_ + length2_ * length2_);
    Eigen::Isometry3d rot(Eigen::AngleAxisd(90.0f * (M_PI / 180.0f), Eigen::Vector3d::UnitX()));
    cylinder.pose = pose_ * rot;
  }
  else
  {
    cylinder.length = height2_ * 2.0;
    a = width2_;
    b = length2_;
    cylinder.pose = pose_;
  }
  cylinder.radius = sqrt(a * a + b * b);
}

void bodies::Box::computeBoundingBox(bodies::AABB& bbox) const
{
  bbox.setEmpty();

  bbox.extendWithTransformedBox(getPose(), 2 * Eigen::Vector3d(length2_, width2_, height2_));
}

void bodies::Box::computeBoundingBox(bodies::OBB& bbox) const
{
  bbox.setPoseAndExtents(getPose(), 2 * Eigen::Vector3d(length2_, width2_, height2_));
}

bool bodies::Box::intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                                EigenSTL::vector_Vector3d* intersections, unsigned int count) const
{
  // this is faster than always calling dir.normalized() in case the vector is already unit
  const Eigen::Vector3d dirNorm = normalize(dir);

  // Brian Smits. Efficient bounding box intersection. Ray tracing news 15(1), 2002

  // The implemented method only works for axis-aligned boxes. So we treat ours as such, cancel its rotation, and
  // rotate the origin and dir instead. minCorner_ and maxCorner_ are corners with canceled rotation.
  const Eigen::Vector3d o(invRot_ * (origin - center_) + center_);
  const Eigen::Vector3d d(invRot_ * dirNorm);

  Eigen::Vector3d tmpTmin, tmpTmax;
  tmpTmin = (minCorner_ - o).cwiseQuotient(d);
  tmpTmax = (maxCorner_ - o).cwiseQuotient(d);

  // In projection to each axis, if the ray has positive direction, it goes from min corner (minCorner_) to max corner
  // (maxCorner_). If its direction is negative, the first intersection is at max corner and then at min corner.
  for (size_t i = 0; i < 3; ++i)
  {
    if (d[i] < 0)
      std::swap(tmpTmin[i], tmpTmax[i]);
  }

  // tmin and tmax are such values of t in "p = o + t * d" in which the line intersects the box faces.
  // The box is viewed projected from all three directions, values of t are computed for each of the projections,
  // and a final constraint on tmin and tmax is updated by each of these projections. If tmin > tmax, there is no
  // intersection between the line and the box.

  double tmin, tmax;
  // use fmax/fmin to handle NaNs which can sneak in when dividing by d in tmpTmin and tmpTmax
  tmin = std::fmax(tmpTmin.x(), std::fmax(tmpTmin.y(), tmpTmin.z()));
  tmax = std::fmin(tmpTmax.x(), std::fmin(tmpTmax.y(), tmpTmax.z()));

  // tmin > tmax, there is no intersection between the line and the box
  if (tmax - tmin < -detail::ZERO)
    return false;

  // As we're doing intersections with a ray and not a line, cases where tmax is negative mean that the intersection is
  // with the opposite ray and not the one we are working with.
  if (tmax < 0)
    return false;

  if (intersections)
  {
    if (tmax - tmin > detail::ZERO)
    {
      // tmax > tmin, we have two distinct intersection points
      if (tmin > detail::ZERO)
      {
        // tmin > 0, both intersections lie on the ray
        intersections->push_back(tmin * dirNorm + origin);
        if (count == 0 || count > 1)
          intersections->push_back(tmax * dirNorm + origin);
      }
      else
      {
        // tmin <= 0 && tmax >= 0, the first intersection point is on the opposite ray and the second one on the correct
        // ray - this means origin of the ray lies inside the box and we should only report one intersection.
        intersections->push_back(tmax * dirNorm + origin);
      }
    }
    else
    {
      // tmax == tmin, there is exactly one intersection at a corner or edge
      intersections->push_back(tmax * dirNorm + origin);
    }
  }

  return true;
}

bodies::Box::Box(const bodies::AABB& aabb) : Body()
{
  type_ = shapes::BOX;
  shapes::Box shape(aabb.sizes()[0], aabb.sizes()[1], aabb.sizes()[2]);
  setDimensionsDirty(&shape);

  Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  pose.translation() = aabb.center();
  setPose(pose);
}

namespace bodies
{
struct ConvexMesh::MeshData
{
  EigenSTL::vector_Vector4d planes_;
  EigenSTL::vector_Vector3d vertices_;
  std::vector<unsigned int> triangles_;
  std::map<unsigned int, unsigned int> plane_for_triangle_;
  std::map<unsigned int, unsigned int> triangle_for_plane_;
  Eigen::Vector3d mesh_center_;
  double mesh_radiusB_;
  Eigen::Vector3d box_offset_;
  Eigen::Vector3d box_size_;
  BoundingCylinder bounding_cylinder_;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
}  // namespace bodies

bool bodies::ConvexMesh::containsPoint(const Eigen::Vector3d& p, bool /* verbose */) const
{
  if (!mesh_data_)
    return false;
  if (bounding_box_.containsPoint(p))
  {
    // Transform the point to the "base space" of this mesh
    Eigen::Vector3d ip(i_pose_ * p);
    return isPointInsidePlanes(ip);
  }
  else
    return false;
}

void bodies::ConvexMesh::correctVertexOrderFromPlanes()
{
  for (unsigned int i = 0; i < mesh_data_->triangles_.size(); i += 3)
  {
    Eigen::Vector3d d1 =
        mesh_data_->vertices_[mesh_data_->triangles_[i]] - mesh_data_->vertices_[mesh_data_->triangles_[i + 1]];
    Eigen::Vector3d d2 =
        mesh_data_->vertices_[mesh_data_->triangles_[i]] - mesh_data_->vertices_[mesh_data_->triangles_[i + 2]];
    // expected computed normal from triangle vertex order
    Eigen::Vector3d tri_normal = d1.cross(d2);
    tri_normal.normalize();
    // actual plane normal
    Eigen::Vector3d normal(mesh_data_->planes_[mesh_data_->plane_for_triangle_[i / 3]].x(),
                           mesh_data_->planes_[mesh_data_->plane_for_triangle_[i / 3]].y(),
                           mesh_data_->planes_[mesh_data_->plane_for_triangle_[i / 3]].z());
    bool same_dir = tri_normal.dot(normal) > 0;
    if (!same_dir)
    {
      std::swap(mesh_data_->triangles_[i], mesh_data_->triangles_[i + 1]);
    }
  }
}

void bodies::ConvexMesh::useDimensions(const shapes::Shape* shape)
{
  mesh_data_ = std::allocate_shared<MeshData>(Eigen::aligned_allocator<MeshData>());
  const shapes::Mesh* mesh = static_cast<const shapes::Mesh*>(shape);

  double maxX = -std::numeric_limits<double>::infinity(), maxY = -std::numeric_limits<double>::infinity(),
         maxZ = -std::numeric_limits<double>::infinity();
  double minX = std::numeric_limits<double>::infinity(), minY = std::numeric_limits<double>::infinity(),
         minZ = std::numeric_limits<double>::infinity();

  for (unsigned int i = 0; i < mesh->vertex_count; ++i)
  {
    double vx = mesh->vertices[3 * i];
    double vy = mesh->vertices[3 * i + 1];
    double vz = mesh->vertices[3 * i + 2];

    if (maxX < vx)
      maxX = vx;
    if (maxY < vy)
      maxY = vy;
    if (maxZ < vz)
      maxZ = vz;

    if (minX > vx)
      minX = vx;
    if (minY > vy)
      minY = vy;
    if (minZ > vz)
      minZ = vz;
  }

  if (maxX < minX)
    maxX = minX = 0.0;
  if (maxY < minY)
    maxY = minY = 0.0;
  if (maxZ < minZ)
    maxZ = minZ = 0.0;

  mesh_data_->box_size_ = Eigen::Vector3d(maxX - minX, maxY - minY, maxZ - minZ);

  mesh_data_->box_offset_ = Eigen::Vector3d((minX + maxX) / 2.0, (minY + maxY) / 2.0, (minZ + maxZ) / 2.0);

  mesh_data_->planes_.clear();
  mesh_data_->triangles_.clear();
  mesh_data_->vertices_.clear();
  mesh_data_->mesh_radiusB_ = 0.0;
  mesh_data_->mesh_center_ = Eigen::Vector3d::Zero();

  double xdim = maxX - minX;
  double ydim = maxY - minY;
  double zdim = maxZ - minZ;

  double pose1;
  double pose2;

  unsigned int off1;
  unsigned int off2;

  /* compute bounding cylinder */
  double cyl_length;
  double maxdist = -std::numeric_limits<double>::infinity();
  if (xdim > ydim && xdim > zdim)
  {
    off1 = 1;
    off2 = 2;
    pose1 = mesh_data_->box_offset_.y();
    pose2 = mesh_data_->box_offset_.z();
    cyl_length = xdim;
  }
  else if (ydim > zdim)
  {
    off1 = 0;
    off2 = 2;
    pose1 = mesh_data_->box_offset_.x();
    pose2 = mesh_data_->box_offset_.z();
    cyl_length = ydim;
  }
  else
  {
    off1 = 0;
    off2 = 1;
    pose1 = mesh_data_->box_offset_.x();
    pose2 = mesh_data_->box_offset_.y();
    cyl_length = zdim;
  }

  /* compute convex hull */
  coordT* points = (coordT*)calloc(mesh->vertex_count * 3, sizeof(coordT));
  for (unsigned int i = 0; i < mesh->vertex_count; ++i)
  {
    points[3 * i + 0] = (coordT)mesh->vertices[3 * i + 0];
    points[3 * i + 1] = (coordT)mesh->vertices[3 * i + 1];
    points[3 * i + 2] = (coordT)mesh->vertices[3 * i + 2];

    double dista = mesh->vertices[3 * i + off1] - pose1;
    double distb = mesh->vertices[3 * i + off2] - pose2;
    double dist = sqrt(((dista * dista) + (distb * distb)));
    if (dist > maxdist)
      maxdist = dist;
  }
  mesh_data_->bounding_cylinder_.radius = maxdist;
  mesh_data_->bounding_cylinder_.length = cyl_length;

  static FILE* null = fopen("/dev/null", "w");

  char flags[] = "qhull Tv Qt";
  qhT qh_qh;
  qhT* qh = &qh_qh;
  QHULL_LIB_CHECK
  qh_zero(qh, null);
  int exitcode = qh_new_qhull(qh, 3, mesh->vertex_count, points, true, flags, null, null);

  if (exitcode != 0)
  {
    CONSOLE_BRIDGE_logWarn("Convex hull creation failed");
    qh_freeqhull(qh, !qh_ALL);
    int curlong, totlong;
    qh_memfreeshort(qh, &curlong, &totlong);
    return;
  }

  int num_facets = qh->num_facets;

  int num_vertices = qh->num_vertices;
  mesh_data_->vertices_.reserve(num_vertices);
  Eigen::Vector3d sum(0, 0, 0);

  // necessary for FORALLvertices
  std::map<unsigned int, unsigned int> qhull_vertex_table;
  vertexT* vertex;
  FORALLvertices
  {
    Eigen::Vector3d vert(vertex->point[0], vertex->point[1], vertex->point[2]);
    qhull_vertex_table[vertex->id] = mesh_data_->vertices_.size();
    sum += vert;
    mesh_data_->vertices_.push_back(vert);
  }

  mesh_data_->mesh_center_ = sum / (double)(num_vertices);
  for (unsigned int j = 0; j < mesh_data_->vertices_.size(); ++j)
  {
    double dist = (mesh_data_->vertices_[j] - mesh_data_->mesh_center_).squaredNorm();
    if (dist > mesh_data_->mesh_radiusB_)
      mesh_data_->mesh_radiusB_ = dist;
  }

  mesh_data_->mesh_radiusB_ = sqrt(mesh_data_->mesh_radiusB_);
  mesh_data_->triangles_.reserve(num_facets);

  // neccessary for qhull macro
  facetT* facet;
  FORALLfacets
  {
    Eigen::Vector4d planeEquation(facet->normal[0], facet->normal[1], facet->normal[2], facet->offset);
    if (!mesh_data_->planes_.empty())
    {
      // filter equal planes - assuming same ones follow each other
      if ((planeEquation - mesh_data_->planes_.back()).cwiseAbs().maxCoeff() > 1e-6)  // max diff to last
        mesh_data_->planes_.push_back(planeEquation);
    }
    else
    {
      mesh_data_->planes_.push_back(planeEquation);
    }

    // Needed by FOREACHvertex_i_
    int vertex_n, vertex_i;
    FOREACHvertex_i_(qh, (*facet).vertices)
    {
      mesh_data_->triangles_.push_back(qhull_vertex_table[vertex->id]);
    }

    mesh_data_->plane_for_triangle_[(mesh_data_->triangles_.size() - 1) / 3] = mesh_data_->planes_.size() - 1;
    mesh_data_->triangle_for_plane_[mesh_data_->planes_.size() - 1] = (mesh_data_->triangles_.size() - 1) / 3;
  }
  qh_freeqhull(qh, !qh_ALL);
  int curlong, totlong;
  qh_memfreeshort(qh, &curlong, &totlong);
}

std::vector<double> bodies::ConvexMesh::getDimensions() const
{
  return {};
}

std::vector<double> bodies::ConvexMesh::getScaledDimensions() const
{
  return {};
}

void bodies::ConvexMesh::computeScaledVerticesFromPlaneProjections()
{
  // compute the scaled vertices, if needed
  if (padding_ == 0.0 && scale_ == 1.0)
  {
    scaled_vertices_ = &mesh_data_->vertices_;
    return;
  }

  if (!scaled_vertices_storage_)
    scaled_vertices_storage_.reset(new EigenSTL::vector_Vector3d());
  scaled_vertices_ = scaled_vertices_storage_.get();
  scaled_vertices_storage_->resize(mesh_data_->vertices_.size());
  // project vertices along the vertex - center line to the scaled and padded plane
  // take the average of all tri's planes around that vertex as the result
  // is not unique

  // First figure out, which tris are connected to each vertex
  std::map<unsigned int, std::vector<unsigned int>> vertex_to_tris;
  for (unsigned int i = 0; i < mesh_data_->triangles_.size() / 3; ++i)
  {
    vertex_to_tris[mesh_data_->triangles_[3 * i + 0]].push_back(i);
    vertex_to_tris[mesh_data_->triangles_[3 * i + 1]].push_back(i);
    vertex_to_tris[mesh_data_->triangles_[3 * i + 2]].push_back(i);
  }

  for (unsigned int i = 0; i < mesh_data_->vertices_.size(); ++i)
  {
    Eigen::Vector3d v(mesh_data_->vertices_[i] - mesh_data_->mesh_center_);
    EigenSTL::vector_Vector3d projected_vertices;
    for (unsigned int t : vertex_to_tris[i])
    {
      const Eigen::Vector4d& plane = mesh_data_->planes_[mesh_data_->plane_for_triangle_[t]];
      Eigen::Vector3d plane_normal(plane.x(), plane.y(), plane.z());
      double d_scaled_padded =
          scale_ * plane.w() - (1 - scale_) * mesh_data_->mesh_center_.dot(plane_normal) - padding_;

      // intersect vert - center with scaled/padded plane equation
      double denom = v.dot(plane_normal);
      if (fabs(denom) < 1e-3)
        continue;
      double lambda = (-mesh_data_->mesh_center_.dot(plane_normal) - d_scaled_padded) / denom;
      Eigen::Vector3d vert_on_plane = v * lambda + mesh_data_->mesh_center_;
      projected_vertices.push_back(vert_on_plane);
    }
    if (projected_vertices.empty())
    {
      double l = v.norm();
      scaled_vertices_storage_->at(i) =
          mesh_data_->mesh_center_ + v * (scale_ + (l > detail::ZERO ? padding_ / l : 0.0));
    }
    else
    {
      Eigen::Vector3d sum(0, 0, 0);
      for (const Eigen::Vector3d& vertex : projected_vertices)
      {
        sum += vertex;
      }
      sum /= projected_vertices.size();
      scaled_vertices_storage_->at(i) = sum;
    }
  }
}

void bodies::ConvexMesh::updateInternalData()
{
  if (!mesh_data_)
    return;
  Eigen::Isometry3d pose = pose_;
  pose.translation() = Eigen::Vector3d(pose_ * mesh_data_->box_offset_);

  shapes::Box box_shape(mesh_data_->box_size_.x(), mesh_data_->box_size_.y(), mesh_data_->box_size_.z());
  bounding_box_.setPoseDirty(pose);
  // The real effect of padding will most likely be smaller due to the mesh padding algorithm, but in "worst case" it
  // can inflate the primitive bounding box by the padding_ value.
  bounding_box_.setPaddingDirty(padding_);
  bounding_box_.setScaleDirty(scale_);
  bounding_box_.setDimensionsDirty(&box_shape);
  bounding_box_.updateInternalData();

  i_pose_ = pose_.inverse();
  center_ = pose_ * mesh_data_->mesh_center_;
  radiusB_ = mesh_data_->mesh_radiusB_ * scale_ + padding_;
  radiusBSqr_ = radiusB_ * radiusB_;

  // compute the scaled vertices, if needed
  if (padding_ == 0.0 && scale_ == 1.0)
    scaled_vertices_ = &mesh_data_->vertices_;
  else
  {
    if (!scaled_vertices_storage_)
      scaled_vertices_storage_.reset(new EigenSTL::vector_Vector3d());
    scaled_vertices_ = scaled_vertices_storage_.get();
    scaled_vertices_storage_->resize(mesh_data_->vertices_.size());
    for (unsigned int i = 0; i < mesh_data_->vertices_.size(); ++i)
    {
      Eigen::Vector3d v(mesh_data_->vertices_[i] - mesh_data_->mesh_center_);
      double l = v.norm();
      scaled_vertices_storage_->at(i) =
          mesh_data_->mesh_center_ + v * (scale_ + (l > detail::ZERO ? padding_ / l : 0.0));
    }
  }
}
const std::vector<unsigned int>& bodies::ConvexMesh::getTriangles() const
{
  static const std::vector<unsigned int> empty;
  return mesh_data_ ? mesh_data_->triangles_ : empty;
}

const EigenSTL::vector_Vector3d& bodies::ConvexMesh::getVertices() const
{
  static const EigenSTL::vector_Vector3d empty;
  return mesh_data_ ? mesh_data_->vertices_ : empty;
}

const EigenSTL::vector_Vector3d& bodies::ConvexMesh::getScaledVertices() const
{
  return scaled_vertices_ ? *scaled_vertices_ : getVertices();
}

const EigenSTL::vector_Vector4d& bodies::ConvexMesh::getPlanes() const
{
  static const EigenSTL::vector_Vector4d empty;
  return mesh_data_ ? mesh_data_->planes_ : empty;
}

std::shared_ptr<bodies::Body> bodies::ConvexMesh::cloneAt(const Eigen::Isometry3d& pose, double padding,
                                                          double scale) const
{
  auto m = std::allocate_shared<ConvexMesh>(Eigen::aligned_allocator<ConvexMesh>());
  m->mesh_data_ = mesh_data_;
  m->padding_ = padding;
  m->scale_ = scale;
  m->pose_ = pose;
  m->updateInternalData();
  return m;
}

void bodies::ConvexMesh::computeBoundingSphere(BoundingSphere& sphere) const
{
  sphere.center = center_;
  sphere.radius = radiusB_;
}

void bodies::ConvexMesh::computeBoundingCylinder(BoundingCylinder& cylinder) const
{
  // the padding contibution might be smaller in reality, but we want to get it right for the worst case
  cylinder.length = mesh_data_ ? mesh_data_->bounding_cylinder_.length * scale_ + 2 * padding_ : 0.0;
  cylinder.radius = mesh_data_ ? mesh_data_->bounding_cylinder_.radius * scale_ + padding_ : 0.0;
  // need to do rotation correctly to get pose, which bounding box does
  BoundingCylinder cyl;
  bounding_box_.computeBoundingCylinder(cyl);
  cylinder.pose = cyl.pose;
}

void bodies::ConvexMesh::computeBoundingBox(bodies::AABB& bbox) const
{
  bbox.setEmpty();

  bounding_box_.computeBoundingBox(bbox);
}

void bodies::ConvexMesh::computeBoundingBox(bodies::OBB& bbox) const
{
  bounding_box_.computeBoundingBox(bbox);
}

bool bodies::ConvexMesh::isPointInsidePlanes(const Eigen::Vector3d& point) const
{
  unsigned int numplanes = mesh_data_->planes_.size();
  for (unsigned int i = 0; i < numplanes; ++i)
  {
    const Eigen::Vector4d& plane = mesh_data_->planes_[i];
    Eigen::Vector3d plane_vec(plane.x(), plane.y(), plane.z());
    // w() needs to be recomputed from a scaled vertex as normally it refers to the unscaled plane
    // we also cannot simply subtract padding_ from it, because padding of the points on the plane causes a different
    // effect than adding padding along this plane's normal (padding effect is direction-dependent)
    const auto scaled_point_on_plane =
        scaled_vertices_->at(mesh_data_->triangles_[3 * mesh_data_->triangle_for_plane_[i]]);
    const double w_scaled_padded = -plane_vec.dot(scaled_point_on_plane);
    const double dist = plane_vec.dot(point) + w_scaled_padded - detail::ZERO;
    if (dist > 0.0)
      return false;
  }
  return true;
}

unsigned int bodies::ConvexMesh::countVerticesBehindPlane(const Eigen::Vector4f& planeNormal) const
{
  unsigned int numvertices = mesh_data_->vertices_.size();
  unsigned int result = 0;
  for (unsigned int i = 0; i < numvertices; ++i)
  {
    Eigen::Vector3d plane_vec(planeNormal.x(), planeNormal.y(), planeNormal.z());
    double dist = plane_vec.dot(mesh_data_->vertices_[i]) + planeNormal.w() - 1e-6;
    if (dist > 0.0)
      result++;
  }
  return result;
}

double bodies::ConvexMesh::computeVolume() const
{
  double volume = 0.0;
  if (mesh_data_)
    for (unsigned int i = 0; i < mesh_data_->triangles_.size() / 3; ++i)
    {
      const Eigen::Vector3d& v1 = mesh_data_->vertices_[mesh_data_->triangles_[3 * i + 0]];
      const Eigen::Vector3d& v2 = mesh_data_->vertices_[mesh_data_->triangles_[3 * i + 1]];
      const Eigen::Vector3d& v3 = mesh_data_->vertices_[mesh_data_->triangles_[3 * i + 2]];
      volume += v1.x() * v2.y() * v3.z() + v2.x() * v3.y() * v1.z() + v3.x() * v1.y() * v2.z() -
                v1.x() * v3.y() * v2.z() - v2.x() * v1.y() * v3.z() - v3.x() * v2.y() * v1.z();
    }
  return fabs(volume) / 6.0;
}

bool bodies::ConvexMesh::intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                                       EigenSTL::vector_Vector3d* intersections, unsigned int count) const
{
  // this is faster than always calling dir.normalized() in case the vector is already unit
  const Eigen::Vector3d dirNorm = normalize(dir);

  if (!mesh_data_)
    return false;
  if (detail::distanceSQR(center_, origin, dirNorm) > radiusBSqr_)
    return false;
  if (!bounding_box_.intersectsRay(origin, dirNorm))
    return false;

  // transform the ray into the coordinate frame of the mesh
  Eigen::Vector3d orig(i_pose_ * origin);
  Eigen::Vector3d dr(i_pose_.linear() * dirNorm);

  std::vector<detail::intersc> ipts;

  bool result = false;

  // for each triangle
  const auto nt = mesh_data_->triangles_.size() / 3;
  for (size_t i = 0; i < nt; ++i)
  {
    Eigen::Vector3d vec(mesh_data_->planes_[mesh_data_->plane_for_triangle_[i]].x(),
                        mesh_data_->planes_[mesh_data_->plane_for_triangle_[i]].y(),
                        mesh_data_->planes_[mesh_data_->plane_for_triangle_[i]].z());

    const double tmp = vec.dot(dr);
    if (fabs(tmp) > detail::ZERO)
    {
      // planes_[...].w() corresponds to the unscaled mesh, so we need to compute it ourselves
      const double w_scaled_padded = vec.dot(scaled_vertices_->at(mesh_data_->triangles_[3 * i]));
      const double t = -(vec.dot(orig) + w_scaled_padded) / tmp;
      if (t > 0.0)
      {
        const auto i3 = 3 * i;
        const auto v1 = mesh_data_->triangles_[i3 + 0];
        const auto v2 = mesh_data_->triangles_[i3 + 1];
        const auto v3 = mesh_data_->triangles_[i3 + 2];

        const Eigen::Vector3d& a = scaled_vertices_->at(v1);
        const Eigen::Vector3d& b = scaled_vertices_->at(v2);
        const Eigen::Vector3d& c = scaled_vertices_->at(v3);

        Eigen::Vector3d cb(c - b);
        Eigen::Vector3d ab(a - b);

        // intersection of the plane defined by the triangle and the ray
        Eigen::Vector3d P(orig + dr * t);

        // check if it is inside the triangle
        Eigen::Vector3d pb(P - b);
        Eigen::Vector3d c1(cb.cross(pb));
        Eigen::Vector3d c2(cb.cross(ab));
        if (c1.dot(c2) < 0.0)
          continue;

        Eigen::Vector3d ca(c - a);
        Eigen::Vector3d pa(P - a);
        Eigen::Vector3d ba(-ab);

        c1 = ca.cross(pa);
        c2 = ca.cross(ba);
        if (c1.dot(c2) < 0.0)
          continue;

        c1 = ba.cross(pa);
        c2 = ba.cross(ca);

        if (c1.dot(c2) < 0.0)
          continue;

        result = true;
        if (intersections)
        {
          detail::intersc ip(origin + dirNorm * t, t);
          ipts.push_back(ip);
        }
        else
          break;
      }
    }
  }

  if (result && intersections)
  {
    // If a ray hits exactly the boundary between two triangles, it is reported twice;
    // We only want return the intersection once; thus we need to filter them.
    detail::filterIntersections(ipts, intersections, count);
  }

  return result;
}

bodies::BodyVector::BodyVector()
{
}

bodies::BodyVector::BodyVector(const std::vector<shapes::Shape*>& shapes, const EigenSTL::vector_Isometry3d& poses,
                               double padding)
{
  for (unsigned int i = 0; i < shapes.size(); i++)
    addBody(shapes[i], poses[i], padding);
}

bodies::BodyVector::~BodyVector()
{
  clear();
}

void bodies::BodyVector::clear()
{
  for (auto& body : bodies_)
    delete body;
  bodies_.clear();
}

void bodies::BodyVector::addBody(Body* body)
{
  bodies_.push_back(body);
  BoundingSphere sphere;
  body->computeBoundingSphere(sphere);
}

void bodies::BodyVector::addBody(const shapes::Shape* shape, const Eigen::Isometry3d& pose, double padding)
{
  bodies::Body* body = bodies::createBodyFromShape(shape);
  body->setPoseDirty(pose);
  body->setPaddingDirty(padding);
  body->updateInternalData();
  addBody(body);
}

std::size_t bodies::BodyVector::getCount() const
{
  return bodies_.size();
}

void bodies::BodyVector::setPose(unsigned int i, const Eigen::Isometry3d& pose)
{
  if (i >= bodies_.size())
  {
    CONSOLE_BRIDGE_logError("There is no body at index %u", i);
    return;
  }

  bodies_[i]->setPose(pose);
}

const bodies::Body* bodies::BodyVector::getBody(unsigned int i) const
{
  if (i >= bodies_.size())
  {
    CONSOLE_BRIDGE_logError("There is no body at index %u", i);
    return nullptr;
  }
  else
    return bodies_[i];
}

bool bodies::BodyVector::containsPoint(const Eigen::Vector3d& p, std::size_t& index, bool verbose) const
{
  for (std::size_t i = 0; i < bodies_.size(); ++i)
    if (bodies_[i]->containsPoint(p, verbose))
    {
      index = i;
      return true;
    }
  return false;
}

bool bodies::BodyVector::containsPoint(const Eigen::Vector3d& p, bool verbose) const
{
  std::size_t dummy;
  return containsPoint(p, dummy, verbose);
}

bool bodies::BodyVector::intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir, std::size_t& index,
                                       EigenSTL::vector_Vector3d* intersections, unsigned int count) const
{
  for (std::size_t i = 0; i < bodies_.size(); ++i)
    if (bodies_[i]->intersectsRay(origin, dir, intersections, count))
    {
      index = i;
      return true;
    }
  return false;
}
