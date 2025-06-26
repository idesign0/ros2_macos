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

/** \author Ioan Sucan, E. Gil Jones */

#include <geometric_shapes/body_operations.h>
#include <geometric_shapes/check_isometry.h>
#include <geometric_shapes/shape_operations.h>
#include <console_bridge/console.h>
#include <Eigen/Geometry>

bodies::Body* bodies::createEmptyBodyFromShapeType(const shapes::ShapeType& shapeType)
{
  Body* body = nullptr;

  switch (shapeType)
  {
    case shapes::BOX:
      body = new bodies::Box();
      break;
    case shapes::SPHERE:
      body = new bodies::Sphere();
      break;
    case shapes::CYLINDER:
      body = new bodies::Cylinder();
      break;
    case shapes::MESH:
      body = new bodies::ConvexMesh();
      break;
    default:
      CONSOLE_BRIDGE_logError("Creating body from shape: Unknown shape type %d", (int)shapeType);
      break;
  }
  return body;
}

bodies::Body* bodies::createBodyFromShape(const shapes::Shape* shape)
{
  Body* body = nullptr;

  if (shape)
  {
    body = createEmptyBodyFromShapeType(shape->type);
    body->setDimensions(shape);
  }

  return body;
}

shapes::ShapeConstPtr bodies::constructShapeFromBody(const bodies::Body* body)
{
  shapes::ShapePtr result;

  switch (body->getType())
  {
    case shapes::SPHERE:
    {
      const auto& dims = static_cast<const bodies::Sphere*>(body)->getScaledDimensions();
      result.reset(new shapes::Sphere(dims[0]));
      break;
    }
    case shapes::BOX:
    {
      const auto& dims = static_cast<const bodies::Box*>(body)->getScaledDimensions();
      result.reset(new shapes::Box(dims[0], dims[1], dims[2]));
      break;
    }
    case shapes::CYLINDER:
    {
      const auto& dims = static_cast<const bodies::Cylinder*>(body)->getScaledDimensions();
      result.reset(new shapes::Cylinder(dims[0], dims[1]));
      break;
    }
    case shapes::MESH:
    {
      const auto mesh = static_cast<const bodies::ConvexMesh*>(body);
      const auto& scaledVertices = mesh->getScaledVertices();

      // createMeshFromVertices requires an "expanded" list of triangles where each triangle is
      // represented by its three vertex positions
      EigenSTL::vector_Vector3d vertexList;
      vertexList.reserve(3 * mesh->getTriangles().size());
      for (const auto& triangle : mesh->getTriangles())
        vertexList.push_back(scaledVertices[triangle]);

      result.reset(shapes::createMeshFromVertices(vertexList));
      break;
    }
    default:
    {
      CONSOLE_BRIDGE_logError("Unknown body type: %d", (int)body->getType());
      break;
    }
  }
  return result;
}

void bodies::constructMarkerFromBody(const bodies::Body* body, visualization_msgs::msg::Marker& msg)
{
  auto shape = bodies::constructShapeFromBody(body);
  shapes::constructMarkerFromShape(shape.get(), msg, true);
  const auto& pose = body->getPose();
  msg.pose.position.x = pose.translation().x();
  msg.pose.position.y = pose.translation().y();
  msg.pose.position.z = pose.translation().z();

  ASSERT_ISOMETRY(pose);
  Eigen::Quaterniond quat(pose.linear().matrix());
  msg.pose.orientation.x = quat.x();
  msg.pose.orientation.y = quat.y();
  msg.pose.orientation.z = quat.z();
  msg.pose.orientation.w = quat.w();
}

void bodies::mergeBoundingSpheres(const std::vector<BoundingSphere>& spheres, BoundingSphere& mergedSphere)
{
  if (spheres.empty())
  {
    mergedSphere.center = Eigen::Vector3d(0.0, 0.0, 0.0);
    mergedSphere.radius = 0.0;
  }
  else
  {
    mergedSphere = spheres[0];
    for (unsigned int i = 1; i < spheres.size(); ++i)
    {
      if (spheres[i].radius <= 0.0)
        continue;
      Eigen::Vector3d diff = spheres[i].center - mergedSphere.center;
      double d = diff.norm();
      if (d + mergedSphere.radius <= spheres[i].radius)
      {
        mergedSphere.center = spheres[i].center;
        mergedSphere.radius = spheres[i].radius;
      }
      else if (d + spheres[i].radius > mergedSphere.radius)
      {
        Eigen::Vector3d delta = mergedSphere.center - spheres[i].center;
        mergedSphere.radius = (delta.norm() + spheres[i].radius + mergedSphere.radius) / 2.0;
        mergedSphere.center = delta.normalized() * (mergedSphere.radius - spheres[i].radius) + spheres[i].center;
      }
    }
  }
}

namespace bodies
{
template <typename T>
Body* constructBodyFromMsgHelper(const T& shape_msg, const geometry_msgs::msg::Pose& pose)
{
  shapes::ShapePtr shape(shapes::constructShapeFromMsg(shape_msg));

  if (shape)
  {
    Body* body = createEmptyBodyFromShapeType(shape->type);
    if (body)
    {
      Eigen::Quaterniond q(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z);
      if (fabs(q.squaredNorm() - 1.0) > 1e-3)
      {
        CONSOLE_BRIDGE_logError("Quaternion is not normalized. Assuming identity.");
        q = Eigen::Quaterniond(1.0, 0.0, 0.0, 0.0);
      }
      Eigen::Isometry3d af(Eigen::Translation3d(pose.position.x, pose.position.y, pose.position.z) * q);
      body->setPoseDirty(af);
      body->setDimensionsDirty(shape.get());
      body->updateInternalData();
      return body;
    }
  }
  return nullptr;
}
}  // namespace bodies

bodies::Body* bodies::constructBodyFromMsg(const shapes::ShapeMsg& shape_msg, const geometry_msgs::msg::Pose& pose)
{
  return constructBodyFromMsgHelper(shape_msg, pose);
}

bodies::Body* bodies::constructBodyFromMsg(const shape_msgs::msg::Mesh& shape_msg, const geometry_msgs::msg::Pose& pose)
{
  return constructBodyFromMsgHelper(shape_msg, pose);
}

bodies::Body* bodies::constructBodyFromMsg(const shape_msgs::msg::SolidPrimitive& shape_msg,
                                           const geometry_msgs::msg::Pose& pose)
{
  return constructBodyFromMsgHelper(shape_msg, pose);
}

void bodies::computeBoundingSphere(const std::vector<const bodies::Body*>& bodies, bodies::BoundingSphere& sphere)
{
  Eigen::Vector3d sum(0.0, 0.0, 0.0);

  // TODO - expand to all body types
  unsigned int vertex_count = 0;
  for (auto body : bodies)
  {
    if (!body || body->getType() != shapes::MESH)
      continue;
    // MESH type implies bodies::ConvexMesh
    const bodies::ConvexMesh* conv = static_cast<const bodies::ConvexMesh*>(body);

    for (unsigned int j = 0; j < conv->getScaledVertices().size(); j++, vertex_count++)
    {
      sum += conv->getPose() * conv->getScaledVertices()[j];
    }
  }

  sphere.center = sum / (double)vertex_count;

  double max_dist_squared = 0.0;
  for (auto body : bodies)
  {
    if (!body || body->getType() != shapes::MESH)
      continue;
    // MESH type implies bodies::ConvexMesh
    const bodies::ConvexMesh* conv = static_cast<const bodies::ConvexMesh*>(body);

    for (unsigned int j = 0; j < conv->getScaledVertices().size(); j++)
    {
      double dist = (conv->getPose() * conv->getScaledVertices()[j] - sphere.center).squaredNorm();
      if (dist > max_dist_squared)
      {
        max_dist_squared = dist;
      }
    }
  }
  sphere.radius = sqrt(max_dist_squared);
}

void bodies::mergeBoundingBoxes(const std::vector<bodies::AABB>& boxes, bodies::AABB& mergedBox)
{
  for (const auto& box : boxes)
    mergedBox.extend(box);
}

void bodies::mergeBoundingBoxesApprox(const std::vector<bodies::OBB>& boxes, bodies::OBB& mergedBox)
{
  for (const auto& box : boxes)
    mergedBox.extendApprox(box);
}
