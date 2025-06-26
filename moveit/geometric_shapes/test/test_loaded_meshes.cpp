// Copyright 2016 Jorge Santos
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
//    * Neither the name of the Jorge Santos nor the names of its
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

/** \Author Jorge Santos */

#include <geometric_shapes/bodies.h>
#include <geometric_shapes/shape_operations.h>
#include <geometric_shapes/body_operations.h>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include "resources/config.h"

/**
 * Test fixture that generates meshes from the primitive shapes SPHERE, CYLINDER, CONE and BOX,
 * and load their twins from STL files. All the following tests are intended to verify that both
 * procedures produce equivalent meshes, and particularly that changes related to issue #38 don't
 * break mesh loading.
 */
class CompareMeshVsPrimitive : public ::testing::Test
{
public:
  CompareMeshVsPrimitive()
  {
  }

  void SetUp() override
  {
    // BOX
    shapes::Box box(1.0, 1.0, 1.0);
    shape_meshes.push_back(shapes::createMeshFromShape(&box));
    loaded_meshes.push_back(shapes::createMeshFromResource(
        "file://" + (boost::filesystem::path(TEST_RESOURCES_DIR) / "/cube.stl").string()));

    shape_convex_meshes.push_back(new bodies::ConvexMesh(shape_meshes.back()));
    loaded_convex_meshes.push_back(new bodies::ConvexMesh(loaded_meshes.back()));
  }

  void TearDown() override
  {
    for (size_t i = 0; i < shape_meshes.size(); ++i)
    {
      delete shape_meshes[i];
      delete loaded_meshes[i];

      delete shape_convex_meshes[i];
      delete loaded_convex_meshes[i];
    }
  }

  ~CompareMeshVsPrimitive() override
  {
  }

protected:
  random_numbers::RandomNumberGenerator rng;

  std::vector<shapes::Mesh*> shape_meshes;
  std::vector<shapes::Mesh*> loaded_meshes;

  std::vector<bodies::Body*> shape_convex_meshes;
  std::vector<bodies::Body*> loaded_convex_meshes;
};

TEST_F(CompareMeshVsPrimitive, ContainsPoint)
{
  // Any point inside a mesh must be inside the other too
  for (size_t i = 0; i < shape_meshes.size(); ++i)
  {
    bodies::Body* shape_cms = shape_convex_meshes[i];
    bodies::Body* loaded_cms = loaded_convex_meshes[i];

    Eigen::Vector3d p;
    bool found = false;
    for (int i = 0; i < 100; ++i)
    {
      if ((shape_cms->samplePointInside(rng, 10000, p)) || (loaded_cms->samplePointInside(rng, 10000, p)))
      {
        found = true;
        EXPECT_EQ(shape_cms->containsPoint(p), loaded_cms->containsPoint(p));
      }
    }
    EXPECT_TRUE(found) << "No point inside the meshes was found (very unlikely)";
  }
}

TEST_F(CompareMeshVsPrimitive, IntersectsRay)
{
  // Random rays must intersect both meshes nearly at the same points
  for (size_t i = 0; i < shape_meshes.size(); ++i)
  {
    bodies::Body* shape_cms = shape_convex_meshes[i];
    bodies::Body* loaded_cms = loaded_convex_meshes[i];

    bool intersects = false;
    for (int i = 0; i < 100; ++i)
    {
      Eigen::Vector3d ray_o(rng.uniformReal(-1.0, +1.0), rng.uniformReal(-1.0, +1.0), rng.uniformReal(-1.0, +1.0));
      Eigen::Vector3d ray_d(rng.uniformReal(-1.0, +1.0), rng.uniformReal(-1.0, +1.0), rng.uniformReal(-1.0, +1.0));
      EigenSTL::vector_Vector3d vi1, vi2;
      shape_cms->intersectsRay(ray_o, ray_d, &vi1);
      loaded_cms->intersectsRay(ray_o, ray_d, &vi2);

      // DEBUG printing
      //    if (vi1.size() != vi2.size() && vi1.size() > 0 && vi2.size() > 0)
      //    {
      //        std::cout << vi1.size() << "   " << vi2.size() << "\n";
      //        std::cout << ray_o.x() << "  "<< ray_o.y() << "  "<< ray_o.z()
      //          << "\n" << ray_d.x() << "  "<< ray_d.y() << "  "<< ray_d.z() << "\n";
      //    }

      EXPECT_EQ(vi1.size(), vi2.size());
      if (!vi1.empty() && !vi2.empty())
      {
        EXPECT_NEAR(vi1[0].x(), vi2[0].x(), 0.01);
        EXPECT_NEAR(vi1[0].y(), vi2[0].y(), 0.01);
        EXPECT_NEAR(vi1[0].z(), vi2[0].z(), 0.01);

        intersects = true;
      }
    }

    EXPECT_TRUE(intersects) << "No ray intersects the meshes (very unlikely)";
  }
}

TEST_F(CompareMeshVsPrimitive, BoundingSphere)
{
  // Bounding spheres must be nearly identical
  for (size_t i = 0; i < shape_meshes.size(); ++i)
  {
    shapes::Mesh* shape_ms = shape_meshes[i];
    shapes::Mesh* loaded_ms = loaded_meshes[i];

    shapes::Sphere shape(1.0);
    Eigen::Vector3d center1, center2;
    double radius1, radius2;
    computeShapeBoundingSphere(shape_ms, center1, radius1);
    computeShapeBoundingSphere(loaded_ms, center2, radius2);

    EXPECT_NEAR(radius1, radius2, 0.001);
    EXPECT_NEAR(center1.x(), center2.x(), 0.001);
    EXPECT_NEAR(center1.y(), center2.y(), 0.001);
    EXPECT_NEAR(center1.z(), center2.z(), 0.001);
  }
}

TEST_F(CompareMeshVsPrimitive, BoxVertexCount)
{
  // For a simple shape as a cube, we expect that both meshes have the same number of vertex and triangles
  // But that was not the case before fixing issue #38!
  // These tests don't apply to curve shapes because the number of elements depends on how smooth they where
  // created. So ensure that "back()" gives a pointer to box meshes!
  EXPECT_EQ(shape_meshes.back()->vertex_count, loaded_meshes.back()->vertex_count);
}

TEST_F(CompareMeshVsPrimitive, BoxTriangleCount)
{
  EXPECT_EQ(shape_meshes.back()->triangle_count, loaded_meshes.back()->triangle_count);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
