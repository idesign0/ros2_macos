// Copyright 2019 Bielefeld University
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
//    * Neither the name of the Bielefeld University nor the names of its
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

#include <geometric_shapes/body_operations.h>
#include <geometric_shapes/shape_operations.h>
#include <boost/filesystem.hpp>
#include "resources/config.h"
#include <gtest/gtest.h>

using namespace bodies;

void expectVector3dSetsEqual(EigenSTL::vector_Vector3d vec1, EigenSTL::vector_Vector3d vec2,
                             const double upToError = 1e-6)
{
  ASSERT_EQ(vec1.size(), vec2.size());

  auto vecCompare = [upToError](const Eigen::Vector3d& a, const Eigen::Vector3d& b) -> bool {
    if (a.x() < b.x() - upToError)
      return true;
    if (a.x() > b.x() + upToError)
      return false;

    if (a.y() < b.y() - upToError)
      return true;
    if (a.y() > b.y() + upToError)
      return false;

    if (a.z() < b.z() - upToError)
      return true;
    if (a.z() > b.z() + upToError)
      return false;

    return false;
  };

  std::sort(vec1.begin(), vec1.end(), vecCompare);
  std::sort(vec2.begin(), vec2.end(), vecCompare);

  for (size_t i = 0; i < vec1.size(); ++i)
  {
    EXPECT_NEAR(vec1[i].x(), vec2[i].x(), upToError);
    EXPECT_NEAR(vec1[i].y(), vec2[i].y(), upToError);
    EXPECT_NEAR(vec1[i].z(), vec2[i].z(), upToError);
  }
}

TEST(Bodies, ConstructShapeFromBodySphere)
{
  const shapes::Shape* shape = new shapes::Sphere(2.0);
  const auto body = new Sphere(shape);

  const auto constructedShape = constructShapeFromBody(body);
  const auto constructedSphere = std::dynamic_pointer_cast<const shapes::Sphere>(constructedShape);

  EXPECT_EQ(shape->type, constructedShape->type);
  ASSERT_NE(nullptr, constructedSphere);
  EXPECT_EQ(2.0, constructedSphere->radius);
}

TEST(Bodies, ConstructShapeFromBodyBox)
{
  const shapes::Shape* shape = new shapes::Box(1.0, 2.0, 3.0);
  const auto body = new Box(shape);

  const auto constructedShape = constructShapeFromBody(body);
  const auto constructedBox = std::dynamic_pointer_cast<const shapes::Box>(constructedShape);

  EXPECT_EQ(shape->type, constructedShape->type);
  ASSERT_NE(nullptr, constructedBox);
  EXPECT_EQ(1.0, constructedBox->size[0]);
  EXPECT_EQ(2.0, constructedBox->size[1]);
  EXPECT_EQ(3.0, constructedBox->size[2]);
}

TEST(Bodies, ConstructShapeFromBodyCylinder)
{
  const shapes::Shape* shape = new shapes::Cylinder(1.0, 2.0);
  const auto body = new Cylinder(shape);

  const auto constructedShape = constructShapeFromBody(body);
  const auto constructedCylinder = std::dynamic_pointer_cast<const shapes::Cylinder>(constructedShape);

  EXPECT_EQ(shape->type, constructedShape->type);
  ASSERT_NE(nullptr, constructedCylinder);
  EXPECT_EQ(1.0, constructedCylinder->radius);
  EXPECT_EQ(2.0, constructedCylinder->length);
}

TEST(Bodies, ConstructShapeFromBodyMesh)
{
  shapes::Mesh* shape =
      shapes::createMeshFromResource("file://" + (boost::filesystem::path(TEST_RESOURCES_DIR) / "/box.dae").string());
  const auto body = new ConvexMesh(shape);

  const auto constructedShape = constructShapeFromBody(body);
  const auto constructedMesh = std::dynamic_pointer_cast<const shapes::Mesh>(constructedShape);

  EXPECT_EQ(shape->type, constructedShape->type);
  ASSERT_NE(nullptr, constructedMesh);
  ASSERT_EQ(shape->vertex_count, constructedMesh->vertex_count);
  ASSERT_EQ(shape->triangle_count, constructedMesh->triangle_count);

  // Compare the vertices and triangle normals of the constructed mesh
  // Triangle indices cannot be checked because the vertex IDs could change in the constructed mesh

  EigenSTL::vector_Vector3d verticesOrig, verticesConstructed;
  for (size_t i = 0; i < shape->vertex_count * 3; i += 3)
    verticesOrig.push_back({ shape->vertices[i], shape->vertices[i + 1], shape->vertices[i + 2] });
  for (size_t i = 0; i < constructedMesh->vertex_count * 3; i += 3)
    verticesConstructed.push_back(
        { constructedMesh->vertices[i], constructedMesh->vertices[i + 1], constructedMesh->vertices[i + 2] });

  expectVector3dSetsEqual(verticesOrig, verticesConstructed);

  EigenSTL::vector_Vector3d normalsOrig, normalsConstructed;
  shape->computeTriangleNormals();
  // constructedMesh->computeTriangleNormals();  // is done during construction
  for (size_t i = 0; i < shape->triangle_count * 3; i += 3)
    normalsOrig.push_back(
        { shape->triangle_normals[i], shape->triangle_normals[i + 1], shape->triangle_normals[i + 2] });
  for (size_t i = 0; i < constructedMesh->triangle_count * 3; i += 3)
    normalsConstructed.push_back({ constructedMesh->triangle_normals[i], constructedMesh->triangle_normals[i + 1],
                                   constructedMesh->triangle_normals[i + 2] });

  expectVector3dSetsEqual(normalsOrig, normalsConstructed);
}

TEST(Bodies, ConstructMarkerFromBodySphere)
{
  const Eigen::Isometry3d pose =
      Eigen::Translation3d(1.0, 2.0, 3.0) * Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(0.0, 1.0, 0.0));

  const shapes::Shape* shape = new shapes::Sphere(2.0);
  const auto body = new Sphere(shape);
  body->setPose(pose);

  visualization_msgs::msg::Marker marker;
  constructMarkerFromBody(body, marker);

  EXPECT_EQ(visualization_msgs::msg::Marker::SPHERE, marker.type);
  EXPECT_DOUBLE_EQ(4.0, marker.scale.x);
  EXPECT_DOUBLE_EQ(4.0, marker.scale.y);
  EXPECT_DOUBLE_EQ(4.0, marker.scale.z);
  EXPECT_DOUBLE_EQ(1.0, marker.pose.position.x);
  EXPECT_DOUBLE_EQ(2.0, marker.pose.position.y);
  EXPECT_DOUBLE_EQ(3.0, marker.pose.position.z);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.x);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.y);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.z);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.w);
}

TEST(Bodies, ConstructMarkerFromBodyBox)
{
  const Eigen::Isometry3d pose =
      Eigen::Translation3d(1.0, 2.0, 3.0) * Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(0.0, 1.0, 0.0));

  const shapes::Shape* shape = new shapes::Box(1.0, 2.0, 3.0);
  const auto body = new Box(shape);
  body->setPose(pose);

  visualization_msgs::msg::Marker marker;
  constructMarkerFromBody(body, marker);

  EXPECT_EQ(visualization_msgs::msg::Marker::CUBE, marker.type);
  EXPECT_DOUBLE_EQ(1.0, marker.scale.x);
  EXPECT_DOUBLE_EQ(2.0, marker.scale.y);
  EXPECT_DOUBLE_EQ(3.0, marker.scale.z);
  EXPECT_DOUBLE_EQ(1.0, marker.pose.position.x);
  EXPECT_DOUBLE_EQ(2.0, marker.pose.position.y);
  EXPECT_DOUBLE_EQ(3.0, marker.pose.position.z);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.x);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.y);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.z);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.w);
}

TEST(Bodies, ConstructMarkerFromBodyCylinder)
{
  const Eigen::Isometry3d pose =
      Eigen::Translation3d(1.0, 2.0, 3.0) * Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(0.0, 1.0, 0.0));

  const shapes::Shape* shape = new shapes::Cylinder(3.0, 2.0);
  const auto body = new Cylinder(shape);
  body->setPose(pose);

  visualization_msgs::msg::Marker marker;
  constructMarkerFromBody(body, marker);

  EXPECT_EQ(visualization_msgs::msg::Marker::CYLINDER, marker.type);
  EXPECT_DOUBLE_EQ(6.0, marker.scale.x);
  EXPECT_DOUBLE_EQ(6.0, marker.scale.y);
  EXPECT_DOUBLE_EQ(2.0, marker.scale.z);
  EXPECT_DOUBLE_EQ(1.0, marker.pose.position.x);
  EXPECT_DOUBLE_EQ(2.0, marker.pose.position.y);
  EXPECT_DOUBLE_EQ(3.0, marker.pose.position.z);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.x);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.y);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.z);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.w);
}

TEST(Bodies, ConstructMarkerFromBodyMesh)
{
  const Eigen::Isometry3d pose =
      Eigen::Translation3d(1.0, 2.0, 3.0) * Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(0.0, 1.0, 0.0));

  shapes::Mesh* shape =
      shapes::createMeshFromResource("file://" + (boost::filesystem::path(TEST_RESOURCES_DIR) / "/box.dae").string());
  const auto body = new ConvexMesh(shape);
  body->setPose(pose);

  visualization_msgs::msg::Marker marker;
  constructMarkerFromBody(body, marker);

  EXPECT_EQ(visualization_msgs::msg::Marker::TRIANGLE_LIST, marker.type);
  EXPECT_DOUBLE_EQ(1.0, marker.scale.x);
  EXPECT_DOUBLE_EQ(1.0, marker.scale.y);
  EXPECT_DOUBLE_EQ(1.0, marker.scale.z);
  EXPECT_DOUBLE_EQ(1.0, marker.pose.position.x);
  EXPECT_DOUBLE_EQ(2.0, marker.pose.position.y);
  EXPECT_DOUBLE_EQ(3.0, marker.pose.position.z);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.x);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.y);
  EXPECT_DOUBLE_EQ(0.0, marker.pose.orientation.z);
  EXPECT_DOUBLE_EQ(M_SQRT1_2, marker.pose.orientation.w);

  // We can't directly use shape->triangles because after passing it to the body constructor,
  // it can "optimize" the vertices (i.e. swap normals etc.) or reorder them
  const auto shapeFromBody = std::dynamic_pointer_cast<const shapes::Mesh>(constructShapeFromBody(body));

  EigenSTL::vector_Vector3d shapeVertices, markerVertices;
  for (size_t t = 0; t < shapeFromBody->triangle_count * 3; ++t)
  {
    const auto vertexId = shapeFromBody->triangles[t];
    shapeVertices.push_back({ shapeFromBody->vertices[3 * vertexId + 0], shapeFromBody->vertices[3 * vertexId + 1],
                              shapeFromBody->vertices[3 * vertexId + 2] });
  }
  for (auto& point : marker.points)
    markerVertices.push_back({ point.x, point.y, point.z });

  expectVector3dSetsEqual(shapeVertices, markerVertices);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
