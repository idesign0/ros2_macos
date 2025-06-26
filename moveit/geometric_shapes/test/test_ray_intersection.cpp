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

/** \Author Ioan Sucan */
/** \Author Martin Pecka */

#include <geometric_shapes/bodies.h>
#include <geometric_shapes/shape_operations.h>
#include <geometric_shapes/body_operations.h>
#include <boost/filesystem.hpp>
#include <random_numbers/random_numbers.h>
#include <gtest/gtest.h>
#include "resources/config.h"

Eigen::Isometry3d getRandomPose(random_numbers::RandomNumberGenerator& g)
{
  const Eigen::Vector3d t(g.uniformReal(-100, 100), g.uniformReal(-100, 100), g.uniformReal(-100, 100));

  double quat[4];
  g.quaternion(quat);
  const Eigen::Quaterniond r({ quat[3], quat[0], quat[1], quat[2] });

  return Eigen::Isometry3d::TranslationType(t) * r;
}

#define EXPECT_VECTORS_EQUAL(v1, v2, error)                                                                            \
  EXPECT_NEAR((v1)[0], (v2)[0], (error));                                                                              \
  EXPECT_NEAR((v1)[1], (v2)[1], (error));                                                                              \
  EXPECT_NEAR((v1)[2], (v2)[2], (error));

#define CHECK_NO_INTERSECTION(body, origin, direction)                                                                 \
  {                                                                                                                    \
    EigenSTL::vector_Vector3d intersections;                                                                           \
    Eigen::Vector3d o origin;                                                                                          \
    Eigen::Vector3d d direction;                                                                                       \
    const auto result = (body).intersectsRay(o, d, &intersections, 2);                                                 \
    EXPECT_FALSE(result);                                                                                              \
    EXPECT_EQ(0u, intersections.size());                                                                               \
  }

#define CHECK_INTERSECTS(body, origin, direction, numIntersections)                                                    \
  {                                                                                                                    \
    EigenSTL::vector_Vector3d intersections;                                                                           \
    Eigen::Vector3d o origin;                                                                                          \
    Eigen::Vector3d d direction;                                                                                       \
    const auto result = (body).intersectsRay(o, d, &intersections, 2);                                                 \
    EXPECT_TRUE(result);                                                                                               \
    EXPECT_EQ(static_cast<size_t>((numIntersections)), intersections.size());                                          \
  }

#define CHECK_INTERSECTS_ONCE(body, origin, direction, intersection, error)                                            \
  {                                                                                                                    \
    EigenSTL::vector_Vector3d intersections;                                                                           \
    Eigen::Vector3d o origin;                                                                                          \
    Eigen::Vector3d d direction;                                                                                       \
    Eigen::Vector3d i intersection;                                                                                    \
    const auto result = (body).intersectsRay(o, d, &intersections, 2);                                                 \
    EXPECT_TRUE(result);                                                                                               \
    EXPECT_EQ(1u, intersections.size());                                                                               \
    if (intersections.size() == 1u)                                                                                    \
    {                                                                                                                  \
      EXPECT_VECTORS_EQUAL(intersections.at(0), i, (error));                                                           \
    }                                                                                                                  \
  }

#define CHECK_INTERSECTS_TWICE(body, origin, direction, intersc1, intersc2, error)                                     \
  {                                                                                                                    \
    EigenSTL::vector_Vector3d intersections;                                                                           \
    Eigen::Vector3d o origin;                                                                                          \
    Eigen::Vector3d d direction;                                                                                       \
    Eigen::Vector3d i1 intersc1;                                                                                       \
    Eigen::Vector3d i2 intersc2;                                                                                       \
    const auto result = (body).intersectsRay(o, d, &intersections, 2);                                                 \
    EXPECT_TRUE(result);                                                                                               \
    EXPECT_EQ(2u, intersections.size());                                                                               \
    if (intersections.size() == 2u)                                                                                    \
    {                                                                                                                  \
      if (fabs(static_cast<double>((intersections.at(0) - i1).norm())) < (error))                                      \
      {                                                                                                                \
        EXPECT_VECTORS_EQUAL(intersections.at(0), i1, (error));                                                        \
        EXPECT_VECTORS_EQUAL(intersections.at(1), i2, (error));                                                        \
      }                                                                                                                \
      else                                                                                                             \
      {                                                                                                                \
        EXPECT_VECTORS_EQUAL(intersections.at(0), i2, (error));                                                        \
        EXPECT_VECTORS_EQUAL(intersections.at(1), i1, (error));                                                        \
      }                                                                                                                \
    }                                                                                                                  \
  }

TEST(SphereRayIntersection, OriginInside)
{
  shapes::Sphere shape(1.0);
  bodies::Sphere sphere(&shape);

  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), (-1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0, -1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  0,  1), ( 0,  0,  1), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  0, -1), ( 0,  0, -1), 1e-6)
  // clang-format on

  // scaling

  sphere.setScale(1.1);
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move origin within the sphere

  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (0.5, 0  , 0  ), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0.5, 0  , 0  ), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0  , 0.5, 0  ), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0  , 0.5, 0  ), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0  , 0  , 0.5), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0  , 0  , 0.5), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move sphere

  Eigen::Isometry3d pose = Eigen::Translation3d(0.5, 0, 0) * Eigen::Quaterniond::Identity();
  sphere.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 1, 0, 0), ( 1.6, 0, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), (-1, 0, 0), (-0.6, 0, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0.5, 0) * Eigen::Quaterniond::Identity();
  sphere.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0,  1, 0), (0,  1.6, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0, -1, 0), (0, -0.6, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0, 0.5) * Eigen::Quaterniond::Identity();
  sphere.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0, 0,  1), (0, 0,  1.6), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (0, 0, 0), ( 0, 0, -1), (0, 0, -0.6), 1e-6)
  // clang-format on

  // 3D diagonal

  sphere.setPose(Eigen::Isometry3d::Identity());
  sphere.setScale(1.0);
  sphere.setPadding(0.1);

  const auto sq3 = sqrt(pow(1 + 0.1, 2) / 3);
  const auto dir3 = Eigen::Vector3d::Ones().normalized();
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), ( dir3), ( sq3,  sq3,  sq3), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(sphere, ( 0.5,  0.5,  0.5), ( dir3), ( sq3,  sq3,  sq3), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(sphere, (-0.5, -0.5, -0.5), ( dir3), ( sq3,  sq3,  sq3), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), (-dir3), (-sq3, -sq3, -sq3), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, ( 0.5,  0.5,  0.5), (-dir3), (-sq3, -sq3, -sq3), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (-0.5, -0.5, -0.5), (-dir3), (-sq3, -sq3, -sq3), 1e-4)
  // clang-format on

  // 2D diagonal

  const auto sq2 = sqrt(pow(1 + 0.1, 2) / 2);
  const auto dir2 = 1 / sqrt(2);
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), ( dir2,  dir2,     0), ( sq2,  sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), (-dir2, -dir2,     0), (-sq2, -sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, ( 0.5,  0.5,    0), ( dir2,  dir2,     0), ( sq2,  sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, ( 0.5,  0.5,    0), (-dir2, -dir2,     0), (-sq2, -sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (-0.5, -0.5,    0), ( dir2,  dir2,     0), ( sq2,  sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (-0.5, -0.5,    0), (-dir2, -dir2,     0), (-sq2, -sq2,    0), 1e-4)

  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), (    0,  dir2,  dir2), (   0,  sq2,  sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), (    0, -dir2, -dir2), (   0, -sq2, -sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0,  0.5,  0.5), (    0,  dir2,  dir2), (   0,  sq2,  sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0,  0.5,  0.5), (    0, -dir2, -dir2), (   0, -sq2, -sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0, -0.5, -0.5), (    0,  dir2,  dir2), (   0,  sq2,  sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0, -0.5, -0.5), (    0, -dir2, -dir2), (   0, -sq2, -sq2), 1e-4)

  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), ( dir2,     0,  dir2), ( sq2,    0,  sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (   0,    0,    0), (-dir2,     0, -dir2), (-sq2,    0, -sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, ( 0.5,    0,  0.5), ( dir2,     0,  dir2), ( sq2,    0,  sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, ( 0.5,    0,  0.5), (-dir2,     0, -dir2), (-sq2,    0, -sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (-0.5,    0, -0.5), ( dir2,     0,  dir2), ( sq2,    0,  sq2), 1e-4)
  CHECK_INTERSECTS_ONCE(sphere, (-0.5,    0, -0.5), (-dir2,     0, -dir2), (-sq2,    0, -sq2), 1e-4)
  // clang-format on

  // any random rays that start inside the sphere should have exactly one intersection with it
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    sphere.setPose(pos);
    sphere.setScale(g.uniformReal(0.5, 100.0));
    sphere.setPadding(g.uniformReal(-0.1, 100.0));

    Eigen::Vector3d origin;
    sphere.samplePointInside(g, 10, origin);

    // get the scaled sphere
    bodies::BoundingSphere s;
    sphere.computeBoundingSphere(s);

    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    EigenSTL::vector_Vector3d intersections;
    if (sphere.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(1u, intersections.size());
      if (intersections.size() == 1)
      {
        EXPECT_NEAR(s.radius, (s.center - intersections[0]).norm(), 1e-6);
        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(SphereRayIntersection, OriginOutside)
{
  shapes::Sphere shape(1.0);
  bodies::Sphere sphere(&shape);

  // clang-format off
  CHECK_INTERSECTS_TWICE(sphere, (-2,  0,  0), ( 1,  0,  0), ( 1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(sphere, ( 2,  0,  0), (-1,  0,  0), (-1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(sphere, ( 0, -2,  0), ( 0,  1,  0), ( 0,  1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(sphere, ( 0,  2,  0), ( 0, -1,  0), ( 0, -1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(sphere, ( 0,  0, -2), ( 0,  0,  1), ( 0,  0,  1), ( 0,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(sphere, ( 0,  0,  2), ( 0,  0, -1), ( 0,  0, -1), ( 0,  0,  1), 1e-6)
  // clang-format on

  // test hitting the surface
  // clang-format off
  CHECK_INTERSECTS_ONCE(sphere, (-1, -1,  0), ( 0,  1,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (-1,  1,  0), ( 0, -1,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 1, -1,  0), ( 0,  1,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 1,  1,  0), ( 0, -1,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 0, -1, -1), ( 0,  0,  1), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 0, -1,  1), ( 0,  0, -1), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 0,  1, -1), ( 0,  0,  1), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 0,  1,  1), ( 0,  0, -1), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (-1,  0, -1), ( 1,  0,  0), ( 0,  0, -1), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 1,  0, -1), (-1,  0,  0), ( 0,  0, -1), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, (-1,  0,  1), ( 1,  0,  0), ( 0,  0,  1), 1e-6)
  CHECK_INTERSECTS_ONCE(sphere, ( 1,  0,  1), (-1,  0,  0), ( 0,  0,  1), 1e-6)
  // clang-format on

  // test missing the surface
  // clang-format off
  CHECK_NO_INTERSECTION(sphere, (-1.1,   -1,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(sphere, (-1.1,    1,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(sphere, ( 1.1,   -1,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(sphere, ( 1.1,    1,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(sphere, (   0, -1.1,   -1), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(sphere, (   0, -1.1,    1), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(sphere, (   0,  1.1,   -1), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(sphere, (   0,  1.1,    1), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(sphere, (  -1,    0, -1.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(sphere, (   1,    0, -1.1), (-1,  0,  0))
  CHECK_NO_INTERSECTION(sphere, (  -1,    0,  1.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(sphere, (   1,    0,  1.1), (-1,  0,  0))
  // clang-format on

  // generate some random rays outside the sphere and test them
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    sphere.setPose(pos);
    sphere.setScale(g.uniformReal(0.5, 100.0));
    sphere.setPadding(g.uniformReal(-0.1, 100.0));

    // choose a random direction
    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    // get the scaled dimensions of the sphere
    bodies::BoundingSphere s;
    sphere.computeBoundingSphere(s);

    // create origin outside the sphere in the opposite direction than the chosen one
    const Eigen::Vector3d origin = s.center + -dir * 1.5 * s.radius;

    // a ray constructed in this way should intersect twice (through the sphere center)
    EigenSTL::vector_Vector3d intersections;
    if (sphere.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        EXPECT_NEAR(s.radius, (s.center - intersections[0]).norm(), 1e-4);
        EXPECT_NEAR(s.radius, (s.center - intersections[1]).norm(), 1e-4);

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }

    // check that the opposite ray misses
    CHECK_NO_INTERSECTION(sphere, (origin), (-dir))  // NOLINT(performance-unnecessary-copy-initialization)

    // shift the ray a bit sideways

    // choose a perpendicular direction
    Eigen::Vector3d perpDir = dir.cross(Eigen::Vector3d({ dir.z(), dir.z(), -dir.x() - dir.y() }));
    if (perpDir.norm() < 1e-6)
      perpDir = dir.cross(Eigen::Vector3d({ -dir.y() - dir.z(), dir.x(), dir.x() }));
    perpDir.normalize();

    // now move origin "sideways" but still only so much that the ray will hit the sphere
    const Eigen::Vector3d origin2 = origin + g.uniformReal(1e-6, s.radius - 1e-4) * perpDir;

    intersections.clear();
    if (sphere.intersectsRay(origin2, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        EXPECT_NEAR(s.radius, (s.center - intersections[0]).norm(), 1e-4);
        EXPECT_NEAR(s.radius, (s.center - intersections[1]).norm(), 1e-4);

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin2).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin2).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(SphereRayIntersection, SimpleRay1)
{
  shapes::Sphere shape(1.0);
  bodies::Sphere sphere(&shape);
  sphere.setScale(1.05);

  CHECK_INTERSECTS_TWICE(sphere, (5, 0, 0), (-1, 0, 0), (1.05, 0, 0), (-1.05, 0, 0), 1e-6)
}

TEST(SphereRayIntersection, SimpleRay2)
{
  shapes::Sphere shape(1.0);
  bodies::Sphere sphere(&shape);
  sphere.setScale(1.05);

  CHECK_NO_INTERSECTION(sphere, (5, 0, 0), (1, 0, 0))
}

TEST(CylinderRayIntersection, OriginInside)
{
  shapes::Cylinder shape(1.0, 2.0);
  bodies::Cylinder cylinder(&shape);

  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), (-1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0, -1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  0,  1), ( 0,  0,  1), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  0, -1), ( 0,  0, -1), 1e-6)
  // clang-format on

  // scaling

  cylinder.setScale(1.1);
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move origin within the cylinder

  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (0.5, 0  , 0  ), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0.5, 0  , 0  ), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0  , 0.5, 0  ), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0  , 0.5, 0  ), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0  , 0  , 0.5), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0  , 0  , 0.5), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move cylinder

  Eigen::Isometry3d pose = Eigen::Translation3d(0.5, 0, 0) * Eigen::Quaterniond::Identity();
  cylinder.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 1, 0, 0), ( 1.6, 0, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), (-1, 0, 0), (-0.6, 0, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0.5, 0) * Eigen::Quaterniond::Identity();
  cylinder.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0,  1, 0), (0,  1.6, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0, -1, 0), (0, -0.6, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0, 0.5) * Eigen::Quaterniond::Identity();
  cylinder.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0, 0,  1), (0, 0,  1.6), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (0, 0, 0), ( 0, 0, -1), (0, 0, -0.6), 1e-6)
  // clang-format on

  // 3D diagonal

  cylinder.setPose(Eigen::Isometry3d::Identity());
  cylinder.setScale(1.0);
  cylinder.setPadding(0.1);

  // diagonal distance to the base boundary
  const auto sq2 = sqrt(pow(1 + 0.1, 2) / 2);
  // direction towards the very "corner" of the cylinder
  const auto dir3 = Eigen::Vector3d({ sq2, sq2, 1.1 }).normalized();
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, ( 0,  0,  0), ( dir3), ( sq2,  sq2,  1.1), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(cylinder, (  dir3 / 2), ( dir3), ( sq2,  sq2,  1.1), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(cylinder, ( -dir3 / 2), ( dir3), ( sq2,  sq2,  1.1), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(cylinder, ( 0,  0,  0), (-dir3), (-sq2, -sq2, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (  dir3 / 2), (-dir3), (-sq2, -sq2, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, ( -dir3 / 2), (-dir3), (-sq2, -sq2, -1.1), 1e-4)
  // clang-format on

  // 2D diagonal

  // coordinate of the diagonal direction in the base
  const auto dir2 = 1 / sqrt(2);
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (   0,    0,    0), ( dir2,  dir2,     0), ( sq2,  sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0,    0,    0), (-dir2, -dir2,     0), (-sq2, -sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, ( 0.5,  0.5,    0), ( dir2,  dir2,     0), ( sq2,  sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, ( 0.5,  0.5,    0), (-dir2, -dir2,     0), (-sq2, -sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (-0.5, -0.5,    0), ( dir2,  dir2,     0), ( sq2,  sq2,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (-0.5, -0.5,    0), (-dir2, -dir2,     0), (-sq2, -sq2,    0), 1e-4)

  CHECK_INTERSECTS_ONCE(cylinder, (   0,    0,    0), (    0,  dir2,  dir2), (   0,  1.1,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0,    0,    0), (    0, -dir2, -dir2), (   0, -1.1, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0,  0.5,  0.5), (    0,  dir2,  dir2), (   0,  1.1,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0,  0.5,  0.5), (    0, -dir2, -dir2), (   0, -1.1, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0, -0.5, -0.5), (    0,  dir2,  dir2), (   0,  1.1,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0, -0.5, -0.5), (    0, -dir2, -dir2), (   0, -1.1, -1.1), 1e-4)

  CHECK_INTERSECTS_ONCE(cylinder, (   0,    0,    0), ( dir2,     0,  dir2), ( 1.1,    0,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (   0,    0,    0), (-dir2,     0, -dir2), (-1.1,    0, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, ( 0.5,    0,  0.5), ( dir2,     0,  dir2), ( 1.1,    0,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, ( 0.5,    0,  0.5), (-dir2,     0, -dir2), (-1.1,    0, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (-0.5,    0, -0.5), ( dir2,     0,  dir2), ( 1.1,    0,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(cylinder, (-0.5,    0, -0.5), (-dir2,     0, -dir2), (-1.1,    0, -1.1), 1e-4)
  // clang-format on

  // any random rays that start inside the cylinder should have exactly one intersection with it
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    cylinder.setPose(pos);
    cylinder.setScale(g.uniformReal(0.5, 100.0));
    cylinder.setPadding(g.uniformReal(-0.1, 100.0));

    Eigen::Vector3d origin;
    cylinder.samplePointInside(g, 10, origin);

    // get the bounding sphere of the scaled cylinder
    bodies::BoundingSphere s;
    cylinder.computeBoundingSphere(s);

    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    EigenSTL::vector_Vector3d intersections;
    if (cylinder.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(1u, intersections.size());
      if (intersections.size() == 1)
      {
        EXPECT_GE(s.radius, (s.center - intersections[0]).norm());
        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(CylinderRayIntersection, OriginOutside)
{
  shapes::Cylinder shape(1.0, 2.0);
  bodies::Cylinder cylinder(&shape);
  cylinder.setScale(1.5);
  cylinder.setPadding(0.5);

  // clang-format off
  CHECK_INTERSECTS_TWICE(cylinder, (-4,  0,  0), ( 1,  0,  0), ( 2,  0,  0), (-2,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 4,  0,  0), (-1,  0,  0), (-2,  0,  0), ( 2,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0, -4,  0), ( 0,  1,  0), ( 0,  2,  0), ( 0, -2,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0,  4,  0), ( 0, -1,  0), ( 0, -2,  0), ( 0,  2,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0,  0, -4), ( 0,  0,  1), ( 0,  0,  2), ( 0,  0, -2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0,  0,  4), ( 0,  0, -1), ( 0,  0, -2), ( 0,  0,  2), 1e-6)
  // clang-format on

  // test hitting the surface
  // clang-format off
  CHECK_INTERSECTS_ONCE(cylinder, (-2, -2,  0), ( 0,  1,  0), (-2,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (-2,  2,  0), ( 0, -1,  0), (-2,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, ( 2, -2,  0), ( 0,  1,  0), ( 2,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, ( 2,  2,  0), ( 0, -1,  0), ( 2,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (-2, -2,  0), ( 1,  0,  0), ( 0, -2,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, ( 2, -2,  0), (-1,  0,  0), ( 0, -2,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, (-2,  2,  0), ( 1,  0,  0), ( 0,  2,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(cylinder, ( 2,  2,  0), (-1,  0,  0), ( 0,  2,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0, -2, -3), ( 0,  0,  1), ( 0, -2, -2), ( 0, -2,  2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0, -2,  3), ( 0,  0, -1), ( 0, -2,  2), ( 0, -2, -2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0,  2, -3), ( 0,  0,  1), ( 0,  2, -2), ( 0,  2,  2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 0,  2,  3), ( 0,  0, -1), ( 0,  2,  2), ( 0,  2, -2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, (-2,  0, -3), ( 0,  0,  1), (-2,  0, -2), (-2,  0,  2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, (-2,  0,  3), ( 0,  0, -1), (-2,  0,  2), (-2,  0, -2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 2,  0, -3), ( 0,  0,  1), ( 2,  0, -2), ( 2,  0,  2), 1e-6)
  CHECK_INTERSECTS_TWICE(cylinder, ( 2,  0,  3), ( 0,  0, -1), ( 2,  0,  2), ( 2,  0, -2), 1e-6)
  // clang-format on

  // test missing the surface
  // clang-format off
  CHECK_NO_INTERSECTION(cylinder, (-2.1,   -1,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(cylinder, (-2.1,    1,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(cylinder, ( 2.1,   -1,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(cylinder, ( 2.1,    1,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(cylinder, (   0, -2.1,   -2), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(cylinder, (   0, -2.1,    2), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(cylinder, (   0,  2.1,   -2), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(cylinder, (   0,  2.1,    2), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(cylinder, (  -2,    0, -2.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(cylinder, (   2,    0, -2.1), (-1,  0,  0))
  CHECK_NO_INTERSECTION(cylinder, (  -2,    0,  2.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(cylinder, (   2,    0,  2.1), (-1,  0,  0))
  // clang-format on

  // generate some random rays outside the cylinder and test them
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    cylinder.setPose(pos);
    cylinder.setScale(g.uniformReal(0.5, 100.0));
    cylinder.setPadding(g.uniformReal(-0.1, 100.0));

    // choose a random direction
    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    // get the bounding sphere of the cylinder
    bodies::BoundingSphere s;
    cylinder.computeBoundingSphere(s);

    // create origin outside the cylinder in the opposite direction than the chosen one
    const Eigen::Vector3d origin = s.center + -dir * 1.5 * s.radius;

    // a ray constructed in this way should intersect twice (through the cylinder center)
    EigenSTL::vector_Vector3d intersections;
    if (cylinder.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        EXPECT_GE(s.radius, (s.center - intersections[0]).norm());
        EXPECT_GE(s.radius, (s.center - intersections[1]).norm());

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }

    // check that the opposite ray misses
    CHECK_NO_INTERSECTION(cylinder, (origin), (-dir))  // NOLINT(performance-unnecessary-copy-initialization)

    // shift the ray a bit sideways

    // choose a perpendicular direction
    Eigen::Vector3d perpDir = dir.cross(Eigen::Vector3d({ dir.z(), dir.z(), -dir.x() - dir.y() }));
    if (perpDir.norm() < 1e-6)
      perpDir = dir.cross(Eigen::Vector3d({ -dir.y() - dir.z(), dir.x(), dir.x() }));
    perpDir.normalize();

    // get the scaled cylinder
    bodies::BoundingCylinder c;
    cylinder.computeBoundingCylinder(c);

    // now move origin "sideways" but still only so much that the ray will hit the cylinder
    auto minRadius = (std::min)(c.radius, c.length);
    const Eigen::Vector3d origin2 = origin + g.uniformReal(1e-6, minRadius - 1e-4) * perpDir;

    intersections.clear();
    if (cylinder.intersectsRay(origin2, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        EXPECT_GT(s.radius, (s.center - intersections[0]).norm());
        EXPECT_GT(s.radius, (s.center - intersections[1]).norm());
        EXPECT_LT(minRadius, (s.center - intersections[0]).norm());
        EXPECT_LT(minRadius, (s.center - intersections[1]).norm());

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin2).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin2).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(CylinderRayIntersection, SimpleRay1)
{
  shapes::Cylinder shape(1.0, 2.0);
  bodies::Cylinder cylinder(&shape);
  cylinder.setScale(1.05);

  CHECK_INTERSECTS_TWICE(cylinder, (5, 0, 0), (-1, 0, 0), (1.05, 0, 0), (-1.05, 0, 0), 1e-6)
}

TEST(CylinderRayIntersection, SimpleRay2)
{
  shapes::Cylinder shape(1.0, 2.0);
  bodies::Cylinder cylinder(&shape);
  cylinder.setScale(1.05);

  CHECK_NO_INTERSECTION(cylinder, (5, 0, 0), (1, 0, 0))
}

TEST(BoxRayIntersection, SimpleRay1)
{
  shapes::Box shape(1.0, 1.0, 3.0);
  bodies::Box box(&shape);
  box.setScale(0.95);

  CHECK_INTERSECTS_TWICE(box, (10, 0.449, 0), (-1, 0, 0), (0.475, 0.449, 0), (-0.475, 0.449, 0), 1e-4)
}

TEST(BoxRayIntersection, SimpleRay2)
{
  shapes::Box shape(0.9, 0.01, 1.2);
  bodies::Box box(&shape);

  Eigen::Isometry3d pose(Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()));
  pose.translation() = Eigen::Vector3d(0, 0.005, 0.6);
  box.setPose(pose);

  const Eigen::Vector3d ray_d(0, -5.195, -0.77);

  CHECK_INTERSECTS(box, (0, 5, 1.6), (ray_d.normalized()), 2)
}

TEST(BoxRayIntersection, SimpleRay3)
{
  shapes::Box shape(0.02, 0.4, 1.2);
  bodies::Box box(&shape);

  Eigen::Isometry3d pose(Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()));
  pose.translation() = Eigen::Vector3d(0.45, -0.195, 0.6);
  box.setPose(pose);

  const Eigen::Vector3d ray_d(0, 1.8, -0.669);

  CHECK_NO_INTERSECTION(box, (0, -2, 1.11), (ray_d.normalized()))
}

TEST(BoxRayIntersection, Regression109)
{
  shapes::Box shape(1.0, 1.0, 1.0);
  bodies::Box box(&shape);

  // rotates the box so that the original (0.5, 0.5, 0.5) corner gets elsewhere and is no longer the
  // maximal corner
  const auto rot = Eigen::AngleAxisd(M_PI * 2 / 3, Eigen::Vector3d(1.0, -1.0, 1.0).normalized());
  box.setPose(Eigen::Isometry3d(rot));

  CHECK_INTERSECTS_TWICE(box, (-2, 0, 0), (1, 0, 0), (0.5, 0, 0), (-0.5, 0, 0), 1e-6)
}

TEST(BoxRayIntersection, OriginInside)
{
  shapes::Box shape(2.0, 2.0, 2.0);
  bodies::Box box(&shape);

  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), (-1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0, -1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  0,  1), ( 0,  0,  1), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  0, -1), ( 0,  0, -1), 1e-6)
  // clang-format on

  // scaling

  box.setScale(1.1);
  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move origin within the box

  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (0.5, 0  , 0  ), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0.5, 0  , 0  ), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0  , 0.5, 0  ), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0  , 0.5, 0  ), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0  , 0  , 0.5), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0  , 0  , 0.5), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move box

  Eigen::Isometry3d pose(Eigen::Translation3d(0.5, 0, 0));
  box.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 1, 0, 0), ( 1.6, 0, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), (-1, 0, 0), (-0.6, 0, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0.5, 0);
  box.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0,  1, 0), (0,  1.6, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0, -1, 0), (0, -0.6, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0, 0.5);
  box.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0, 0,  1), (0, 0,  1.6), 1e-6)
  CHECK_INTERSECTS_ONCE(box, (0, 0, 0), ( 0, 0, -1), (0, 0, -0.6), 1e-6)
  // clang-format on

  // 3D diagonal

  box.setPose(Eigen::Isometry3d::Identity());
  box.setScale(1.0);
  box.setPadding(0.1);

  const auto dir3 = Eigen::Vector3d::Ones().normalized();
  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), ( dir3), ( 1.1,  1.1,  1.1), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(box, ( 0.5,  0.5,  0.5), ( dir3), ( 1.1,  1.1,  1.1), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(box, (-0.5, -0.5, -0.5), ( dir3), ( 1.1,  1.1,  1.1), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), (-dir3), (-1.1, -1.1, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, ( 0.5,  0.5,  0.5), (-dir3), (-1.1, -1.1, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (-0.5, -0.5, -0.5), (-dir3), (-1.1, -1.1, -1.1), 1e-4)
  // clang-format on

  // 2D diagonal

  const auto dir2 = 1 / sqrt(2);
  // clang-format off
  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), ( dir2,  dir2,     0), ( 1.1,  1.1,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), (-dir2, -dir2,     0), (-1.1, -1.1,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(box, ( 0.5,  0.5,    0), ( dir2,  dir2,     0), ( 1.1,  1.1,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(box, ( 0.5,  0.5,    0), (-dir2, -dir2,     0), (-1.1, -1.1,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (-0.5, -0.5,    0), ( dir2,  dir2,     0), ( 1.1,  1.1,    0), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (-0.5, -0.5,    0), (-dir2, -dir2,     0), (-1.1, -1.1,    0), 1e-4)

  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), (    0,  dir2,  dir2), (   0,  1.1,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), (    0, -dir2, -dir2), (   0, -1.1, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0,  0.5,  0.5), (    0,  dir2,  dir2), (   0,  1.1,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0,  0.5,  0.5), (    0, -dir2, -dir2), (   0, -1.1, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0, -0.5, -0.5), (    0,  dir2,  dir2), (   0,  1.1,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0, -0.5, -0.5), (    0, -dir2, -dir2), (   0, -1.1, -1.1), 1e-4)

  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), ( dir2,     0,  dir2), ( 1.1,    0,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (   0,    0,    0), (-dir2,     0, -dir2), (-1.1,    0, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, ( 0.5,    0,  0.5), ( dir2,     0,  dir2), ( 1.1,    0,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, ( 0.5,    0,  0.5), (-dir2,     0, -dir2), (-1.1,    0, -1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (-0.5,    0, -0.5), ( dir2,     0,  dir2), ( 1.1,    0,  1.1), 1e-4)
  CHECK_INTERSECTS_ONCE(box, (-0.5,    0, -0.5), (-dir2,     0, -dir2), (-1.1,    0, -1.1), 1e-4)
  // clang-format on

  // any random rays that start inside the box should have exactly one intersection with it
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    box.setPose(pos);
    box.setScale(g.uniformReal(0.5, 100.0));
    box.setPadding(g.uniformReal(-0.1, 100.0));

    // get the scaled dimensions of the box
    Eigen::Vector3d sizes = { box.getDimensions()[0], box.getDimensions()[1], box.getDimensions()[2] };
    sizes *= box.getScale();
    sizes += 2 * box.getPadding() * Eigen::Vector3d::Ones();

    // radii of the inscribed and bounding spheres
    const auto minRadius = sizes.minCoeff() / 2;
    const auto maxRadius = (sizes / 2).norm();

    const Eigen::Vector3d boxCenter = box.getPose().translation();

    Eigen::Vector3d origin;
    box.samplePointInside(g, 10, origin);

    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    EigenSTL::vector_Vector3d intersections;
    if (box.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(1u, intersections.size());
      if (intersections.size() == 1)
      {
        // just approximate verification that the point is between inscribed and bounding sphere
        EXPECT_GE(maxRadius, (boxCenter - intersections[0]).norm());
        EXPECT_LE(minRadius, (boxCenter - intersections[0]).norm());
        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(BoxRayIntersection, OriginOutsideIntersects)
{
  shapes::Box shape(2.0, 2.0, 2.0);
  bodies::Box box(&shape);

  // clang-format off
  CHECK_INTERSECTS_TWICE(box, (-2, 0, 0), ( 1,  0,  0), ( 1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 2, 0, 0), (-1,  0,  0), (-1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (0, -2, 0), ( 0,  1,  0), ( 0,  1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (0,  2, 0), ( 0, -1,  0), ( 0, -1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (0, 0, -2), ( 0,  0,  1), ( 0,  0,  1), ( 0,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (0, 0,  2), ( 0,  0, -1), ( 0,  0, -1), ( 0,  0,  1), 1e-6)
  // clang-format on

  // test hitting the surface
  // clang-format off
  CHECK_INTERSECTS_TWICE(box, (-1, -2,  0), ( 0,  1,  0), (-1, -1,  0), (-1,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (-1,  2,  0), ( 0, -1,  0), (-1,  1,  0), (-1, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 1, -2,  0), ( 0,  1,  0), ( 1, -1,  0), ( 1,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 1,  2,  0), ( 0, -1,  0), ( 1,  1,  0), ( 1, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 0, -1, -2), ( 0,  0,  1), ( 0, -1, -1), ( 0, -1,  1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 0, -1,  2), ( 0,  0, -1), ( 0, -1,  1), ( 0, -1, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 0,  1, -2), ( 0,  0,  1), ( 0,  1, -1), ( 0,  1,  1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 0,  1,  2), ( 0,  0, -1), ( 0,  1,  1), ( 0,  1, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (-2,  0, -1), ( 1,  0,  0), (-1,  0, -1), ( 1,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 2,  0, -1), (-1,  0,  0), ( 1,  0, -1), (-1,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, (-2,  0,  1), ( 1,  0,  0), (-1,  0,  1), ( 1,  0,  1), 1e-6)
  CHECK_INTERSECTS_TWICE(box, ( 2,  0,  1), (-1,  0,  0), ( 1,  0,  1), (-1,  0,  1), 1e-6)
  // clang-format on

  // test missing the surface
  // clang-format off
  CHECK_NO_INTERSECTION(box, (-1.1,   -2,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(box, (-1.1,    2,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(box, ( 1.1,   -2,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(box, ( 1.1,    2,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(box, (   0, -1.1,   -2), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(box, (   0, -1.1,    2), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(box, (   0,  1.1,   -2), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(box, (   0,  1.1,    2), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(box, (  -2,    0, -1.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(box, (   2,    0, -1.1), (-1,  0,  0))
  CHECK_NO_INTERSECTION(box, (  -2,    0,  1.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(box, (   2,    0,  1.1), (-1,  0,  0))
  // clang-format on

  // generate some random rays outside the box and test them
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    box.setPose(pos);
    box.setScale(g.uniformReal(0.5, 100.0));
    box.setPadding(g.uniformReal(-0.1, 100.0));

    // choose a random direction
    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    // get the scaled dimensions of the box
    Eigen::Vector3d sizes = { box.getDimensions()[0], box.getDimensions()[1], box.getDimensions()[2] };
    sizes *= box.getScale();
    sizes += 2 * box.getPadding() * Eigen::Vector3d::Ones();

    // radii of the inscribed and bounding spheres
    const auto minRadius = sizes.minCoeff() / 2;
    const auto maxRadius = (sizes / 2).norm();

    const Eigen::Vector3d boxCenter = box.getPose().translation();

    // create origin outside the box in the opposite direction than the chosen one
    const Eigen::Vector3d origin = boxCenter + -dir * 1.5 * maxRadius;

    // a ray constructed in this way should intersect twice (through the box center)
    EigenSTL::vector_Vector3d intersections;
    if (box.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        // just approximate verification that the point is between inscribed and bounding sphere
        EXPECT_GE(maxRadius, (boxCenter - intersections[0]).norm());
        EXPECT_LE(minRadius, (boxCenter - intersections[0]).norm());
        EXPECT_GE(maxRadius, (boxCenter - intersections[1]).norm());
        EXPECT_LE(minRadius, (boxCenter - intersections[1]).norm());

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }

    // check that the opposite ray misses
    CHECK_NO_INTERSECTION(box, (origin), (-dir))  // NOLINT(performance-unnecessary-copy-initialization)

    // shift the ray a bit sideways

    // choose a perpendicular direction
    Eigen::Vector3d perpDir = dir.cross(Eigen::Vector3d({ dir.z(), dir.z(), -dir.x() - dir.y() }));
    if (perpDir.norm() < 1e-6)
      perpDir = dir.cross(Eigen::Vector3d({ -dir.y() - dir.z(), dir.x(), dir.x() }));
    perpDir.normalize();

    // now move origin "sideways" but still only so much that the ray will hit the box
    const Eigen::Vector3d origin2 = origin + g.uniformReal(1e-6, minRadius - 1e-4) * perpDir;

    intersections.clear();
    if (box.intersectsRay(origin2, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        // just approximate verification that the point is between inscribed and bounding sphere
        EXPECT_GE(maxRadius, (boxCenter - intersections[0]).norm());
        EXPECT_LE(minRadius, (boxCenter - intersections[0]).norm());
        EXPECT_GE(maxRadius, (boxCenter - intersections[1]).norm());
        EXPECT_LE(minRadius, (boxCenter - intersections[1]).norm());

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin2).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin2).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(ConvexMeshRayIntersection, SimpleRay1)
{
  shapes::Box box(1.0, 1.0, 3.0);
  shapes::Mesh* shape = shapes::createMeshFromShape(&box);
  bodies::ConvexMesh mesh(shape);
  delete shape;
  mesh.setScale(0.95);  // NOLINT(performance-unnecessary-copy-initialization)

  CHECK_INTERSECTS_TWICE(mesh, (10, 0.449, 0), (-1, 0, 0), (0.475, 0.449, 0), (-0.475, 0.449, 0), 1e-4)
}

TEST(ConvexMeshRayIntersection, SimpleRay2)
{
  shapes::Box box(0.9, 0.01, 1.2);
  shapes::Mesh* shape = shapes::createMeshFromShape(&box);
  bodies::ConvexMesh mesh(shape);
  delete shape;

  Eigen::Isometry3d pose(Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()));
  pose.translation() = Eigen::Vector3d(0, 0.005, 0.6);
  mesh.setPose(pose);

  const Eigen::Vector3d ray_d(0, -5.195, -0.77);

  CHECK_INTERSECTS(mesh, (0, 5, 1.6), (ray_d.normalized()), 2)
}

TEST(ConvexMeshRayIntersection, SimpleRay3)
{
  shapes::Box box(0.02, 0.4, 1.2);
  shapes::Mesh* shape = shapes::createMeshFromShape(&box);
  bodies::ConvexMesh mesh(shape);
  delete shape;

  Eigen::Isometry3d pose(Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX()));
  pose.translation() = Eigen::Vector3d(0.45, -0.195, 0.6);
  mesh.setPose(pose);

  const Eigen::Vector3d ray_d(0, 1.8, -0.669);

  CHECK_NO_INTERSECTION(mesh, (0, -2, 1.11), (ray_d.normalized()))
}

TEST(ConvexMeshRayIntersection, OriginInside)
{
  shapes::Box box(2.0, 2.0, 2.0);
  shapes::Mesh* shape = shapes::createMeshFromShape(&box);
  bodies::ConvexMesh mesh(shape);
  delete shape;

  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), (-1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0, -1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  0,  1), ( 0,  0,  1), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  0, -1), ( 0,  0, -1), 1e-6)
  // clang-format on

  // scaling

  mesh.setScale(1.1);
  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move origin within the mesh

  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (0.5, 0  , 0  ), ( 1,  0,  0), ( 1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0.5, 0  , 0  ), (-1,  0,  0), (-1.1,    0,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0  , 0.5, 0  ), ( 0,  1,  0), ( 0,    1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0  , 0.5, 0  ), ( 0, -1,  0), ( 0,   -1.1,    0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0  , 0  , 0.5), ( 0,  0,  1), ( 0,      0,  1.1), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0  , 0  , 0.5), ( 0,  0, -1), ( 0,      0, -1.1), 1e-6)
  // clang-format on

  // move mesh

  Eigen::Isometry3d pose(Eigen::Translation3d(0.5, 0, 0));
  mesh.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 1, 0, 0), ( 1.6, 0, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), (-1, 0, 0), (-0.6, 0, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0.5, 0);
  mesh.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0,  1, 0), (0,  1.6, 0), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0, -1, 0), (0, -0.6, 0), 1e-6)
  // clang-format on

  pose = Eigen::Translation3d(0, 0, 0.5);
  mesh.setPose(pose);
  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0, 0,  1), (0, 0,  1.6), 1e-6)
  CHECK_INTERSECTS_ONCE(mesh, (0, 0, 0), ( 0, 0, -1), (0, 0, -0.6), 1e-6)
  // clang-format on

  // 3D diagonal

  mesh.setPose(Eigen::Isometry3d::Identity());
  mesh.setScale(1.0);
  mesh.setPadding(0.1);

  // half-size of the scaled and padded mesh (mesh scaling isn't "linear" in padding)
  const double s = 1.0 + 0.1 / sqrt(3);

  const auto dir3 = Eigen::Vector3d::Ones().normalized();
  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), ( dir3), ( s,  s,  s), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(mesh, ( 0.5,  0.5,  0.5), ( dir3), ( s,  s,  s), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(mesh, (-0.5, -0.5, -0.5), ( dir3), ( s,  s,  s), 1e-4) // NOLINT(performance-unnecessary-copy-initialization)
  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), (-dir3), (-s, -s, -s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, ( 0.5,  0.5,  0.5), (-dir3), (-s, -s, -s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (-0.5, -0.5, -0.5), (-dir3), (-s, -s, -s), 1e-4)
  // clang-format on

  // 2D diagonal

  const auto dir2 = 1 / sqrt(2);
  // clang-format off
  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), ( dir2,  dir2,     0), ( s,  s,  0), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), (-dir2, -dir2,     0), (-s, -s,  0), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, ( 0.5,  0.5,    0), ( dir2,  dir2,     0), ( s,  s,  0), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, ( 0.5,  0.5,    0), (-dir2, -dir2,     0), (-s, -s,  0), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (-0.5, -0.5,    0), ( dir2,  dir2,     0), ( s,  s,  0), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (-0.5, -0.5,    0), (-dir2, -dir2,     0), (-s, -s,  0), 1e-4)

  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), (    0,  dir2,  dir2), ( 0,  s,  s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), (    0, -dir2, -dir2), ( 0, -s, -s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0,  0.5,  0.5), (    0,  dir2,  dir2), ( 0,  s,  s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0,  0.5,  0.5), (    0, -dir2, -dir2), ( 0, -s, -s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0, -0.5, -0.5), (    0,  dir2,  dir2), ( 0,  s,  s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0, -0.5, -0.5), (    0, -dir2, -dir2), ( 0, -s, -s), 1e-4)

  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), ( dir2,     0,  dir2), ( s,  0,  s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (   0,    0,    0), (-dir2,     0, -dir2), (-s,  0, -s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, ( 0.5,    0,  0.5), ( dir2,     0,  dir2), ( s,  0,  s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, ( 0.5,    0,  0.5), (-dir2,     0, -dir2), (-s,  0, -s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (-0.5,    0, -0.5), ( dir2,     0,  dir2), ( s,  0,  s), 1e-4)
  CHECK_INTERSECTS_ONCE(mesh, (-0.5,    0, -0.5), (-dir2,     0, -dir2), (-s,  0, -s), 1e-4)
  // clang-format on

  // any random rays that start inside the mesh should have exactly one intersection with it
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    mesh.setPose(pos);
    mesh.setScale(g.uniformReal(0.5, 100.0));
    mesh.setPadding(g.uniformReal(-0.1, 100.0));

    // get the scaled dimensions of the mesh
    Eigen::Vector3d sizes = { box.size[0], box.size[1], box.size[2] };
    sizes *= mesh.getScale();
    sizes += 2 * mesh.getPadding() / sqrt(3) * Eigen::Vector3d::Ones();

    // radii of the inscribed and bounding spheres
    const auto minRadius = sizes.minCoeff() / 2;
    const auto maxRadius = (sizes / 2).norm();

    const Eigen::Vector3d meshCenter = mesh.getPose().translation();

    Eigen::Vector3d origin;
    mesh.samplePointInside(g, 10000, origin);

    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    EigenSTL::vector_Vector3d intersections;
    if (mesh.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(1u, intersections.size());
      if (intersections.size() == 1)
      {
        // just approximate verification that the point is between inscribed and bounding sphere
        EXPECT_GE(maxRadius, (meshCenter - intersections[0]).norm());
        EXPECT_LE(minRadius, (meshCenter - intersections[0]).norm());
        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

TEST(ConvexMeshRayIntersection, OriginOutsideIntersects)
{
  shapes::Box box(2.0, 2.0, 2.0);
  shapes::Mesh* shape = shapes::createMeshFromShape(&box);
  bodies::ConvexMesh mesh(shape);
  delete shape;

  // clang-format off
  CHECK_INTERSECTS_TWICE(mesh, (-2, 0, 0), ( 1,  0,  0), ( 1,  0,  0), (-1,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 2, 0, 0), (-1,  0,  0), (-1,  0,  0), ( 1,  0,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (0, -2, 0), ( 0,  1,  0), ( 0,  1,  0), ( 0, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (0,  2, 0), ( 0, -1,  0), ( 0, -1,  0), ( 0,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (0, 0, -2), ( 0,  0,  1), ( 0,  0,  1), ( 0,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (0, 0,  2), ( 0,  0, -1), ( 0,  0, -1), ( 0,  0,  1), 1e-6)
  // clang-format on

  // test hitting the surface
  // clang-format off
  CHECK_INTERSECTS_TWICE(mesh, (-1, -2,  0), ( 0,  1,  0), (-1, -1,  0), (-1,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (-1,  2,  0), ( 0, -1,  0), (-1,  1,  0), (-1, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 1, -2,  0), ( 0,  1,  0), ( 1, -1,  0), ( 1,  1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 1,  2,  0), ( 0, -1,  0), ( 1,  1,  0), ( 1, -1,  0), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 0, -1, -2), ( 0,  0,  1), ( 0, -1, -1), ( 0, -1,  1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 0, -1,  2), ( 0,  0, -1), ( 0, -1,  1), ( 0, -1, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 0,  1, -2), ( 0,  0,  1), ( 0,  1, -1), ( 0,  1,  1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 0,  1,  2), ( 0,  0, -1), ( 0,  1,  1), ( 0,  1, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (-2,  0, -1), ( 1,  0,  0), (-1,  0, -1), ( 1,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 2,  0, -1), (-1,  0,  0), ( 1,  0, -1), (-1,  0, -1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, (-2,  0,  1), ( 1,  0,  0), (-1,  0,  1), ( 1,  0,  1), 1e-6)
  CHECK_INTERSECTS_TWICE(mesh, ( 2,  0,  1), (-1,  0,  0), ( 1,  0,  1), (-1,  0,  1), 1e-6)
  // clang-format on

  // test missing the surface
  // clang-format off
  CHECK_NO_INTERSECTION(mesh, (-1.1,   -2,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(mesh, (-1.1,    2,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(mesh, ( 1.1,   -2,    0), ( 0,  1,  0))
  CHECK_NO_INTERSECTION(mesh, ( 1.1,    2,    0), ( 0, -1,  0))
  CHECK_NO_INTERSECTION(mesh, (   0, -1.1,   -2), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(mesh, (   0, -1.1,    2), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(mesh, (   0,  1.1,   -2), ( 0,  0,  1))
  CHECK_NO_INTERSECTION(mesh, (   0,  1.1,    2), ( 0,  0, -1))
  CHECK_NO_INTERSECTION(mesh, (  -2,    0, -1.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(mesh, (   2,    0, -1.1), (-1,  0,  0))
  CHECK_NO_INTERSECTION(mesh, (  -2,    0,  1.1), ( 1,  0,  0))
  CHECK_NO_INTERSECTION(mesh, (   2,    0,  1.1), (-1,  0,  0))
  // clang-format on

  // generate some random rays outside the mesh and test them
  random_numbers::RandomNumberGenerator g(0u);
  for (size_t i = 0; i < 1000; ++i)
  {
    const Eigen::Isometry3d pos = getRandomPose(g);
    mesh.setPose(pos);
    mesh.setScale(g.uniformReal(0.5, 100.0));
    mesh.setPadding(g.uniformReal(-0.1, 100.0));

    // choose a random direction
    const Eigen::Vector3d dir = Eigen::Vector3d::Random().normalized();

    // get the scaled dimensions of the mesh
    Eigen::Vector3d sizes = { box.size[0], box.size[1], box.size[2] };
    sizes *= mesh.getScale();
    sizes += 2 * mesh.getPadding() / sqrt(3) * Eigen::Vector3d::Ones();

    // radii of the inscribed and bounding spheres
    const auto minRadius = sizes.minCoeff() / 2;
    const auto maxRadius = (sizes / 2).norm();

    const Eigen::Vector3d meshCenter = mesh.getPose().translation();

    // create origin outside the mesh in the opposite direction than the chosen one
    const Eigen::Vector3d origin = meshCenter + -dir * 1.5 * maxRadius;

    // a ray constructed in this way should intersect twice (through the mesh center)
    EigenSTL::vector_Vector3d intersections;
    if (mesh.intersectsRay(origin, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        // just approximate verification that the point is between inscribed and bounding sphere
        EXPECT_GE(maxRadius, (meshCenter - intersections[0]).norm());
        EXPECT_LE(minRadius, (meshCenter - intersections[0]).norm());
        EXPECT_GE(maxRadius, (meshCenter - intersections[1]).norm());
        EXPECT_LE(minRadius, (meshCenter - intersections[1]).norm());

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }

    // check that the opposite ray misses
    CHECK_NO_INTERSECTION(mesh, (origin), (-dir))  // NOLINT(performance-unnecessary-copy-initialization)

    // shift the ray a bit sideways

    // choose a perpendicular direction
    Eigen::Vector3d perpDir = dir.cross(Eigen::Vector3d({ dir.z(), dir.z(), -dir.x() - dir.y() }));
    if (perpDir.norm() < 1e-6)
      perpDir = dir.cross(Eigen::Vector3d({ -dir.y() - dir.z(), dir.x(), dir.x() }));
    perpDir.normalize();

    // now move origin "sideways" but still only so much that the ray will hit the mesh
    const Eigen::Vector3d origin2 = origin + g.uniformReal(1e-6, minRadius - 1e-4) * perpDir;

    intersections.clear();
    if (mesh.intersectsRay(origin2, dir, &intersections, 2))
    {
      EXPECT_EQ(2u, intersections.size());
      if (intersections.size() == 2)
      {
        // just approximate verification that the point is between inscribed and bounding sphere
        EXPECT_GE(maxRadius, (meshCenter - intersections[0]).norm());
        EXPECT_LE(minRadius, (meshCenter - intersections[0]).norm());
        EXPECT_GE(maxRadius, (meshCenter - intersections[1]).norm());
        EXPECT_LE(minRadius, (meshCenter - intersections[1]).norm());

        // verify the point lies on the ray
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[0])), 1e-6);
        EXPECT_LT(0.0, (intersections[0] - origin2).normalized().dot(dir));
        EXPECT_NEAR(0.0, (Eigen::ParametrizedLine<double, 3>(origin2, dir).distance(intersections[1])), 1e-6);
        EXPECT_LT(0.0, (intersections[1] - origin2).normalized().dot(dir));
      }
    }
    else
    {
      GTEST_NONFATAL_FAILURE_(("No intersection in iteration " + std::to_string(i)).c_str());
    }
  }
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
