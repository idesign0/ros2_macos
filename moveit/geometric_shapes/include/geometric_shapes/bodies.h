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

/* Author: Ioan Sucan, E. Gil Jones */

#ifndef GEOMETRIC_SHAPES_BODIES_
#define GEOMETRIC_SHAPES_BODIES_

#define _USE_MATH_DEFINES
#include "geometric_shapes/aabb.h"
#include "geometric_shapes/obb.h"
#include "geometric_shapes/shapes.h"
#include <eigen_stl_containers/eigen_stl_containers.h>
#include <random_numbers/random_numbers.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <memory>
#include <vector>

/** \brief This set of classes allows quickly detecting whether a given point
   is inside an object or not. This capability is useful when removing
   points from inside the robot (when the robot sees its arms, for
   example). */
namespace bodies
{
/** \brief Definition of a sphere that bounds another object */
struct BoundingSphere
{
  Eigen::Vector3d center;
  double radius;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** \brief Definition of a cylinder */
struct BoundingCylinder
{
  Eigen::Isometry3d pose;
  double radius;
  double length;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Body;

/** \brief Shared pointer to a Body */
typedef std::shared_ptr<Body> BodyPtr;

/** \brief Shared pointer to a const Body */
typedef std::shared_ptr<const Body> BodyConstPtr;

/** \brief A body is a shape + its pose. Point inclusion, ray
    intersection can be tested, volumes and bounding spheres can
    be computed.*/
class Body
{
public:
  Body() : scale_(1.0), padding_(0.0), type_(shapes::UNKNOWN_SHAPE)
  {
    pose_.setIdentity();
  }

  virtual ~Body() = default;

  /** \brief Get the type of shape this body represents */
  inline shapes::ShapeType getType() const
  {
    return type_;
  }

  /**
   * \brief If the dimension of the body should be scaled, this method sets the scale.
   * \note This is the dirty version of the function which does not update internal data that depend on the scale.
   *       In the general case, you should call setScale() instead. Only call this function if you have a series of
   *       calls like setScale/setPadding/setPose/setDimensions and you want to avoid the overhead of updating the
   *       internal structures after each call. When you are finished with the batch, call updateInternalData().
   * \param scale The scale to set. 1.0 means no scaling.
   */
  inline void setScaleDirty(double scale)
  {
    scale_ = scale;
  }

  /** \brief If the dimension of the body should be scaled, this
      method sets the scale. Default is 1.0 */
  inline void setScale(double scale)
  {
    setScaleDirty(scale);
    updateInternalData();
  }

  /** \brief Retrieve the current scale */
  inline double getScale() const
  {
    return scale_;
  }

  /**
   * \brief If the dimension of the body should be padded, this method sets the pading.
   * \note This is the dirty version of the function which does not update internal data that depend on the scale.
   *       In the general case, you should call setPadding() instead. Only call this function if you have a series of
   *       calls like setScale/setPadding/setPose/setDimensions and you want to avoid the overhead of updating the
   *       internal structures after each call. When you are finished with the batch, call updateInternalData().
   * \param padd The padding to set (in meters). 0.0 means no padding.
   */
  inline void setPaddingDirty(double padd)
  {
    padding_ = padd;
  }

  /** \brief If constant padding should be added to the body, this
      method sets the padding. Default is 0.0 */
  inline void setPadding(double padd)
  {
    setPaddingDirty(padd);
    updateInternalData();
  }

  /** \brief Retrieve the current padding */
  inline double getPadding() const
  {
    return padding_;
  }

  /**
   * \brief Set the pose of the body.
   * \note This is the dirty version of the function which does not update internal data that depend on the pose.
   *       In the general case, you should call setPose() instead. Only call this function if you have a series of
   *       calls like setScale/setPadding/setPose/setDimensions and you want to avoid the overhead of updating the
   *       internal structures after each call. When you are finished with the batch, call updateInternalData().
   * \param pose The pose to set. Default is identity.
   */
  inline void setPoseDirty(const Eigen::Isometry3d& pose)
  {
    pose_ = pose;
  }

  /** \brief Set the pose of the body. Default is identity */
  inline void setPose(const Eigen::Isometry3d& pose)
  {
    setPoseDirty(pose);
    updateInternalData();
  }

  /** \brief Retrieve the pose of the body */
  inline const Eigen::Isometry3d& getPose() const
  {
    return pose_;
  }

  /**
   * \brief Set the dimensions of the body (from corresponding shape).
   * \note This is the dirty version of the function which does not update internal data that depend on the dimensions.
   *       In the general case, you should call setDimensions() instead. Only call this function if you have a series of
   *       calls like setScale/setPadding/setPose/setDimensions and you want to avoid the overhead of updating the
   *       internal structures after each call. When you are finished with the batch, call updateInternalData().
   * \param shape The shape whose dimensions should be assumed. After the function finishes, the pointer can be deleted.
   */
  inline void setDimensionsDirty(const shapes::Shape* shape)
  {
    useDimensions(shape);
  }

  /** \brief Get the dimensions associated to this body (as read from corresponding shape) */
  virtual std::vector<double> getDimensions() const = 0;

  /** \brief Get the dimensions associated to this body (scaled and padded) */
  virtual std::vector<double> getScaledDimensions() const = 0;

  /** \brief Set the dimensions of the body (from corresponding shape) */
  inline void setDimensions(const shapes::Shape* shape)
  {
    setDimensionsDirty(shape);
    updateInternalData();
  }

  /** \brief Check if a point is inside the body */
  inline bool containsPoint(double x, double y, double z, bool verbose = false) const
  {
    Eigen::Vector3d pt(x, y, z);
    return containsPoint(pt, verbose);
  }

  /** \brief Check if a point is inside the body. Surface points are included. */
  virtual bool containsPoint(const Eigen::Vector3d& p, bool verbose = false) const = 0;

  /** \brief Check if a ray intersects the body, and find the
      set of intersections, in order, along the ray. A maximum
      number of intersections can be specified as well. If that
      number is 0, all intersections are returned.
      Passing dir as a unit vector will result in faster computation. */
  virtual bool intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                             EigenSTL::vector_Vector3d* intersections = nullptr, unsigned int count = 0) const = 0;

  /** \brief Compute the volume of the body. This method includes
      changes induced by scaling and padding */
  virtual double computeVolume() const = 0;

  /** \brief Sample a point that is included in the body using a given random number generator.

     Sometimes multiple attempts need to be generated.
     The function terminates with failure (returns false) after \e max_attempts attempts.
     If the call is successful (returns true) the point is written to \e result */
  virtual bool samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int max_attempts,
                                 Eigen::Vector3d& result) const;

  /** \brief Compute the bounding radius for the body, in its current
      pose. Scaling and padding are accounted for. */
  virtual void computeBoundingSphere(BoundingSphere& sphere) const = 0;

  /** \brief Compute the bounding cylinder for the body, in its current
      pose. Scaling and padding are accounted for. */
  virtual void computeBoundingCylinder(BoundingCylinder& cylinder) const = 0;

  /** \brief Compute the axis-aligned bounding box for the body, in its current
      pose. Scaling and padding are accounted for. */
  virtual void computeBoundingBox(AABB& bbox) const = 0;

  /** \brief Compute the oriented bounding box for the body, in its current
     pose. Scaling and padding are accounted for. */
  virtual void computeBoundingBox(OBB& bbox) const = 0;

  /** \brief Get a clone of this body, but one that is located at the pose \e pose */
  inline BodyPtr cloneAt(const Eigen::Isometry3d& pose) const
  {
    return cloneAt(pose, padding_, scale_);
  }

  /** \brief Get a clone of this body, but one that is located at the
      pose \e pose and has possibly different passing and scaling: \e
      padding and \e scaling. This function is useful to implement
      thread safety, when bodies need to be moved around. */
  virtual BodyPtr cloneAt(const Eigen::Isometry3d& pose, double padding, double scaling) const = 0;

  /** \brief This function is called every time a change to the body
      is made, so that intermediate values stored for efficiency
      reasons are kept up to date. */
  virtual void updateInternalData() = 0;

protected:
  /** \brief Depending on the shape, this function copies the relevant data to the body. */
  virtual void useDimensions(const shapes::Shape* shape) = 0;

  /** \brief The scale that was set for this body */
  double scale_;

  /** \brief The scale that was set for this body */
  double padding_;

  /** \brief The type of shape this body was constructed from */
  shapes::ShapeType type_;

  /** \brief The location of the body (position and orientation) */
  Eigen::Isometry3d pose_;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** \brief Definition of a sphere */
class Sphere : public Body
{
public:
  Sphere() : Body()
  {
    type_ = shapes::SPHERE;
  }

  Sphere(const shapes::Shape* shape) : Body()
  {
    type_ = shapes::SPHERE;
    setDimensions(shape);
  }

  explicit Sphere(const BoundingSphere& sphere);

  ~Sphere() override = default;

  /** \brief Get the radius of the sphere */
  std::vector<double> getDimensions() const override;
  std::vector<double> getScaledDimensions() const override;

  bool containsPoint(const Eigen::Vector3d& p, bool verbose = false) const override;
  double computeVolume() const override;
  bool samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int max_attempts,
                         Eigen::Vector3d& result) const override;
  void computeBoundingSphere(BoundingSphere& sphere) const override;
  void computeBoundingCylinder(BoundingCylinder& cylinder) const override;
  void computeBoundingBox(AABB& bbox) const override;
  void computeBoundingBox(OBB& bbox) const override;
  bool intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                     EigenSTL::vector_Vector3d* intersections = nullptr, unsigned int count = 0) const override;

  BodyPtr cloneAt(const Eigen::Isometry3d& pose, double padding, double scale) const override;

  void updateInternalData() override;

protected:
  void useDimensions(const shapes::Shape* shape) override;

  // shape-dependent data
  double radius_;

  // pose/padding/scaling-dependent values & values computed for convenience and fast upcoming computations
  Eigen::Vector3d center_;
  double radiusU_;
  double radius2_;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** \brief Definition of a cylinder */
class Cylinder : public Body
{
public:
  Cylinder() : Body()
  {
    type_ = shapes::CYLINDER;
  }

  Cylinder(const shapes::Shape* shape) : Body()
  {
    type_ = shapes::CYLINDER;
    setDimensions(shape);
  }

  explicit Cylinder(const BoundingCylinder& cylinder);

  ~Cylinder() override = default;

  /** \brief Get the radius & length of the cylinder */
  std::vector<double> getDimensions() const override;
  std::vector<double> getScaledDimensions() const override;

  bool containsPoint(const Eigen::Vector3d& p, bool verbose = false) const override;
  double computeVolume() const override;
  bool samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int max_attempts,
                         Eigen::Vector3d& result) const override;
  void computeBoundingSphere(BoundingSphere& sphere) const override;
  void computeBoundingCylinder(BoundingCylinder& cylinder) const override;
  void computeBoundingBox(AABB& bbox) const override;
  void computeBoundingBox(OBB& bbox) const override;
  bool intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                     EigenSTL::vector_Vector3d* intersections = nullptr, unsigned int count = 0) const override;

  BodyPtr cloneAt(const Eigen::Isometry3d& pose, double padding, double scale) const override;

  void updateInternalData() override;

protected:
  void useDimensions(const shapes::Shape* shape) override;

  // shape-dependent data
  double length_;
  double radius_;

  // pose/padding/scaling-dependent values & values computed for convenience and fast upcoming computations
  Eigen::Vector3d center_;
  Eigen::Vector3d normalH_;
  Eigen::Vector3d normalB1_;
  Eigen::Vector3d normalB2_;

  double length2_;
  double radiusU_;
  double radiusB_;
  double radiusBSqr_;
  double radius2_;
  double d1_;
  double d2_;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** \brief Definition of a box */
class Box : public Body
{
public:
  Box() : Body()
  {
    type_ = shapes::BOX;
  }

  Box(const shapes::Shape* shape) : Body()
  {
    type_ = shapes::BOX;
    setDimensions(shape);
  }

  explicit Box(const AABB& aabb);

  ~Box() override = default;

  /** \brief Get the length & width & height (x, y, z) of the box */
  std::vector<double> getDimensions() const override;
  std::vector<double> getScaledDimensions() const override;

  bool containsPoint(const Eigen::Vector3d& p, bool verbose = false) const override;
  double computeVolume() const override;
  bool samplePointInside(random_numbers::RandomNumberGenerator& rng, unsigned int max_attempts,
                         Eigen::Vector3d& result) const override;
  void computeBoundingSphere(BoundingSphere& sphere) const override;
  void computeBoundingCylinder(BoundingCylinder& cylinder) const override;
  void computeBoundingBox(AABB& bbox) const override;
  void computeBoundingBox(OBB& bbox) const override;
  bool intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                     EigenSTL::vector_Vector3d* intersections = nullptr, unsigned int count = 0) const override;

  BodyPtr cloneAt(const Eigen::Isometry3d& pose, double padding, double scale) const override;

  void updateInternalData() override;

protected:
  void useDimensions(const shapes::Shape* shape) override;  // (x, y, z) = (length, width, height)

  // shape-dependent data
  double length_;
  double width_;
  double height_;

  // pose/padding/scaling-dependent values & values computed for convenience and fast upcoming computations
  Eigen::Vector3d center_;
  Eigen::Matrix3d invRot_;

  Eigen::Vector3d minCorner_;  //!< The translated, but not rotated min corner
  Eigen::Vector3d maxCorner_;  //!< The translated, but not rotated max corner

  double length2_;
  double width2_;
  double height2_;
  double radiusB_;
  double radius2_;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** \brief Definition of a convex mesh. Convex hull is computed for a given shape::Mesh */
class ConvexMesh : public Body
{
public:
  ConvexMesh() : Body()
  {
    type_ = shapes::MESH;
    scaled_vertices_ = nullptr;
  }

  ConvexMesh(const shapes::Shape* shape) : Body()
  {
    type_ = shapes::MESH;
    scaled_vertices_ = nullptr;
    setDimensions(shape);
  }

  ~ConvexMesh() override = default;

  /** \brief Returns an empty vector */
  std::vector<double> getDimensions() const override;
  /** \brief Returns an empty vector */
  std::vector<double> getScaledDimensions() const override;

  bool containsPoint(const Eigen::Vector3d& p, bool verbose = false) const override;
  double computeVolume() const override;

  void computeBoundingSphere(BoundingSphere& sphere) const override;
  void computeBoundingCylinder(BoundingCylinder& cylinder) const override;
  void computeBoundingBox(AABB& bbox) const override;
  void computeBoundingBox(OBB& bbox) const override;
  bool intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir,
                     EigenSTL::vector_Vector3d* intersections = nullptr, unsigned int count = 0) const override;

  const std::vector<unsigned int>& getTriangles() const;
  const EigenSTL::vector_Vector3d& getVertices() const;
  const EigenSTL::vector_Vector3d& getScaledVertices() const;

  /**
   * @brief Get the planes that define the convex shape.
   * @return A list of Vector4d(nx, ny, nz, d).
   */
  const EigenSTL::vector_Vector4d& getPlanes() const;

  BodyPtr cloneAt(const Eigen::Isometry3d& pose, double padding, double scale) const override;

  /// Project the original vertex to the scaled and padded planes and average.
  void computeScaledVerticesFromPlaneProjections();

  void correctVertexOrderFromPlanes();

  void updateInternalData() override;

protected:
  void useDimensions(const shapes::Shape* shape) override;

  /** \brief (Used mainly for debugging) Count the number of vertices behind a plane*/
  unsigned int countVerticesBehindPlane(const Eigen::Vector4f& planeNormal) const;

  /** \brief Check if the point is inside all halfspaces this mesh consists of (mesh_data_->planes_).
   *
   * \note The point is expected to have pose_ "cancelled" (have inverse pose of this mesh applied to it).
   * \note Scale and padding of the mesh are taken into account.
   * \note There is a 1e-9 margin "outside" the planes where points are still considered to be inside.
   */
  bool isPointInsidePlanes(const Eigen::Vector3d& point) const;

  // PIMPL structure
  struct MeshData;

  // shape-dependent data; keep this in one struct so that a cheap pointer copy can be done in cloneAt()
  std::shared_ptr<MeshData> mesh_data_;

  // pose/padding/scaling-dependent values & values computed for convenience and fast upcoming computations
  Eigen::Isometry3d i_pose_;
  Eigen::Vector3d center_;
  double radiusB_;
  double radiusBSqr_;
  Box bounding_box_;

  // pointer to an array of scaled vertices
  // If the padding is 0 & scaling is 1, then there is no need to have scaled vertices;
  // we can just point to the vertices in mesh_data_.
  // Otherwise, point to scaled_vertices_storage_
  EigenSTL::vector_Vector3d* scaled_vertices_;

private:
  std::unique_ptr<EigenSTL::vector_Vector3d> scaled_vertices_storage_;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** @class BodyVector
 *  @brief A vector of Body objects
 */
class BodyVector
{
public:
  BodyVector();

  /** \brief Construct a body vector from a vector of shapes, a vector of poses and a padding */
  BodyVector(const std::vector<shapes::Shape*>& shapes, const EigenSTL::vector_Isometry3d& poses, double padding = 0.0);

  ~BodyVector();

  /** \brief Add a body*/
  void addBody(Body* body);

  /** \brief Add a body from a shape, a pose for the body and a padding */
  void addBody(const shapes::Shape* shape, const Eigen::Isometry3d& pose, double padding = 0.0);

  /** \brief Clear all bodies from the vector*/
  void clear();

  /** \brief Set the pose of a particular body in the vector of bodies */
  void setPose(unsigned int i, const Eigen::Isometry3d& pose);

  /** \brief Get the number of bodies in this vector*/
  std::size_t getCount() const;

  /** \brief Check if any body in the vector contains the input point */
  bool containsPoint(const Eigen::Vector3d& p, bool verbose = false) const;

  /** \brief Check if any body contains the input point, and report the first body's index if so */
  bool containsPoint(const Eigen::Vector3d& p, std::size_t& index, bool verbose = false) const;

  /** \brief Check if any of the bodies intersects the ray defined by \e origin and \e dir.
      When the first intersection is found, this function terminates. The index of the body that
      does intersect the ray is set to \e index (unset if no intersections were found). Optionally,
      the intersection points are computed and set to \e intersections
      (only for the first body that is found to intersect the ray) */
  bool intersectsRay(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir, std::size_t& index,
                     EigenSTL::vector_Vector3d* intersections = nullptr, unsigned int count = 0) const;

  /** \brief Get the \e i<sup>th</sup> body in the vector*/
  const Body* getBody(unsigned int i) const;

private:
  std::vector<Body*> bodies_;
};

/** \brief Shared pointer to a Body */
typedef std::shared_ptr<Body> BodyPtr;

/** \brief Shared pointer to a const Body */
typedef std::shared_ptr<const Body> BodyConstPtr;
}  // namespace bodies

#endif
