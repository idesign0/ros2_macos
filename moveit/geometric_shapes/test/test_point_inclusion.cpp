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

/** \Author Ioan Sucan */

#include <geometric_shapes/bodies.h>
#include <geometric_shapes/shape_operations.h>
#include <geometric_shapes/body_operations.h>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include "resources/config.h"

// We expect surface points are counted inside.
#define EXPECT_SURF EXPECT_TRUE

// split length into the largest number elem, such that sqrt(elem^2 + elem^2) <= length
double largestComponentForLength2D(const double length)
{
  // HACK: sqrt(2) / 2 is a problem due to rounding errors since the distance
  //       is computed as 1.0000000000000002 . In such case we subtract an
  //       epsilon.

  double sq2 = sqrt(length / 2);
  while (sq2 * sq2 + sq2 * sq2 > length)
    sq2 -= std::numeric_limits<double>::epsilon();
  return sq2;
}

Eigen::Isometry3d getRandomPose(random_numbers::RandomNumberGenerator& g)
{
  const Eigen::Vector3d t(g.uniformReal(-100, 100), g.uniformReal(-100, 100), g.uniformReal(-100, 100));

  double quat[4];
  g.quaternion(quat);
  const Eigen::Quaterniond r({ quat[3], quat[0], quat[1], quat[2] });

  return Eigen::Isometry3d::TranslationType(t) * r;
}

TEST(SpherePointContainment, Basic)
{
  shapes::Sphere shape(1.0);
  bodies::Sphere sphere(&shape);

  // clang-format off
  // zero
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.00, 0.00, 0.00)));
  // general point outside
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(1.00, 1.00, 1.00)));

  // near single-axis maximum
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.99, 0.00, 0.00)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(1.00, 0.00, 0.00)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(1.01, 0.00, 0.00)));
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.00, 0.99, 0.00)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(0.00, 1.00, 0.00)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.00, 1.01, 0.00)));
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.00, 0.00, 0.99)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(0.00, 0.00, 1.00)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.00, 0.00, 1.01)));

  // near two-axis maximum
  const double sq2e = largestComponentForLength2D(1.0);
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.70, 0.70, 0.00)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(sq2e, sq2e, 0.00)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.71, 0.71, 0.00)));
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.70, 0.00, 0.70)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(sq2e, 0.00, sq2e)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.71, 0.00, 0.71)));
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.00, 0.70, 0.70)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(0.00, sq2e, sq2e)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.00, 0.71, 0.71)));

  // near three-axis maximum
  const double sq3 = sqrt(3) / 3;
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.57, 0.57, 0.57)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(sq3 , sq3 , sq3 )));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.58, 0.58, 0.58)));

  // near three-axis maximum with translation
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1.0, 0.0, 0.0);
  sphere.setPose(pose);

  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(1.57, 0.57, 0.57)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(1+sq3,sq3 , sq3 )));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(1.58, 0.58, 0.58)));

  pose.translation() = Eigen::Vector3d(0.0, 1.0, 0.0);
  sphere.setPose(pose);
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.57, 1.57, 0.57)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(sq3 ,1+sq3, sq3 )));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.58, 1.58, 0.58)));

  pose.translation() = Eigen::Vector3d(0.0, 0.0, 1.0);
  sphere.setPose(pose);
  EXPECT_TRUE( sphere.containsPoint(Eigen::Vector3d(0.57, 0.57, 1.57)));
  EXPECT_SURF( sphere.containsPoint(Eigen::Vector3d(sq3 , sq3, 1+sq3)));
  EXPECT_FALSE(sphere.containsPoint(Eigen::Vector3d(0.58, 0.58, 1.58)));
  // clang-format on
}

TEST(SpherePointContainment, SimpleInside)
{
  shapes::Sphere shape(1.0);
  bodies::Body* sphere = new bodies::Sphere(&shape);
  sphere->setScale(1.05);
  EXPECT_TRUE(sphere->containsPoint(0, 0, 1.0));

  random_numbers::RandomNumberGenerator r(0);
  Eigen::Vector3d p;
  for (int i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(r);
    sphere->setPose(pos);
    sphere->setScale(r.uniformReal(0.1, 100.0));
    sphere->setPadding(r.uniformReal(-0.001, 10.0));

    EXPECT_TRUE(sphere->samplePointInside(r, 100, p));
    EXPECT_TRUE(sphere->containsPoint(p));
  }
  delete sphere;
}

TEST(SpherePointContainment, SimpleOutside)
{
  shapes::Sphere shape(1.0);
  bodies::Body* sphere = new bodies::Sphere(&shape);
  sphere->setScale(0.95);
  bool contains = sphere->containsPoint(0, 0, 1.0);
  delete sphere;
  EXPECT_FALSE(contains);
}

TEST(SpherePointContainment, ComplexInside)
{
  shapes::Sphere shape(1.0);
  bodies::Body* sphere = new bodies::Sphere(&shape);
  sphere->setScale(0.95);
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1.0, 1.0, 1.0);
  sphere->setPose(pose);
  bool contains = sphere->containsPoint(0.5, 1, 1.0);
  delete sphere;
  EXPECT_TRUE(contains);
}

TEST(SpherePointContainment, ComplexOutside)
{
  shapes::Sphere shape(1.0);
  bodies::Body* sphere = new bodies::Sphere(&shape);
  sphere->setScale(0.95);
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1.0, 1.0, 1.0);
  sphere->setPose(pose);
  bool contains = sphere->containsPoint(0.5, 0.0, 0.0);
  delete sphere;
  EXPECT_FALSE(contains);
}

TEST(BoxPointContainment, Basic)
{
  shapes::Box shape(2.0, 2.0, 2.0);
  bodies::Box box(&shape);

  // clang-format off
  // zero
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.00, 0.00, 0.00)));
  // general point outside
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(2.00, 2.00, 2.00)));

  // near single-axis maximum
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.99, 0.00, 0.00)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(1.00, 0.00, 0.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(1.01, 0.00, 0.00)));
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.00, 0.99, 0.00)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(0.00, 1.00, 0.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(0.00, 1.01, 0.00)));
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.00, 0.00, 0.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(0.00, 0.00, 1.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(0.00, 0.00, 1.01)));

  // near two-axis maximum
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.99, 0.99, 0.00)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(1.00, 1.00, 0.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(1.01, 1.01, 0.00)));
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.99, 0.00, 0.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(1.00, 0.00, 1.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(1.01, 0.00, 1.01)));
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.00, 0.99, 0.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(0.00, 1.00, 1.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(0.00, 1.01, 1.01)));

  // near three-axis maximum
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.99, 0.99, 0.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(1.00, 1.00, 1.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(1.01, 1.01, 1.01)));

  // near three-axis maximum with translation
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1.0, 0.0, 0.0);
  box.setPose(pose);

  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(1.99, 0.99, 0.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(2.00, 1.00, 1.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(2.01, 1.01, 1.01)));

  pose.translation() = Eigen::Vector3d(0.0, 1.0, 0.0);
  box.setPose(pose);
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.99, 1.99, 0.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(1.00, 2.00, 1.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(1.01, 2.01, 1.01)));

  pose.translation() = Eigen::Vector3d(0.0, 0.0, 1.0);
  box.setPose(pose);
  EXPECT_TRUE( box.containsPoint(Eigen::Vector3d(0.99, 0.99, 1.99)));
  EXPECT_SURF( box.containsPoint(Eigen::Vector3d(1.00, 1.00, 2.00)));
  EXPECT_FALSE(box.containsPoint(Eigen::Vector3d(1.01, 1.01, 2.01)));
  // clang-format on
}

TEST(BoxPointContainment, SimpleInside)
{
  shapes::Box shape(1.0, 2.0, 3.0);
  bodies::Body* box = new bodies::Box(&shape);
  box->setScale(0.95);
  bool contains = box->containsPoint(0, 0, 1.0);
  EXPECT_TRUE(contains);

  random_numbers::RandomNumberGenerator r;
  Eigen::Vector3d p;
  EXPECT_TRUE(box->samplePointInside(r, 100, p));
  EXPECT_TRUE(box->containsPoint(p));

  delete box;
}

TEST(BoxPointContainment, SimpleOutside)
{
  shapes::Box shape(1.0, 2.0, 3.0);
  bodies::Body* box = new bodies::Box(&shape);
  box->setScale(0.95);
  bool contains = box->containsPoint(0, 0, 3.0);
  delete box;
  EXPECT_FALSE(contains);
}

TEST(BoxPointContainment, ComplexInside)
{
  shapes::Box shape(1.0, 1.0, 1.0);
  bodies::Body* box = new bodies::Box(&shape);
  box->setScale(1.01);
  Eigen::Isometry3d pose(Eigen::AngleAxisd(M_PI / 3.0, Eigen::Vector3d::UnitX()));
  pose.translation() = Eigen::Vector3d(1.0, 1.0, 1.0);
  box->setPose(pose);

  bool contains = box->containsPoint(1.5, 1.0, 1.5);
  EXPECT_TRUE(contains);

  delete box;
}

TEST(BoxPointContainment, Sampled)
{
  shapes::Box shape(1.0, 2.0, 3.0);
  bodies::Box box(&shape);

  random_numbers::RandomNumberGenerator r(0);
  Eigen::Vector3d p;
  for (int i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(r);
    box.setPose(pos);
    box.setScale(r.uniformReal(0.1, 100.0));
    box.setPadding(r.uniformReal(-0.001, 10.0));

    EXPECT_TRUE(box.samplePointInside(r, 100, p));
    EXPECT_TRUE(box.containsPoint(p));
  }
}

TEST(BoxPointContainment, ComplexOutside)
{
  shapes::Box shape(1.0, 1.0, 1.0);
  bodies::Body* box = new bodies::Box(&shape);
  box->setScale(1.01);
  Eigen::Isometry3d pose(Eigen::AngleAxisd(M_PI / 3.0, Eigen::Vector3d::UnitX()));
  pose.translation() = Eigen::Vector3d(1.0, 1.0, 1.0);
  box->setPose(pose);

  bool contains = box->containsPoint(1.5, 1.5, 1.5);
  delete box;
  EXPECT_FALSE(contains);
}

TEST(CylinderPointContainment, Basic)
{
  shapes::Cylinder shape(1.0, 4.0);
  bodies::Cylinder cylinder(&shape);

  // clang-format off
  // zero
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.00, 0.00, 0.00)));
  // general point outside
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(1.00, 1.00, 4.00)));

  // near single-axis maximum
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.99, 0.00, 0.00)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(1.00, 0.00, 0.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(1.01, 0.00, 0.00)));
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.00, 0.99, 0.00)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(0.00, 1.00, 0.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.00, 1.01, 0.00)));
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.00, 0.00, 1.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(0.00, 0.00, 2.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.00, 0.00, 2.01)));

  // near two-axis maximum
  const double sq2 = sqrt(2) / 2;
  const double sq2e = largestComponentForLength2D(1);

  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.70, 0.70, 0.00)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(sq2e, sq2e, 0.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.71, 0.71, 0.00)));
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.99, 0.00, 1.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(1.00, 0.00, 2.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(1.01, 0.00, 2.01)));
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.00, 0.99, 1.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(0.00, 1.00, 2.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.00, 1.01, 2.01)));

  // near three-axis maximum
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.70, 0.70, 1.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(sq2e, sq2e, 2.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.71, 0.71, 2.01)));

  // near three-axis maximum with translation
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1.0, 0.0, 0.0);
  cylinder.setPose(pose);

  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(1.70, 0.70, 1.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(1+sq2,sq2 , 2.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(1.71, 0.71, 2.01)));

  pose.translation() = Eigen::Vector3d(0.0, 1.0, 0.0);
  cylinder.setPose(pose);
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.70, 1.70, 1.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(sq2 ,1+sq2, 2.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.71, 1.71, 2.01)));

  pose.translation() = Eigen::Vector3d(0.0, 0.0, 1.0);
  cylinder.setPose(pose);
  EXPECT_TRUE( cylinder.containsPoint(Eigen::Vector3d(0.70, 0.70, 2.99)));
  EXPECT_SURF( cylinder.containsPoint(Eigen::Vector3d(sq2e, sq2e, 3.00)));
  EXPECT_FALSE(cylinder.containsPoint(Eigen::Vector3d(0.71, 0.71, 3.01)));
  // clang-format on
}

TEST(CylinderPointContainment, SimpleInside)
{
  shapes::Cylinder shape(1.0, 4.0);
  bodies::Body* cylinder = new bodies::Cylinder(&shape);
  cylinder->setScale(1.05);
  bool contains = cylinder->containsPoint(0, 0, 2.0);
  delete cylinder;
  EXPECT_TRUE(contains);
}

TEST(CylinderPointContainment, SimpleOutside)
{
  shapes::Cylinder shape(1.0, 4.0);
  bodies::Body* cylinder = new bodies::Cylinder(&shape);
  cylinder->setScale(0.95);
  bool contains = cylinder->containsPoint(0, 0, 2.0);
  delete cylinder;
  EXPECT_FALSE(contains);
}

TEST(CylinderPointContainment, CylinderPadding)
{
  shapes::Cylinder shape(1.0, 4.0);
  bodies::Body* cylinder = new bodies::Cylinder(&shape);
  bool contains = cylinder->containsPoint(0, 1.01, 0);
  EXPECT_FALSE(contains);
  cylinder->setPadding(.02);
  contains = cylinder->containsPoint(0, 1.01, 0);
  EXPECT_TRUE(contains);
  cylinder->setPadding(0.0);
  bodies::BoundingSphere bsphere;
  cylinder->computeBoundingSphere(bsphere);
  EXPECT_TRUE(bsphere.radius > 2.0);

  delete cylinder;
}

TEST(CylinderPointContainment, Sampled)
{
  shapes::Cylinder shape(1.0, 4.0);
  bodies::Cylinder cylinder(&shape);

  random_numbers::RandomNumberGenerator r(0);
  Eigen::Vector3d p;
  for (int i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(r);
    cylinder.setPose(pos);
    cylinder.setScale(r.uniformReal(0.1, 100.0));
    cylinder.setPadding(r.uniformReal(-0.001, 10.0));

    EXPECT_TRUE(cylinder.samplePointInside(r, 100, p));
    EXPECT_TRUE(cylinder.containsPoint(p));
  }
}

TEST(MeshPointContainment, Basic)
{
  // clang-format off
  shapes::Mesh *ms = shapes::createMeshFromResource("file://" +
    (boost::filesystem::path(TEST_RESOURCES_DIR) / "/box.dae").string());
  ASSERT_TRUE(ms != nullptr);
  bodies::ConvexMesh cubeMesh(ms);
  cubeMesh.setScale(1.5);
  cubeMesh.setPadding(0.5 * sqrt(3));

  // zero
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 0.00, 0.00)));
  // general point outside
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(3.00, 3.00, 3.00)));

  // near single-axis maximum
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(1.99, 0.00, 0.00)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(2.00, 0.00, 0.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(2.01, 0.00, 0.00)));
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 1.99, 0.00)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 2.00, 0.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(0.00, 2.01, 0.00)));
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 0.00, 1.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 0.00, 2.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(0.00, 0.00, 2.01)));

  // near two-axis maximum
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(1.99, 1.99, 0.00)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(2.00, 2.00, 0.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(2.01, 2.01, 0.00)));
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(1.99, 0.00, 1.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(2.00, 0.00, 2.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(2.01, 0.00, 2.01)));
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 1.99, 1.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(0.00, 2.00, 2.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(0.00, 2.01, 2.01)));

  // near three-axis maximum
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(1.99, 1.99, 1.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(2.00, 2.00, 2.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(2.01, 2.01, 2.01)));

  // near three-axis maximum with translation
  Eigen::Isometry3d pose;
  pose.setIdentity();
  pose.translation() = Eigen::Vector3d(1.0, 0.0, 0.0);
  cubeMesh.setPose(pose);

  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(2.99, 1.99, 1.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(3.00, 2.00, 2.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(3.01, 2.01, 2.01)));

  pose.translation() = Eigen::Vector3d(0.0, 1.0, 0.0);
  cubeMesh.setPose(pose);
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(1.99, 2.99, 1.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(2.00, 3.00, 2.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(2.01, 3.01, 2.01)));

  pose.translation() = Eigen::Vector3d(0.0, 0.0, 1.0);
  cubeMesh.setPose(pose);
  EXPECT_TRUE( cubeMesh.containsPoint(Eigen::Vector3d(1.99, 1.99, 2.99)));
  EXPECT_SURF( cubeMesh.containsPoint(Eigen::Vector3d(2.00, 2.00, 3.00)));
  EXPECT_FALSE(cubeMesh.containsPoint(Eigen::Vector3d(2.01, 2.01, 3.01)));
  // clang-format on

  delete ms;
}

TEST(MeshPointContainment, Pr2Forearm)
{
  shapes::Mesh* ms = shapes::createMeshFromResource(
      "file://" + (boost::filesystem::path(TEST_RESOURCES_DIR) / "/forearm_roll.stl").string());
  ASSERT_TRUE(ms != nullptr);
  bodies::Body* m = new bodies::ConvexMesh(ms);
  ASSERT_TRUE(m != nullptr);
  Eigen::Isometry3d t(Eigen::Isometry3d::Identity());
  t.translation().x() = 1.0;
  EXPECT_FALSE(m->cloneAt(t)->containsPoint(-1.0, 0.0, 0.0));

  random_numbers::RandomNumberGenerator r(0);
  Eigen::Vector3d p;
  bool found = true;
  for (int i = 0; i < 10; ++i)
  {
    if (m->samplePointInside(r, 10000, p))
    {
      found = true;
      EXPECT_TRUE(m->containsPoint(p));
    }
  }
  EXPECT_TRUE(found);

  delete m;
  delete ms;
}

TEST(MergeBoundingSpheres, MergeTwoSpheres)
{
  std::vector<bodies::BoundingSphere> spheres;

  bodies::BoundingSphere s1;
  s1.center = Eigen::Vector3d(5.0, 0.0, 0.0);
  s1.radius = 1.0;

  bodies::BoundingSphere s2;
  s2.center = Eigen::Vector3d(-5.1, 0.0, 0.0);
  s2.radius = 1.0;

  spheres.push_back(s1);
  spheres.push_back(s2);

  bodies::BoundingSphere merged_sphere;
  bodies::mergeBoundingSpheres(spheres, merged_sphere);

  EXPECT_NEAR(merged_sphere.center[0], -.05, .00001);
  EXPECT_EQ(merged_sphere.radius, 6.05);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
