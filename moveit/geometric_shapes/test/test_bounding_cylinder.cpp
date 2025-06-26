// Copyright 2019 Open Robotics
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

/** \Author Martin Pecka */

#include <geometric_shapes/bodies.h>
#include <geometric_shapes/body_operations.h>
#include <random_numbers/random_numbers.h>
#include <gtest/gtest.h>

// The magic numbers in this test were verified visually using Blender

TEST(SphereBoundingCylinder, Sphere1)
{
  shapes::Sphere shape(1.0);
  bodies::Sphere body(&shape);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  // orientation can be any
  EXPECT_NEAR(1.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(2.0, bcyl.length, 1e-4);

  body.setScale(2.0);
  body.setPadding(1.0);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  // orientation can be any
  EXPECT_NEAR(3.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(6.0, bcyl.length, 1e-4);
}

TEST(SphereBoundingCylinder, Sphere2)
{
  shapes::Sphere shape(2.0);
  bodies::Sphere body(&shape);
  Eigen::Isometry3d pose(Eigen::Isometry3d::TranslationType(1, 2, 3));
  body.setPose(pose);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  // orientation can be any
  EXPECT_NEAR(2.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(4.0, bcyl.length, 1e-4);

  // verify the bounding cylinder is rotation-invariant

  pose *= Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(1, 1, 1).normalized());
  body.setPose(pose);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  // orientation can be any
  EXPECT_NEAR(2.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(4.0, bcyl.length, 1e-4);

  random_numbers::RandomNumberGenerator gen;
  double quatData[4];
  Eigen::Quaterniond quat;

  for (size_t i = 0; i < 10; ++i)
  {
    gen.quaternion(quatData);
    quat.x() = quatData[0];
    quat.y() = quatData[1];
    quat.z() = quatData[2];
    quat.w() = quatData[3];
    pose.linear() = quat.toRotationMatrix();
    body.setPose(pose);
    bodies::BoundingCylinder bcyl2;
    body.computeBoundingCylinder(bcyl2);

    EXPECT_NEAR(1.0, bcyl2.pose.translation().x(), 1e-4);
    EXPECT_NEAR(2.0, bcyl2.pose.translation().y(), 1e-4);
    EXPECT_NEAR(3.0, bcyl2.pose.translation().z(), 1e-4);
    // orientation can be any
    EXPECT_NEAR(2.0, bcyl2.radius, 1e-4);
    EXPECT_NEAR(4.0, bcyl2.length, 1e-4);
  }
}

TEST(BoxBoundingCylinder, Box1)
{
  shapes::Box shape(1.0, 2.0, 3.0);
  bodies::Box body(&shape);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  body.setScale(2.0);
  body.setPadding(1.0);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(2 * 2 + 3 * 3), bcyl.radius, 1e-4);
  EXPECT_NEAR(8.0, bcyl.length, 1e-4);

  // test that the rotational axis of the cylinder sticks with the longest dimension

  shape = shapes::Box(2.0, 3.0, 1.0);
  body = bodies::Box(&shape);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  body.setScale(2.0);
  body.setPadding(1.0);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(2 * 2 + 3 * 3), bcyl.radius, 1e-4);
  EXPECT_NEAR(8.0, bcyl.length, 1e-4);

  shape = shapes::Box(3.0, 1.0, 2.0);
  body = bodies::Box(&shape);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  body.setScale(2.0);
  body.setPadding(1.0);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(2 * 2 + 3 * 3), bcyl.radius, 1e-4);
  EXPECT_NEAR(8.0, bcyl.length, 1e-4);
}

TEST(BoxBoundingCylinder, Box2)
{
  shapes::Box shape(1.0, 2.0, 3.0);
  bodies::Box body(&shape);
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1, 2, 3);
  body.setPose(pose);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  Eigen::AngleAxisd rot(M_PI_2, Eigen::Vector3d(1, 1, 1).normalized());
  pose *= rot;
  body.setPose(pose);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).x(), Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).y(), Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).z(), Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).w(), Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);
}

TEST(CylinderBoundingCylinder, Cylinder1)
{
  shapes::Cylinder shape(1.0, 2.0);
  bodies::Cylinder body(&shape);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(1.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(2.0, bcyl.length, 1e-4);

  body.setScale(2.0);
  body.setPadding(1.0);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(6.0, bcyl.length, 1e-4);
}

TEST(CylinderBoundingCylinder, Cylinder2)
{
  shapes::Cylinder shape(1.0, 2.0);
  bodies::Cylinder body(&shape);
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1, 2, 3);
  body.setPose(pose);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(1.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(2.0, bcyl.length, 1e-4);

  pose.linear() = Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(1, 1, 1).normalized()).toRotationMatrix();
  body.setPose(pose);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(pose.linear()).x(), Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(pose.linear()).y(), Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(pose.linear()).z(), Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(pose.linear()).w(), Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  EXPECT_NEAR(1.0, bcyl.radius, 1e-4);
  EXPECT_NEAR(2.0, bcyl.length, 1e-4);
}

shapes::Mesh* createBoxMesh(const Eigen::Vector3d& min, const Eigen::Vector3d& max)
{
  shapes::Mesh* m = new shapes::Mesh(8, 12);

  m->vertices[3 * 0 + 0] = min.x();
  m->vertices[3 * 0 + 1] = min.y();
  m->vertices[3 * 0 + 2] = min.z();

  m->vertices[3 * 1 + 0] = max.x();
  m->vertices[3 * 1 + 1] = min.y();
  m->vertices[3 * 1 + 2] = min.z();

  m->vertices[3 * 2 + 0] = min.x();
  m->vertices[3 * 2 + 1] = max.y();
  m->vertices[3 * 2 + 2] = min.z();

  m->vertices[3 * 3 + 0] = max.x();
  m->vertices[3 * 3 + 1] = max.y();
  m->vertices[3 * 3 + 2] = min.z();

  m->vertices[3 * 4 + 0] = min.x();
  m->vertices[3 * 4 + 1] = min.y();
  m->vertices[3 * 4 + 2] = max.z();

  m->vertices[3 * 5 + 0] = max.x();
  m->vertices[3 * 5 + 1] = min.y();
  m->vertices[3 * 5 + 2] = max.z();

  m->vertices[3 * 6 + 0] = min.x();
  m->vertices[3 * 6 + 1] = max.y();
  m->vertices[3 * 6 + 2] = max.z();

  m->vertices[3 * 7 + 0] = max.x();
  m->vertices[3 * 7 + 1] = max.y();
  m->vertices[3 * 7 + 2] = max.z();

  m->triangles[3 * 0 + 0] = 0;
  m->triangles[3 * 0 + 1] = 1;
  m->triangles[3 * 0 + 2] = 2;

  m->triangles[3 * 1 + 0] = 1;
  m->triangles[3 * 1 + 1] = 3;
  m->triangles[3 * 1 + 2] = 2;

  m->triangles[3 * 2 + 0] = 5;
  m->triangles[3 * 2 + 1] = 4;
  m->triangles[3 * 2 + 2] = 6;

  m->triangles[3 * 3 + 0] = 5;
  m->triangles[3 * 3 + 1] = 6;
  m->triangles[3 * 3 + 2] = 7;

  m->triangles[3 * 4 + 0] = 1;
  m->triangles[3 * 4 + 1] = 5;
  m->triangles[3 * 4 + 2] = 3;

  m->triangles[3 * 5 + 0] = 5;
  m->triangles[3 * 5 + 1] = 7;
  m->triangles[3 * 5 + 2] = 3;

  m->triangles[3 * 6 + 0] = 4;
  m->triangles[3 * 6 + 1] = 0;
  m->triangles[3 * 6 + 2] = 2;

  m->triangles[3 * 7 + 0] = 4;
  m->triangles[3 * 7 + 1] = 2;
  m->triangles[3 * 7 + 2] = 6;

  m->triangles[3 * 8 + 0] = 2;
  m->triangles[3 * 8 + 1] = 3;
  m->triangles[3 * 8 + 2] = 6;

  m->triangles[3 * 9 + 0] = 3;
  m->triangles[3 * 9 + 1] = 7;
  m->triangles[3 * 9 + 2] = 6;

  m->triangles[3 * 10 + 0] = 1;
  m->triangles[3 * 10 + 1] = 0;
  m->triangles[3 * 10 + 2] = 4;

  m->triangles[3 * 11 + 0] = 1;
  m->triangles[3 * 11 + 1] = 4;
  m->triangles[3 * 11 + 2] = 5;

  return m;
}

TEST(MeshBoundingCylinder, Mesh1)
{
  shapes::Mesh* m = createBoxMesh({ -0.5, -1.0, -1.5 }, { 0.5, 1.0, 1.5 });

  double radiusScaled;
  double lengthScaled;

  bodies::ConvexMesh body(m);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  // for meshes without padding, the bounding cylinder should be tight
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  body.setScale(2.0);
  body.setPadding(1.0);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  // computation of bounding cylinder with padding is only approximate; the approximation error in both radius and
  // length is anywhere from zero to padding_ (or 2*padding_ for length)
  radiusScaled = 2 * sqrt(0.5 * 0.5 + 1 * 1);
  EXPECT_LE(radiusScaled, bcyl.radius);
  EXPECT_GE(radiusScaled + 1.0, bcyl.radius);
  lengthScaled = 6.0;
  EXPECT_LE(lengthScaled, bcyl.length);
  EXPECT_GE(lengthScaled + 2 * 1.0, bcyl.length);

  delete m;

  // test that the rotational axis of the cylinder sticks with the longest dimension

  m = createBoxMesh({ -1.0, -1.5, -0.5 }, { 1.0, 1.5, 0.5 });
  bodies::ConvexMesh body2(m);
  body2.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  // for meshes without padding, the bounding cylinder should be tight
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  body2.setScale(2.0);
  body2.setPadding(1.0);
  body2.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  radiusScaled = 2 * sqrt(0.5 * 0.5 + 1 * 1);
  EXPECT_LE(radiusScaled, bcyl.radius);
  EXPECT_GE(radiusScaled + 1.0, bcyl.radius);
  lengthScaled = 6.0;
  EXPECT_LE(lengthScaled, bcyl.length);
  EXPECT_GE(lengthScaled + 2 * 1.0, bcyl.length);

  delete m;

  m = createBoxMesh({ -1.5, -0.5, -1.0 }, { 1.5, 0.5, 1.0 });
  bodies::ConvexMesh body3(m);
  body3.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  // for meshes without padding, the bounding cylinder should be tight
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  body3.setScale(2.0);
  body3.setPadding(1.0);
  body3.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(0.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(0.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(M_SQRT1_2, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  radiusScaled = 2 * sqrt(0.5 * 0.5 + 1 * 1);
  EXPECT_LE(radiusScaled, bcyl.radius);
  EXPECT_GE(radiusScaled + 1.0, bcyl.radius);
  lengthScaled = 6.0;
  EXPECT_LE(lengthScaled, bcyl.length);
  EXPECT_GE(lengthScaled + 2 * 1.0, bcyl.length);

  delete m;
}

TEST(MeshBoundingCylinder, Mesh2)
{
  shapes::Mesh* m = createBoxMesh({ -0.5, -1.0, -1.5 }, { 0.5, 1.0, 1.5 });

  bodies::ConvexMesh body(m);
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1, 2, 3);
  body.setPose(pose);
  bodies::BoundingCylinder bcyl;
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(0.0, Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(1.0, Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  // for meshes without padding, the bounding cylinder should be tight
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  Eigen::AngleAxisd rot(M_PI_2, Eigen::Vector3d(1, 1, 1).normalized());
  pose *= rot;
  body.setPose(pose);
  body.computeBoundingCylinder(bcyl);

  EXPECT_NEAR(1.0, bcyl.pose.translation().x(), 1e-4);
  EXPECT_NEAR(2.0, bcyl.pose.translation().y(), 1e-4);
  EXPECT_NEAR(3.0, bcyl.pose.translation().z(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).x(), Eigen::Quaterniond(bcyl.pose.linear()).x(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).y(), Eigen::Quaterniond(bcyl.pose.linear()).y(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).z(), Eigen::Quaterniond(bcyl.pose.linear()).z(), 1e-4);
  EXPECT_NEAR(Eigen::Quaterniond(rot).w(), Eigen::Quaterniond(bcyl.pose.linear()).w(), 1e-4);
  // for meshes without padding, the bounding cylinder should be tight
  EXPECT_NEAR(sqrt(0.5 * 0.5 + 1 * 1), bcyl.radius, 1e-4);
  EXPECT_NEAR(3.0, bcyl.length, 1e-4);

  delete m;
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
