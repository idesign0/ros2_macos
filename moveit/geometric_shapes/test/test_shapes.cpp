// Copyright 2019 Czech Technical University
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
//    * Neither the name of the Czech Technical University nor the names of its
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

/** \Author Martin Pecka <peckama2@fel.cvut.cz> */

#include "resources/config.h"
#include <geometric_shapes/shapes.h>
#include <geometric_shapes/shape_operations.h>
#include <gtest/gtest.h>

using namespace shapes;

TEST(Plane, ScaleAndPadd)
{
  const Plane plane(1., 1., 1., 1.0);

  auto plane2 = plane;
  EXPECT_EQ(plane2.a, plane.a);
  EXPECT_EQ(plane2.b, plane.b);
  EXPECT_EQ(plane2.c, plane.c);
  EXPECT_EQ(plane2.d, plane.d);

  plane2.scale(2.0);
  EXPECT_EQ(plane2.a, plane.a);
  EXPECT_EQ(plane2.b, plane.b);
  EXPECT_EQ(plane2.c, plane.c);
  EXPECT_EQ(plane2.d, plane.d);

  plane2.padd(1.0);
  EXPECT_EQ(plane2.a, plane.a);
  EXPECT_EQ(plane2.b, plane.b);
  EXPECT_EQ(plane2.c, plane.c);
  EXPECT_EQ(plane2.d, plane.d);

  plane2.scaleAndPadd(2.0, 1.0);
  EXPECT_EQ(plane2.a, plane.a);
  EXPECT_EQ(plane2.b, plane.b);
  EXPECT_EQ(plane2.c, plane.c);
  EXPECT_EQ(plane2.d, plane.d);
}

TEST(OcTree, ScaleAndPaddEmpty)
{
  // just test that scaling/padding an empty octree doesn't throw
  OcTree octree;
  octree.scale(2.0);
  octree.padd(1.0);
  octree.scaleAndPadd(2.0, 1.0);
}

TEST(Sphere, ScaleAndPadd)
{
  const Sphere sphere(1.);

  auto sphere2 = sphere;
  EXPECT_EQ(sphere2.radius, sphere.radius);

  sphere2.scale(2.0);
  EXPECT_DOUBLE_EQ(sphere2.radius, 2.0);

  sphere2.padd(1.0);
  EXPECT_DOUBLE_EQ(sphere2.radius, 3.0);

  sphere2.scaleAndPadd(2.0, 1.0);
  EXPECT_DOUBLE_EQ(sphere2.radius, 7.0);
}

TEST(Cylinder, ScaleAndPadd)
{
  const Cylinder cylinder(1., 2.);

  auto cylinder2 = cylinder;
  EXPECT_EQ(cylinder2.radius, cylinder.radius);
  EXPECT_EQ(cylinder2.length, cylinder.length);

  cylinder2.scale(2.0);
  EXPECT_DOUBLE_EQ(cylinder2.radius, 2.0);
  EXPECT_DOUBLE_EQ(cylinder2.length, 4.0);

  cylinder2.padd(1.0);
  EXPECT_DOUBLE_EQ(cylinder2.radius, 3.0);
  EXPECT_DOUBLE_EQ(cylinder2.length, 6.0);

  cylinder2.scaleAndPadd(2.0, 1.0);
  EXPECT_DOUBLE_EQ(cylinder2.radius, 7.0);
  EXPECT_DOUBLE_EQ(cylinder2.length, 14.0);

  cylinder2.scaleAndPadd(1.0, 3.0, 1.0, 3.0);
  EXPECT_DOUBLE_EQ(cylinder2.radius, 8.0);
  EXPECT_DOUBLE_EQ(cylinder2.length, 48.0);

  cylinder2.scale(2.0, 1.5);
  EXPECT_DOUBLE_EQ(cylinder2.radius, 16.0);
  EXPECT_DOUBLE_EQ(cylinder2.length, 72.0);

  cylinder2.padd(2.0, 3.0);
  EXPECT_DOUBLE_EQ(cylinder2.radius, 18.0);
  EXPECT_DOUBLE_EQ(cylinder2.length, 78.0);
}

TEST(Cone, ScaleAndPadd)
{
  const Cone cone(1., 2.);

  auto cone2 = cone;
  EXPECT_EQ(cone2.radius, cone.radius);
  EXPECT_EQ(cone2.length, cone.length);

  cone2.scale(2.0);
  EXPECT_DOUBLE_EQ(cone2.radius, 2.0);
  EXPECT_DOUBLE_EQ(cone2.length, 4.0);

  cone2.padd(1.0);
  EXPECT_DOUBLE_EQ(cone2.radius, 3.0);
  EXPECT_DOUBLE_EQ(cone2.length, 6.0);

  cone2.scaleAndPadd(2.0, 1.0);
  EXPECT_DOUBLE_EQ(cone2.radius, 7.0);
  EXPECT_DOUBLE_EQ(cone2.length, 14.0);

  cone2.scaleAndPadd(1.0, 3.0, 1.0, 3.0);
  EXPECT_DOUBLE_EQ(cone2.radius, 8.0);
  EXPECT_DOUBLE_EQ(cone2.length, 48.0);

  cone2.scale(2.0, 1.5);
  EXPECT_DOUBLE_EQ(cone2.radius, 16.0);
  EXPECT_DOUBLE_EQ(cone2.length, 72.0);

  cone2.padd(2.0, 3.0);
  EXPECT_DOUBLE_EQ(cone2.radius, 18.0);
  EXPECT_DOUBLE_EQ(cone2.length, 78.0);
}

TEST(Box, ScaleAndPadd)
{
  const Box box(1., 2., 3.0);

  auto box2 = box;
  EXPECT_EQ(box2.size[0], box.size[0]);
  EXPECT_EQ(box2.size[1], box.size[1]);
  EXPECT_EQ(box2.size[2], box.size[2]);

  box2.scale(2.0);
  EXPECT_DOUBLE_EQ(box2.size[0], 2.0);
  EXPECT_DOUBLE_EQ(box2.size[1], 4.0);
  EXPECT_DOUBLE_EQ(box2.size[2], 6.0);

  box2.padd(1.0);
  EXPECT_DOUBLE_EQ(box2.size[0], 4.0);
  EXPECT_DOUBLE_EQ(box2.size[1], 6.0);
  EXPECT_DOUBLE_EQ(box2.size[2], 8.0);

  box2.scaleAndPadd(2.0, 1.0);
  EXPECT_DOUBLE_EQ(box2.size[0], 10.0);
  EXPECT_DOUBLE_EQ(box2.size[1], 14.0);
  EXPECT_DOUBLE_EQ(box2.size[2], 18.0);

  box2.scaleAndPadd(1.0, 2.0, 3.0, 1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(box2.size[0], 12.0);
  EXPECT_DOUBLE_EQ(box2.size[1], 32.0);
  EXPECT_DOUBLE_EQ(box2.size[2], 60.0);

  box2.scale(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(box2.size[0], 12.0);
  EXPECT_DOUBLE_EQ(box2.size[1], 64.0);
  EXPECT_DOUBLE_EQ(box2.size[2], 180.0);

  box2.padd(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(box2.size[0], 14.0);
  EXPECT_DOUBLE_EQ(box2.size[1], 68.0);
  EXPECT_DOUBLE_EQ(box2.size[2], 186.0);
}

TEST(Mesh, ScaleAndPadd)
{
  std::string path = "file://" + std::string(TEST_RESOURCES_DIR) + "/box.dae";
  const auto& mesh = shapes::createMeshFromResource(path);

  ASSERT_EQ(mesh->vertex_count, 8u);

  auto mesh2 = mesh;
  EXPECT_DOUBLE_EQ(mesh2->vertices[0], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], 1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], 1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], 1.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], 1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -1.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], 1.0);

  mesh2->scale(2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[0], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], 2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], 2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], 2.0);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], 2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -2.0);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], 2.0);

  // for a right-angled corner, the vertex normal vector points away equally from the three sides, and hence
  // padding is applied equally in x, y and z, such that the total distance the vertex moves is equal to 1.0.
  mesh2->padd(1.0);
  const double pos = 2.0 + 1.0 / sqrt(3);

  EXPECT_DOUBLE_EQ(mesh2->vertices[0], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], pos);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -pos);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], pos);

  mesh2->scaleAndPadd(2.0, 1.0);
  const double pos2 = pos * 2.0 + 1.0 / sqrt(3);

  EXPECT_DOUBLE_EQ(mesh2->vertices[0], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], pos2);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -pos2);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], pos2);

  mesh2->scaleAndPadd(1.0, 2.0, 3.0, 1.0, 2.0, 3.0);
  const double pos3x = pos2 * (1.0 + 1.0 / sqrt(3 * pos2 * pos2));
  const double pos3y = pos2 * (2.0 + 2.0 / sqrt(3 * pos2 * pos2));
  const double pos3z = pos2 * (3.0 + 3.0 / sqrt(3 * pos2 * pos2));

  EXPECT_DOUBLE_EQ(mesh2->vertices[0], pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], pos3z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], pos3x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -pos3y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], pos3z);

  mesh2->scale(1.0, 2.0, 3.0);
  const double pos4x = pos3x;
  const double pos4y = 2 * pos3y;
  const double pos4z = 3 * pos3z;

  EXPECT_DOUBLE_EQ(mesh2->vertices[0], pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], pos4z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], pos4x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -pos4y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], pos4z);

  mesh2->padd(1.0, 2.0, 3.0);
  const double pos5x = pos4x + (1.0 / sqrt(3));
  const double pos5y = pos4y + (2.0 / sqrt(3));
  const double pos5z = pos4z + (3.0 / sqrt(3));

  EXPECT_DOUBLE_EQ(mesh2->vertices[0], pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[1], pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[2], -pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[3], pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[4], -pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[5], -pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[6], -pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[7], -pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[8], -pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[9], -pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[10], pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[11], -pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[12], pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[13], pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[14], pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[15], -pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[16], pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[17], pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[18], -pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[19], -pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[20], pos5z);

  EXPECT_DOUBLE_EQ(mesh2->vertices[21], pos5x);
  EXPECT_DOUBLE_EQ(mesh2->vertices[22], -pos5y);
  EXPECT_DOUBLE_EQ(mesh2->vertices[23], pos5z);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
