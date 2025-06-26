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

#include "geometric_shapes/mesh_operations.h"
#include "geometric_shapes/shape_operations.h"

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <set>
#include <float.h>

#include <console_bridge/console.h>

// Shim so this works on foxy/galactic/rolling
#if __has_include(<resource_retriever/retriever.hpp>)
#include <resource_retriever/retriever.hpp>
#else
#include <resource_retriever/retriever.h>
#endif

#if __has_include(<resource_retriever/resource.hpp>)
#define NEW_RESOURCE_RETRIEVER
#endif

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <Eigen/Geometry>

#include <boost/math/constants/constants.hpp>

namespace shapes
{
namespace detail
{
namespace
{
/// Local representation of a vertex that knows its position in an array (used for sorting)
struct LocalVertexType
{
  LocalVertexType() : x(0.0), y(0.0), z(0.0)
  {
  }

  LocalVertexType(const Eigen::Vector3d& v) : x(v.x()), y(v.y()), z(v.z())
  {
  }

  double x, y, z;
  unsigned int index;
};

/// Sorting operator by point value
struct ltLocalVertexValue
{
  bool operator()(const LocalVertexType& p1, const LocalVertexType& p2) const
  {
    if (p1.x < p2.x)
      return true;
    if (p1.x > p2.x)
      return false;
    if (p1.y < p2.y)
      return true;
    if (p1.y > p2.y)
      return false;
    if (p1.z < p2.z)
      return true;
    return false;
  }
};

/// Sorting operator by point index
struct ltLocalVertexIndex
{
  bool operator()(const LocalVertexType& p1, const LocalVertexType& p2) const
  {
    return p1.index < p2.index;
  }
};
}  // namespace
}  // namespace detail

Mesh* createMeshFromVertices(const EigenSTL::vector_Vector3d& vertices, const std::vector<unsigned int>& triangles)
{
  unsigned int nt = triangles.size() / 3;
  Mesh* mesh = new Mesh(vertices.size(), nt);
  for (unsigned int i = 0; i < vertices.size(); ++i)
  {
    mesh->vertices[3 * i] = vertices[i].x();
    mesh->vertices[3 * i + 1] = vertices[i].y();
    mesh->vertices[3 * i + 2] = vertices[i].z();
  }

  std::copy(triangles.begin(), triangles.end(), mesh->triangles);
  mesh->computeTriangleNormals();
  mesh->computeVertexNormals();

  return mesh;
}

Mesh* createMeshFromVertices(const EigenSTL::vector_Vector3d& source)
{
  if (source.size() < 3)
    return nullptr;

  if (source.size() % 3 != 0)
    CONSOLE_BRIDGE_logError("The number of vertices to construct a mesh from is not divisible by 3. Probably "
                            "constructed triangles will not make sense.");

  std::set<detail::LocalVertexType, detail::ltLocalVertexValue> vertices;
  std::vector<unsigned int> triangles;

  unsigned int n = source.size() / 3;
  for (unsigned int i = 0; i < n; ++i)
  {
    // check if we have new vertices
    unsigned int i3 = i * 3;
    detail::LocalVertexType vt1(source[i3]);
    std::set<detail::LocalVertexType, detail::ltLocalVertexValue>::iterator p1 = vertices.find(vt1);
    if (p1 == vertices.end())
    {
      vt1.index = vertices.size();
      vertices.insert(vt1);
    }
    else
      vt1.index = p1->index;
    triangles.push_back(vt1.index);

    detail::LocalVertexType vt2(source[++i3]);
    std::set<detail::LocalVertexType, detail::ltLocalVertexValue>::iterator p2 = vertices.find(vt2);
    if (p2 == vertices.end())
    {
      vt2.index = vertices.size();
      vertices.insert(vt2);
    }
    else
      vt2.index = p2->index;
    triangles.push_back(vt2.index);

    detail::LocalVertexType vt3(source[++i3]);
    std::set<detail::LocalVertexType, detail::ltLocalVertexValue>::iterator p3 = vertices.find(vt3);
    if (p3 == vertices.end())
    {
      vt3.index = vertices.size();
      vertices.insert(vt3);
    }
    else
      vt3.index = p3->index;

    triangles.push_back(vt3.index);
  }

  // sort our vertices
  std::vector<detail::LocalVertexType> vt;
  vt.insert(vt.end(), vertices.begin(), vertices.end());
  std::sort(vt.begin(), vt.end(), detail::ltLocalVertexIndex());

  // copy the data to a mesh structure
  unsigned int nt = triangles.size() / 3;

  Mesh* mesh = new Mesh(vt.size(), nt);
  for (unsigned int i = 0; i < vt.size(); ++i)
  {
    unsigned int i3 = i * 3;
    mesh->vertices[i3] = vt[i].x;
    mesh->vertices[i3 + 1] = vt[i].y;
    mesh->vertices[i3 + 2] = vt[i].z;
  }

  std::copy(triangles.begin(), triangles.end(), mesh->triangles);
  mesh->computeTriangleNormals();
  mesh->computeVertexNormals();

  return mesh;
}

Mesh* createMeshFromResource(const std::string& resource)
{
  static const Eigen::Vector3d one(1.0, 1.0, 1.0);
  return createMeshFromResource(resource, one);
}

Mesh* createMeshFromBinary(const char* buffer, std::size_t size, const std::string& assimp_hint)
{
  static const Eigen::Vector3d one(1.0, 1.0, 1.0);
  return createMeshFromBinary(buffer, size, one, assimp_hint);
}

Mesh* createMeshFromBinary(const char* buffer, std::size_t size, const Eigen::Vector3d& scale,
                           const std::string& assimp_hint)
{
  if (!buffer || size < 1)
  {
    CONSOLE_BRIDGE_logWarn("Cannot construct mesh from empty binary buffer");
    return nullptr;
  }

  // try to get a file extension
  std::string hint;
  std::size_t pos = assimp_hint.find_last_of('.');
  if (pos != std::string::npos)
  {
    hint = assimp_hint.substr(pos + 1);
    std::transform(hint.begin(), hint.end(), hint.begin(), ::tolower);
  }
  if (hint.empty())
    hint = assimp_hint;  // send entire file name as hint if no extension was found

  // Create an instance of the Importer class
  Assimp::Importer importer;

  // Issue #38 fix: as part of the post-processing, we remove all other components in file but
  // the meshes, as anyway the resulting shapes:Mesh object just receives vertices and triangles.
  importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS | aiComponent_TANGENTS_AND_BITANGENTS |
                                                          aiComponent_COLORS | aiComponent_TEXCOORDS |
                                                          aiComponent_BONEWEIGHTS | aiComponent_ANIMATIONS |
                                                          aiComponent_TEXTURES | aiComponent_LIGHTS |
                                                          aiComponent_CAMERAS | aiComponent_MATERIALS);

  // And have it read the given file with some post-processing
  const aiScene* scene = importer.ReadFileFromMemory(reinterpret_cast<const void*>(buffer), size,
                                                     aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                                                         aiProcess_SortByPType | aiProcess_RemoveComponent,
                                                     hint.c_str());
  if (!scene)
    return nullptr;

  // Assimp enforces Y_UP convention by rotating models with different conventions.
  // However, that behaviour is confusing and doesn't match the ROS convention
  // where the Z axis is pointing up.
  // Hopefully this doesn't undo legit use of the root node transformation...
  // Note that this is also what RViz does internally.
  scene->mRootNode->mTransformation = aiMatrix4x4();

  // These post processing steps flatten the root node transformation into child nodes,
  // so they must be delayed until after clearing the root node transform above.
  importer.ApplyPostProcessing(aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

  return createMeshFromAsset(scene, scale, hint);
}

Mesh* createMeshFromResource(const std::string& resource, const Eigen::Vector3d& scale)
{
  resource_retriever::Retriever retriever;
#ifdef NEW_RESOURCE_RETRIEVER
  resource_retriever::ResourceSharedPtr res;
#else
  resource_retriever::MemoryResource res;
#endif
  try
  {
#ifdef NEW_RESOURCE_RETRIEVER
    res = retriever.get_shared(resource);
#else
    res = retriever.get(resource);
#endif
  }
  catch (resource_retriever::Exception& e)
  {
    CONSOLE_BRIDGE_logError("%s", e.what());
    return nullptr;
  }

#ifdef NEW_RESOURCE_RETRIEVER
  if (res->data.empty())
#else
  if (res.size == 0)
#endif
  {
    CONSOLE_BRIDGE_logWarn("Retrieved empty mesh for resource '%s'", resource.c_str());
    return nullptr;
  }

#ifdef NEW_RESOURCE_RETRIEVER
  Mesh* m = createMeshFromBinary(reinterpret_cast<const char*>(res->data.data()), res->data.size(), scale, resource);
#else
  Mesh* m = createMeshFromBinary(reinterpret_cast<const char*>(res.data.get()), res.size, scale, resource);
#endif
  if (!m)
  {
    CONSOLE_BRIDGE_logWarn("Assimp reports no scene in %s.", resource.c_str());
    CONSOLE_BRIDGE_logWarn("This could be because of a resource filename that is too long for the Assimp library, a "
                           "known bug. As a workaround shorten the filename/path.");
  }
  return m;
}

namespace
{
void extractMeshData(const aiScene* scene, const aiNode* node, const aiMatrix4x4& parent_transform,
                     const Eigen::Vector3d& scale, EigenSTL::vector_Vector3d& vertices,
                     std::vector<unsigned int>& triangles)
{
  aiMatrix4x4 transform = parent_transform;
  transform *= node->mTransformation;
  for (unsigned int j = 0; j < node->mNumMeshes; ++j)
  {
    const aiMesh* a = scene->mMeshes[node->mMeshes[j]];
    unsigned int offset = vertices.size();
    for (unsigned int i = 0; i < a->mNumVertices; ++i)
    {
      aiVector3D v = transform * a->mVertices[i];
      vertices.push_back(Eigen::Vector3d(v.x * scale.x(), v.y * scale.y(), v.z * scale.z()));
    }
    for (unsigned int i = 0; i < a->mNumFaces; ++i)
      if (a->mFaces[i].mNumIndices == 3)
      {
        triangles.push_back(offset + a->mFaces[i].mIndices[0]);
        triangles.push_back(offset + a->mFaces[i].mIndices[1]);
        triangles.push_back(offset + a->mFaces[i].mIndices[2]);
      }
  }

  for (unsigned int n = 0; n < node->mNumChildren; ++n)
    extractMeshData(scene, node->mChildren[n], transform, scale, vertices, triangles);
}
}  // namespace

Mesh* createMeshFromAsset(const aiScene* scene, const std::string& resource_name)
{
  static const Eigen::Vector3d one(1.0, 1.0, 1.0);
  return createMeshFromAsset(scene, one, resource_name);
}

Mesh* createMeshFromAsset(const aiScene* scene, const Eigen::Vector3d& scale, const std::string& resource_name)
{
  if (!scene->HasMeshes())
  {
    CONSOLE_BRIDGE_logWarn("Assimp reports scene in %s has no meshes", resource_name.c_str());
    return nullptr;
  }
  EigenSTL::vector_Vector3d vertices;
  std::vector<unsigned int> triangles;
  extractMeshData(scene, scene->mRootNode, aiMatrix4x4(), scale, vertices, triangles);
  if (vertices.empty())
  {
    CONSOLE_BRIDGE_logWarn("There are no vertices in the scene %s", resource_name.c_str());
    return nullptr;
  }
  if (triangles.empty())
  {
    CONSOLE_BRIDGE_logWarn("There are no triangles in the scene %s", resource_name.c_str());
    return nullptr;
  }

  return createMeshFromVertices(vertices, triangles);
}

Mesh* createMeshFromShape(const Shape* shape)
{
  if (shape->type == shapes::SPHERE)
    return shapes::createMeshFromShape(static_cast<const shapes::Sphere&>(*shape));
  else if (shape->type == shapes::BOX)
    return shapes::createMeshFromShape(static_cast<const shapes::Box&>(*shape));
  else if (shape->type == shapes::CYLINDER)
    return shapes::createMeshFromShape(static_cast<const shapes::Cylinder&>(*shape));
  else if (shape->type == shapes::CONE)
    return shapes::createMeshFromShape(static_cast<const shapes::Cone&>(*shape));
  else
    CONSOLE_BRIDGE_logError("Conversion of shape of type '%s' to a mesh is not known", shapeStringName(shape).c_str());
  return nullptr;
}

Mesh* createMeshFromShape(const Box& box)
{
  double x = box.size[0] / 2.0;
  double y = box.size[1] / 2.0;
  double z = box.size[2] / 2.0;

  // define vertices of box mesh
  Mesh* result = new Mesh(8, 12);
  result->vertices[0] = -x;
  result->vertices[1] = -y;
  result->vertices[2] = -z;

  result->vertices[3] = x;
  result->vertices[4] = -y;
  result->vertices[5] = -z;

  result->vertices[6] = x;
  result->vertices[7] = -y;
  result->vertices[8] = z;

  result->vertices[9] = -x;
  result->vertices[10] = -y;
  result->vertices[11] = z;

  result->vertices[12] = -x;
  result->vertices[13] = y;
  result->vertices[14] = z;

  result->vertices[15] = -x;
  result->vertices[16] = y;
  result->vertices[17] = -z;

  result->vertices[18] = x;
  result->vertices[19] = y;
  result->vertices[20] = z;

  result->vertices[21] = x;
  result->vertices[22] = y;
  result->vertices[23] = -z;

  static const unsigned int tri[] = { 0, 1, 2, 2, 3, 0, 4, 3, 2, 2, 6, 4, 7, 6, 2, 2, 1, 7,
                                      3, 4, 5, 5, 0, 3, 0, 5, 7, 7, 1, 0, 7, 5, 4, 4, 6, 7 };
  memcpy(result->triangles, tri, sizeof(unsigned int) * 36);
  result->computeTriangleNormals();
  result->computeVertexNormals();
  return result;
}

Mesh* createMeshFromShape(const Sphere& sphere)
{
  // this code is adapted from FCL
  EigenSTL::vector_Vector3d vertices;
  std::vector<unsigned int> triangles;

  const double r = sphere.radius;
  const double pi = boost::math::constants::pi<double>();
  const unsigned int seg = std::max<unsigned int>(6, 0.5 + 2.0 * pi * r / 0.01);  // split the sphere longitudinally up
                                                                                  // to a resolution of 1 cm at the
                                                                                  // ecuator, or a minimum of 6 segments
  const unsigned int ring = std::max<unsigned int>(6, 2.0 * r / 0.01);  // split the sphere into rings along latitude,
                                                                        // up to a height of at most 1 cm, or a minimum
                                                                        // of 6 rings

  double phi, phid;
  phid = pi * 2.0 / seg;
  phi = 0.0;

  double theta, thetad;
  thetad = pi / (ring + 1);
  theta = 0;

  for (unsigned int i = 0; i < ring; ++i)
  {
    double theta_ = theta + thetad * (i + 1);
    for (unsigned int j = 0; j < seg; ++j)
      vertices.push_back(Eigen::Vector3d(r * sin(theta_) * cos(phi + j * phid), r * sin(theta_) * sin(phi + j * phid),
                                         r * cos(theta_)));
  }
  vertices.push_back(Eigen::Vector3d(0.0, 0.0, r));
  vertices.push_back(Eigen::Vector3d(0.0, 0.0, -r));

  for (unsigned int i = 0; i < ring - 1; ++i)
  {
    for (unsigned int j = 0; j < seg; ++j)
    {
      unsigned int a, b, c, d;
      a = i * seg + j;
      b = (j == seg - 1) ? (i * seg) : (i * seg + j + 1);
      c = (i + 1) * seg + j;
      d = (j == seg - 1) ? ((i + 1) * seg) : ((i + 1) * seg + j + 1);
      triangles.push_back(a);
      triangles.push_back(c);
      triangles.push_back(b);
      triangles.push_back(b);
      triangles.push_back(c);
      triangles.push_back(d);
    }
  }

  for (unsigned int j = 0; j < seg; ++j)
  {
    unsigned int a, b;
    a = j;
    b = (j == seg - 1) ? 0 : (j + 1);
    triangles.push_back(ring * seg);
    triangles.push_back(a);
    triangles.push_back(b);

    a = (ring - 1) * seg + j;
    b = (j == seg - 1) ? (ring - 1) * seg : ((ring - 1) * seg + j + 1);
    triangles.push_back(a);
    triangles.push_back(ring * seg + 1);
    triangles.push_back(b);
  }
  return createMeshFromVertices(vertices, triangles);
}

Mesh* createMeshFromShape(const Cylinder& cylinder)
{
  // this code is adapted from FCL
  EigenSTL::vector_Vector3d vertices;
  std::vector<unsigned int> triangles;

  // magic number defining how many triangles to construct for the unit cylinder; perhaps this should be a param
  static unsigned int tot_for_unit_cylinder = 100;

  double r = cylinder.radius;
  double h = cylinder.length;

  const double pi = boost::math::constants::pi<double>();
  unsigned int tot = std::max<unsigned int>(6, ceil(tot_for_unit_cylinder * r));
  double phid = pi * 2 / tot;

  double circle_edge = phid * r;
  unsigned int h_num = ceil(std::abs(h) / circle_edge);

  double phi = 0;
  double hd = h / h_num;

  for (unsigned int i = 0; i < tot; ++i)
    vertices.push_back(Eigen::Vector3d(r * cos(phi + phid * i), r * sin(phi + phid * i), h / 2));

  for (unsigned int i = 0; i < h_num - 1; ++i)
    for (unsigned int j = 0; j < tot; ++j)
      vertices.push_back(Eigen::Vector3d(r * cos(phi + phid * j), r * sin(phi + phid * j), h / 2 - (i + 1) * hd));

  for (unsigned int i = 0; i < tot; ++i)
    vertices.push_back(Eigen::Vector3d(r * cos(phi + phid * i), r * sin(phi + phid * i), -h / 2));

  vertices.push_back(Eigen::Vector3d(0, 0, h / 2));
  vertices.push_back(Eigen::Vector3d(0, 0, -h / 2));

  for (unsigned int i = 0; i < tot; ++i)
  {
    triangles.push_back((h_num + 1) * tot);
    triangles.push_back(i);
    triangles.push_back((i == tot - 1) ? 0 : (i + 1));
  }

  for (unsigned int i = 0; i < tot; ++i)
  {
    triangles.push_back((h_num + 1) * tot + 1);
    triangles.push_back(h_num * tot + ((i == tot - 1) ? 0 : (i + 1)));
    triangles.push_back(h_num * tot + i);
  }

  for (unsigned int i = 0; i < h_num; ++i)
  {
    for (unsigned int j = 0; j < tot; ++j)
    {
      int a, b, c, d;
      a = j;
      b = (j == tot - 1) ? 0 : (j + 1);
      c = j + tot;
      d = (j == tot - 1) ? tot : (j + 1 + tot);

      int start = i * tot;
      triangles.push_back(start + b);
      triangles.push_back(start + a);
      triangles.push_back(start + c);
      triangles.push_back(start + b);
      triangles.push_back(start + c);
      triangles.push_back(start + d);
    }
  }
  return createMeshFromVertices(vertices, triangles);
}

Mesh* createMeshFromShape(const Cone& cone)
{
  // this code is adapted from FCL
  EigenSTL::vector_Vector3d vertices;
  std::vector<unsigned int> triangles;

  // magic number defining how many triangles to construct for the unit cylinder; perhaps this should be a param
  static unsigned int tot_for_unit_cone = 100;

  double r = cone.radius;
  double h = cone.length;

  const double pi = boost::math::constants::pi<double>();
  unsigned int tot = tot_for_unit_cone * r;
  double phid = pi * 2 / tot;

  double circle_edge = phid * r;
  unsigned int h_num = ceil(h / circle_edge);

  double phi = 0;
  double hd = h / h_num;

  for (unsigned int i = 0; i < h_num - 1; ++i)
  {
    double h_i = h / 2 - (i + 1) * hd;
    double rh = r * (0.5 - h_i / h);
    for (unsigned int j = 0; j < tot; ++j)
      vertices.push_back(Eigen::Vector3d(rh * cos(phi + phid * j), rh * sin(phi + phid * j), h_i));
  }

  for (unsigned int i = 0; i < tot; ++i)
    vertices.push_back(Eigen::Vector3d(r * cos(phi + phid * i), r * sin(phi + phid * i), -h / 2));

  vertices.push_back(Eigen::Vector3d(0, 0, h / 2));
  vertices.push_back(Eigen::Vector3d(0, 0, -h / 2));

  for (unsigned int i = 0; i < tot; ++i)
  {
    triangles.push_back(h_num * tot);
    triangles.push_back(i);
    triangles.push_back((i == tot - 1) ? 0 : (i + 1));
  }

  for (unsigned int i = 0; i < tot; ++i)
  {
    triangles.push_back(h_num * tot + 1);
    triangles.push_back((h_num - 1) * tot + ((i == tot - 1) ? 0 : (i + 1)));
    triangles.push_back((h_num - 1) * tot + i);
  }

  for (unsigned int i = 0; i < h_num - 1; ++i)
    for (unsigned int j = 0; j < tot; ++j)
    {
      int a, b, c, d;
      a = j;
      b = (j == tot - 1) ? 0 : (j + 1);
      c = j + tot;
      d = (j == tot - 1) ? tot : (j + 1 + tot);

      int start = i * tot;
      triangles.push_back(start + b);
      triangles.push_back(start + a);
      triangles.push_back(start + c);
      triangles.push_back(start + b);
      triangles.push_back(start + c);
      triangles.push_back(start + d);
    }
  return createMeshFromVertices(vertices, triangles);
}

namespace
{
inline void writeFloatToSTL(char*& ptr, const float data)
{
  memcpy(ptr, &data, sizeof(float));
  ptr += sizeof(float);
}
}  // namespace

void writeSTLBinary(const Mesh* mesh, std::vector<char>& buffer)
{
  buffer.resize(84 + mesh->triangle_count * 50);
  memset(&buffer[0], 0, 80);
  char* ptr = &buffer[80];
  uint32_t nt = mesh->triangle_count;
  memcpy(ptr, &nt, sizeof(uint32_t));
  ptr += sizeof(uint32_t);
  for (unsigned int i = 0; i < mesh->triangle_count; ++i)
  {
    unsigned int i3 = i * 3;

    if (mesh->triangle_normals)
    {
      writeFloatToSTL(ptr, mesh->triangle_normals[i3]);
      writeFloatToSTL(ptr, mesh->triangle_normals[i3 + 1]);
      writeFloatToSTL(ptr, mesh->triangle_normals[i3 + 2]);
    }
    else
    {
      memset(ptr, 0, sizeof(float) * 3);
      ptr += sizeof(float) * 3;
    }

    unsigned int index = mesh->triangles[i3] * 3;
    writeFloatToSTL(ptr, mesh->vertices[index]);
    writeFloatToSTL(ptr, mesh->vertices[index + 1]);
    writeFloatToSTL(ptr, mesh->vertices[index + 2]);
    index = mesh->triangles[i3 + 1] * 3;
    writeFloatToSTL(ptr, mesh->vertices[index]);
    writeFloatToSTL(ptr, mesh->vertices[index + 1]);
    writeFloatToSTL(ptr, mesh->vertices[index + 2]);
    index = mesh->triangles[i3 + 2] * 3;
    writeFloatToSTL(ptr, mesh->vertices[index]);
    writeFloatToSTL(ptr, mesh->vertices[index + 1]);
    writeFloatToSTL(ptr, mesh->vertices[index + 2]);
    memset(ptr, 0, 2);
    ptr += 2;
  }
}
}  // namespace shapes
