// Copyright 2011 Willow Garage, Inc.
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

#include "geometric_shapes/shapes.h"
#include <eigen_stl_containers/eigen_stl_containers.h>
#include <octomap/octomap.h>
#include <console_bridge/console.h>

namespace shapes
{
const std::string Sphere::STRING_NAME = "sphere";
const std::string Box::STRING_NAME = "box";
const std::string Cylinder::STRING_NAME = "cylinder";
const std::string Cone::STRING_NAME = "cone";
const std::string Mesh::STRING_NAME = "mesh";
const std::string Plane::STRING_NAME = "plane";
const std::string OcTree::STRING_NAME = "octree";

std::ostream& operator<<(std::ostream& ss, ShapeType type)
{
  switch (type)
  {
    case UNKNOWN_SHAPE:
      ss << "unknown";
      break;
    case SPHERE:
      ss << Sphere::STRING_NAME;
      break;
    case CYLINDER:
      ss << Cylinder::STRING_NAME;
      break;
    case CONE:
      ss << Cone::STRING_NAME;
      break;
    case BOX:
      ss << Box::STRING_NAME;
      break;
    case PLANE:
      ss << Plane::STRING_NAME;
      break;
    case MESH:
      ss << Mesh::STRING_NAME;
      break;
    case OCTREE:
      ss << OcTree::STRING_NAME;
      break;
    default:
      ss << "impossible";
      break;
  }
  return ss;
}

Shape::Shape()
{
  type = UNKNOWN_SHAPE;
}

Shape::~Shape()
{
}

Sphere::Sphere() : Shape()
{
  type = SPHERE;
  radius = 0.0;
}

Sphere::Sphere(double r) : Shape()
{
  if (r < 0)
    throw std::runtime_error("Sphere radius must be non-negative.");
  type = SPHERE;
  radius = r;
}

Cylinder::Cylinder() : Shape()
{
  type = CYLINDER;
  length = radius = 0.0;
}

Cylinder::Cylinder(double r, double l) : Shape()
{
  if (r < 0 || l < 0)
    throw std::runtime_error("Cylinder dimensions must be non-negative.");
  type = CYLINDER;
  length = l;
  radius = r;
}

Cone::Cone() : Shape()
{
  type = CONE;
  length = radius = 0.0;
}

Cone::Cone(double r, double l) : Shape()
{
  if (r < 0 || l < 0)
    throw std::runtime_error("Cone dimensions must be non-negative.");
  type = CONE;
  length = l;
  radius = r;
}

Box::Box() : Shape()
{
  type = BOX;
  size[0] = size[1] = size[2] = 0.0;
}

Box::Box(double x, double y, double z) : Shape()
{
  if (x < 0 || y < 0 || z < 0)
    throw std::runtime_error("Box dimensions must be non-negative.");
  type = BOX;
  size[0] = x;
  size[1] = y;
  size[2] = z;
}

Mesh::Mesh() : Shape()
{
  type = MESH;
  vertex_count = 0;
  vertices = nullptr;
  triangle_count = 0;
  triangles = nullptr;
  triangle_normals = nullptr;
  vertex_normals = nullptr;
}

Mesh::Mesh(unsigned int v_count, unsigned int t_count) : Shape()
{
  type = MESH;
  vertex_count = v_count;
  vertices = new double[v_count * 3];
  triangle_count = t_count;
  triangles = new unsigned int[t_count * 3];
  triangle_normals = new double[t_count * 3];
  vertex_normals = new double[v_count * 3];
}

Mesh::~Mesh()
{
  if (vertices)
    delete[] vertices;
  if (triangles)
    delete[] triangles;
  if (triangle_normals)
    delete[] triangle_normals;
  if (vertex_normals)
    delete[] vertex_normals;
}

Plane::Plane() : Shape()
{
  type = PLANE;
  a = b = c = d = 0.0;
}

Plane::Plane(double pa, double pb, double pc, double pd) : Shape()
{
  type = PLANE;
  a = pa;
  b = pb;
  c = pc;
  d = pd;
}

OcTree::OcTree() : Shape()
{
  type = OCTREE;
}

OcTree::OcTree(const std::shared_ptr<const octomap::OcTree>& t) : octree(t)
{
  type = OCTREE;
}

Sphere* Sphere::clone() const
{
  return new Sphere(radius);
}

Cylinder* Cylinder::clone() const
{
  return new Cylinder(radius, length);
}

Cone* Cone::clone() const
{
  return new Cone(radius, length);
}

Box* Box::clone() const
{
  return new Box(size[0], size[1], size[2]);
}

Mesh* Mesh::clone() const
{
  Mesh* dest = new Mesh(vertex_count, triangle_count);
  unsigned int n = 3 * vertex_count;
  for (unsigned int i = 0; i < n; ++i)
    dest->vertices[i] = vertices[i];
  if (vertex_normals)
    for (unsigned int i = 0; i < n; ++i)
      dest->vertex_normals[i] = vertex_normals[i];
  else
  {
    delete[] dest->vertex_normals;
    dest->vertex_normals = nullptr;
  }
  n = 3 * triangle_count;
  for (unsigned int i = 0; i < n; ++i)
    dest->triangles[i] = triangles[i];
  if (triangle_normals)
    for (unsigned int i = 0; i < n; ++i)
      dest->triangle_normals[i] = triangle_normals[i];
  else
  {
    delete[] dest->triangle_normals;
    dest->triangle_normals = nullptr;
  }
  return dest;
}

Plane* Plane::clone() const
{
  return new Plane(a, b, c, d);
}

OcTree* OcTree::clone() const
{
  return new OcTree(octree);
}

void OcTree::scaleAndPadd(double /* scale */, double /* padd */)
{
  CONSOLE_BRIDGE_logWarn("OcTrees cannot be scaled or padded");
}

void Plane::scaleAndPadd(double /* scale */, double /* padd */)
{
  CONSOLE_BRIDGE_logWarn("Planes cannot be scaled or padded");
}

void Shape::scale(double scale)
{
  scaleAndPadd(scale, 0.0);
}

void Shape::padd(double padding)
{
  scaleAndPadd(1.0, padding);
}

void Sphere::scaleAndPadd(double scale, double padding)
{
  const auto tmpRadius = radius * scale + padding;
  if (tmpRadius < 0)
    throw std::runtime_error("Sphere radius must be non-negative.");
  radius = tmpRadius;
}

void Cylinder::scaleAndPadd(double scaleRadius, double scaleLength, double paddRadius, double paddLength)
{
  const auto tmpRadius = radius * scaleRadius + paddRadius;
  const auto tmpLength = length * scaleLength + 2.0 * paddLength;
  if (tmpRadius < 0 || tmpLength < 0)
    throw std::runtime_error("Cylinder dimensions must be non-negative.");
  radius = tmpRadius;
  length = tmpLength;
}

void Cylinder::scale(double scaleRadius, double scaleLength)
{
  scaleAndPadd(scaleRadius, scaleLength, 0.0, 0.0);
}

void Cylinder::padd(double paddRadius, double paddLength)
{
  scaleAndPadd(1.0, 1.0, paddRadius, paddLength);
}

void Cylinder::scaleAndPadd(double scale, double padd)
{
  scaleAndPadd(scale, scale, padd, padd);
}

void Cone::scaleAndPadd(double scaleRadius, double scaleLength, double paddRadius, double paddLength)
{
  const auto tmpRadius = radius * scaleRadius + paddRadius;
  const auto tmpLength = length * scaleLength + 2.0 * paddLength;
  if (tmpRadius < 0 || tmpLength < 0)
    throw std::runtime_error("Cone dimensions must be non-negative.");
  radius = tmpRadius;
  length = tmpLength;
}

void Cone::scale(double scaleRadius, double scaleLength)
{
  scaleAndPadd(scaleRadius, scaleLength, 0.0, 0.0);
}

void Cone::padd(double paddRadius, double paddLength)
{
  scaleAndPadd(1.0, 1.0, paddRadius, paddLength);
}

void Cone::scaleAndPadd(double scale, double padd)
{
  scaleAndPadd(scale, scale, padd, padd);
}

void Box::scaleAndPadd(double scaleX, double scaleY, double scaleZ, double paddX, double paddY, double paddZ)
{
  const auto tmpSize0 = size[0] * scaleX + paddX * 2.0;
  const auto tmpSize1 = size[1] * scaleY + paddY * 2.0;
  const auto tmpSize2 = size[2] * scaleZ + paddZ * 2.0;
  if (tmpSize0 < 0 || tmpSize1 < 0 || tmpSize2 < 0)
    throw std::runtime_error("Box dimensions must be non-negative.");
  size[0] = tmpSize0;
  size[1] = tmpSize1;
  size[2] = tmpSize2;
}

void Box::scale(double scaleX, double scaleY, double scaleZ)
{
  scaleAndPadd(scaleX, scaleY, scaleZ, 0.0, 0.0, 0.0);
}

void Box::padd(double paddX, double paddY, double paddZ)
{
  scaleAndPadd(1.0, 1.0, 1.0, paddX, paddY, paddZ);
}

void Box::scaleAndPadd(double scale, double padd)
{
  scaleAndPadd(scale, scale, scale, padd, padd, padd);
}

void Mesh::scaleAndPadd(double scaleX, double scaleY, double scaleZ, double paddX, double paddY, double paddZ)
{
  // find the center of the mesh
  double sx = 0.0, sy = 0.0, sz = 0.0;
  for (unsigned int i = 0; i < vertex_count; ++i)
  {
    unsigned int i3 = i * 3;
    sx += vertices[i3];
    sy += vertices[i3 + 1];
    sz += vertices[i3 + 2];
  }
  sx /= (double)vertex_count;
  sy /= (double)vertex_count;
  sz /= (double)vertex_count;

  // scale the mesh
  for (unsigned int i = 0; i < vertex_count; ++i)
  {
    unsigned int i3 = i * 3;

    // vector from center to the vertex
    double dx = vertices[i3] - sx;
    double dy = vertices[i3 + 1] - sy;
    double dz = vertices[i3 + 2] - sz;

    // Scaled coordinate
    double scaledX = sx + dx * scaleX;
    double scaledY = sy + dy * scaleY;
    double scaledZ = sz + dz * scaleZ;
    // Padding in each direction
    vertices[i3] = scaledX + vertex_normals[i3] * paddX;
    vertices[i3 + 1] = scaledY + vertex_normals[i3 + 1] * paddY;
    vertices[i3 + 2] = scaledZ + vertex_normals[i3 + 2] * paddZ;
  }
}

void Mesh::scale(double scaleX, double scaleY, double scaleZ)
{
  scaleAndPadd(scaleX, scaleY, scaleZ, 0.0, 0.0, 0.0);
}

void Mesh::padd(double paddX, double paddY, double paddZ)
{
  scaleAndPadd(1.0, 1.0, 1.0, paddX, paddY, paddZ);
}

void Mesh::scaleAndPadd(double scale, double padd)
{
  scaleAndPadd(scale, scale, scale, padd, padd, padd);
}

void Shape::print(std::ostream& out) const
{
  out << this << std::endl;
}

void Sphere::print(std::ostream& out) const
{
  out << "Sphere[radius=" << radius << "]" << std::endl;
}

void Cylinder::print(std::ostream& out) const
{
  out << "Cylinder[radius=" << radius << ", length=" << length << "]" << std::endl;
}

void Cone::print(std::ostream& out) const
{
  out << "Cone[radius=" << radius << ", length=" << length << "]" << std::endl;
}

void Box::print(std::ostream& out) const
{
  out << "Box[x=length=" << size[0] << ", y=width=" << size[1] << ", z=height=" << size[2] << "]" << std::endl;
}

void Mesh::print(std::ostream& out) const
{
  out << "Mesh[vertices=" << vertex_count << ", triangles=" << triangle_count << "]" << std::endl;
}

void Plane::print(std::ostream& out) const
{
  out << "Plane[a=" << a << ", b=" << b << ", c=" << c << ", d=" << d << "]" << std::endl;
}

void OcTree::print(std::ostream& out) const
{
  if (octree)
  {
    double minx, miny, minz, maxx, maxy, maxz;
    octree->getMetricMin(minx, miny, minz);
    octree->getMetricMax(maxx, maxy, maxz);
    out << "OcTree[depth = " << octree->getTreeDepth() << ", resolution = " << octree->getResolution()
        << " inside box (minx=" << minx << ", miny=" << miny << ", minz=" << minz << ", maxx=" << maxx
        << ", maxy=" << maxy << ", maxz=" << maxz << ")]" << std::endl;
  }
  else
    out << "OcTree[NULL]" << std::endl;
}

bool Shape::isFixed() const
{
  return false;
}

bool OcTree::isFixed() const
{
  return true;
}

bool Plane::isFixed() const
{
  return true;
}

void Mesh::computeTriangleNormals()
{
  if (triangle_count && !triangle_normals)
    triangle_normals = new double[triangle_count * 3];

  // compute normals
  for (unsigned int i = 0; i < triangle_count; ++i)
  {
    unsigned int i3 = i * 3;
    Eigen::Vector3d s1(vertices[triangles[i3] * 3] - vertices[triangles[i3 + 1] * 3],
                       vertices[triangles[i3] * 3 + 1] - vertices[triangles[i3 + 1] * 3 + 1],
                       vertices[triangles[i3] * 3 + 2] - vertices[triangles[i3 + 1] * 3 + 2]);
    Eigen::Vector3d s2(vertices[triangles[i3 + 1] * 3] - vertices[triangles[i3 + 2] * 3],
                       vertices[triangles[i3 + 1] * 3 + 1] - vertices[triangles[i3 + 2] * 3 + 1],
                       vertices[triangles[i3 + 1] * 3 + 2] - vertices[triangles[i3 + 2] * 3 + 2]);
    Eigen::Vector3d normal = s1.cross(s2);
    normal.normalize();
    triangle_normals[i3] = normal.x();
    triangle_normals[i3 + 1] = normal.y();
    triangle_normals[i3 + 2] = normal.z();
  }
}

void Mesh::computeVertexNormals()
{
  if (!triangle_normals)
    computeTriangleNormals();
  if (vertex_count && !vertex_normals)
    vertex_normals = new double[vertex_count * 3];
  Eigen::Map<Eigen::Matrix<double, 3, Eigen::Dynamic>> mapped_normals(vertex_normals, 3, vertex_count);
  mapped_normals.setZero();

  for (unsigned int tIdx = 0; tIdx < triangle_count; ++tIdx)
  {
    unsigned int tIdx3 = 3 * tIdx;
    unsigned int tIdx3_1 = tIdx3 + 1;
    unsigned int tIdx3_2 = tIdx3 + 2;

    unsigned int v1 = triangles[tIdx3];
    unsigned int v2 = triangles[tIdx3_1];
    unsigned int v3 = triangles[tIdx3_2];

    // Get angles for each vertex at this triangle
    Eigen::Map<Eigen::Vector3d> p1{ vertices + 3 * v1, 3 };
    Eigen::Map<Eigen::Vector3d> p2{ vertices + 3 * v2, 3 };
    Eigen::Map<Eigen::Vector3d> p3{ vertices + 3 * v3, 3 };

    // Use eigen to calculate angle between the two vectors
    auto angleBetweenVectors = [](const Eigen::Vector3d& vec1, const Eigen::Vector3d& vec2) -> double {
      Eigen::AngleAxisd a(Eigen::Quaterniond::FromTwoVectors(vec1, vec2));
      return a.angle();
    };
    auto ang1 = angleBetweenVectors(p2 - p1, p3 - p1);
    auto ang2 = angleBetweenVectors(p1 - p2, p3 - p2);
    auto ang3 = angleBetweenVectors(p1 - p3, p2 - p3);

    // Weight normal with angle
    Eigen::Map<Eigen::Vector3d> triangle_normal{ triangle_normals + tIdx3, 3 };
    mapped_normals.col(v1) += triangle_normal * ang1;
    mapped_normals.col(v2) += triangle_normal * ang2;
    mapped_normals.col(v3) += triangle_normal * ang3;
  }

  // Normalize each column of the matrix
  for (int i = 0; i < mapped_normals.cols(); ++i)
  {
    auto mapped_normal = mapped_normals.col(i);
    if (mapped_normal.squaredNorm() != 0.0)
    {
      mapped_normal.normalize();
    }
  }
}

void Mesh::mergeVertices(double threshold)
{
  const double thresholdSQR = threshold * threshold;

  std::vector<unsigned int> vertex_map(vertex_count);
  EigenSTL::vector_Vector3d orig_vertices(vertex_count);
  EigenSTL::vector_Vector3d compressed_vertices;

  for (unsigned int vIdx = 0; vIdx < vertex_count; ++vIdx)
  {
    orig_vertices[vIdx][0] = vertices[3 * vIdx];
    orig_vertices[vIdx][1] = vertices[3 * vIdx + 1];
    orig_vertices[vIdx][2] = vertices[3 * vIdx + 2];
    vertex_map[vIdx] = vIdx;
  }

  for (unsigned int vIdx1 = 0; vIdx1 < vertex_count; ++vIdx1)
  {
    if (vertex_map[vIdx1] != vIdx1)
      continue;

    vertex_map[vIdx1] = compressed_vertices.size();
    compressed_vertices.push_back(orig_vertices[vIdx1]);

    for (unsigned int vIdx2 = vIdx1 + 1; vIdx2 < vertex_count; ++vIdx2)
    {
      double distanceSQR = (orig_vertices[vIdx1] - orig_vertices[vIdx2]).squaredNorm();
      if (distanceSQR <= thresholdSQR)
        vertex_map[vIdx2] = vertex_map[vIdx1];
    }
  }

  if (compressed_vertices.size() == orig_vertices.size())
    return;

  // redirect triangles to new vertices!
  for (unsigned int tIdx = 0; tIdx < triangle_count; ++tIdx)
  {
    unsigned int i3 = 3 * tIdx;
    triangles[i3] = vertex_map[triangles[i3]];
    triangles[i3 + 1] = vertex_map[triangles[i3 + 1]];
    triangles[i3 + 2] = vertex_map[triangles[i3 + 2]];
  }

  vertex_count = compressed_vertices.size();
  delete[] vertices;
  vertices = new double[vertex_count * 3];

  for (unsigned int vIdx = 0; vIdx < vertex_count; ++vIdx)
  {
    unsigned int i3 = 3 * vIdx;
    vertices[i3] = compressed_vertices[vIdx][0];
    vertices[i3 + 1] = compressed_vertices[vIdx][1];
    vertices[i3 + 2] = compressed_vertices[vIdx][2];
  }

  if (triangle_normals)
    computeTriangleNormals();
  if (vertex_normals)
    computeVertexNormals();
}

} /* namespace shapes */
