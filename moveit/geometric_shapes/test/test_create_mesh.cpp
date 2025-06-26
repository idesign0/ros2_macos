// Copyright 2016 Delft Robotics
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
//    * Neither the name of the Delft Robotics nor the names of its
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

/** \Author Maarten de Vries <maarten@de-vri.es> */

#include "resources/config.h"
#include <geometric_shapes/mesh_operations.h>
#include <gtest/gtest.h>
#include <string>

namespace
{
void assertMesh(shapes::Mesh* mesh)
{
  ASSERT_TRUE(mesh != nullptr);
  ASSERT_EQ(3u, mesh->vertex_count);
  ASSERT_EQ(1u, mesh->triangle_count);

  ASSERT_EQ(0u, mesh->triangles[0]);
  ASSERT_EQ(1u, mesh->triangles[1]);
  ASSERT_EQ(2u, mesh->triangles[2]);

  ASSERT_FLOAT_EQ(0, mesh->vertices[0 + 0]);
  ASSERT_FLOAT_EQ(0, mesh->vertices[0 + 1]);
  ASSERT_FLOAT_EQ(1, mesh->vertices[0 + 2]);
  ASSERT_FLOAT_EQ(0, mesh->vertices[3 + 0]);
  ASSERT_FLOAT_EQ(1, mesh->vertices[3 + 1]);
  ASSERT_FLOAT_EQ(0, mesh->vertices[3 + 2]);
  ASSERT_FLOAT_EQ(1, mesh->vertices[6 + 0]);
  ASSERT_FLOAT_EQ(0, mesh->vertices[6 + 1]);
  ASSERT_FLOAT_EQ(0, mesh->vertices[6 + 2]);
}

shapes::Mesh* loadMesh(const std::string& mesh)
{
  std::string path = "file://" + std::string(TEST_RESOURCES_DIR) + "/" + mesh;
  return shapes::createMeshFromResource(path);
}
}  // namespace

TEST(CreateMesh, stl)
{
  assertMesh(loadMesh("triangle.stl"));
}

TEST(CreateMesh, daeNoUp)
{
  assertMesh(loadMesh("triangle_no_up.dae"));
}

TEST(CreateMesh, daeYUp)
{
  assertMesh(loadMesh("triangle_y_up.dae"));
}

TEST(CreateMesh, daeZUp)
{
  assertMesh(loadMesh("triangle_z_up.dae"));
}

TEST(CreateMesh, daeXUp)
{
  assertMesh(loadMesh("triangle_x_up.dae"));
}

TEST(CreateMesh, daeNoUnit)
{
  assertMesh(loadMesh("triangle_no_unit.dae"));
}

TEST(CreateMesh, dae1M)
{
  assertMesh(loadMesh("triangle_1m.dae"));
}

TEST(CreateMesh, dae10M)
{
  assertMesh(loadMesh("triangle_10m.dae"));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
