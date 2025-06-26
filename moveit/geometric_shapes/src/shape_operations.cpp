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

#include "geometric_shapes/shape_operations.h"

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <set>
#include <float.h>

#include <console_bridge/console.h>

#include <Eigen/Geometry>

#include <geometric_shapes/shape_to_marker.h>
#include <geometric_shapes/shape_extents.h>
#include <geometric_shapes/solid_primitive_dims.h>

namespace shapes
{
Shape* constructShapeFromMsg(const shape_msgs::msg::Plane& shape_msg)
{
  return new Plane(shape_msg.coef[0], shape_msg.coef[1], shape_msg.coef[2], shape_msg.coef[3]);
}

Shape* constructShapeFromMsg(const shape_msgs::msg::Mesh& shape_msg)
{
  if (shape_msg.triangles.empty() || shape_msg.vertices.empty())
  {
    CONSOLE_BRIDGE_logWarn("Mesh definition is empty");
    return nullptr;
  }
  else
  {
    EigenSTL::vector_Vector3d vertices(shape_msg.vertices.size());
    std::vector<unsigned int> triangles(shape_msg.triangles.size() * 3);
    for (unsigned int i = 0; i < shape_msg.vertices.size(); ++i)
      vertices[i] = Eigen::Vector3d(shape_msg.vertices[i].x, shape_msg.vertices[i].y, shape_msg.vertices[i].z);
    for (unsigned int i = 0; i < shape_msg.triangles.size(); ++i)
    {
      unsigned int i3 = i * 3;
      triangles[i3++] = shape_msg.triangles[i].vertex_indices[0];
      triangles[i3++] = shape_msg.triangles[i].vertex_indices[1];
      triangles[i3] = shape_msg.triangles[i].vertex_indices[2];
    }
    return createMeshFromVertices(vertices, triangles);
  }
}

Shape* constructShapeFromMsg(const shape_msgs::msg::SolidPrimitive& shape_msg)
{
  Shape* shape = nullptr;
  if (shape_msg.type == shape_msgs::msg::SolidPrimitive::SPHERE)
  {
    if (shape_msg.dimensions.size() >=
        geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::SPHERE>())
      shape = new Sphere(shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::SPHERE_RADIUS]);
  }
  else if (shape_msg.type == shape_msgs::msg::SolidPrimitive::BOX)
  {
    if (shape_msg.dimensions.size() >= geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::BOX>())
      shape = new Box(shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_X],
                      shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Y],
                      shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Z]);
  }
  else if (shape_msg.type == shape_msgs::msg::SolidPrimitive::CYLINDER)
  {
    if (shape_msg.dimensions.size() >=
        geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CYLINDER>())
      shape = new Cylinder(shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_RADIUS],
                           shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_HEIGHT]);
  }
  else if (shape_msg.type == shape_msgs::msg::SolidPrimitive::CONE)
  {
    if (shape_msg.dimensions.size() >=
        geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CONE>())
      shape = new Cone(shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CONE_RADIUS],
                       shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CONE_HEIGHT]);
  }
  if (shape == nullptr)
    CONSOLE_BRIDGE_logError("Unable to construct shape corresponding to shape_msg of type %d", (int)shape_msg.type);

  return shape;
}

namespace
{
class ShapeVisitorAlloc : public boost::static_visitor<Shape*>
{
public:
  Shape* operator()(const shape_msgs::msg::Plane& shape_msg) const
  {
    return constructShapeFromMsg(shape_msg);
  }

  Shape* operator()(const shape_msgs::msg::Mesh& shape_msg) const
  {
    return constructShapeFromMsg(shape_msg);
  }

  Shape* operator()(const shape_msgs::msg::SolidPrimitive& shape_msg) const
  {
    return constructShapeFromMsg(shape_msg);
  }
};
}  // namespace

Shape* constructShapeFromMsg(const ShapeMsg& shape_msg)
{
  return boost::apply_visitor(ShapeVisitorAlloc(), shape_msg);
}

namespace
{
class ShapeVisitorMarker : public boost::static_visitor<void>
{
public:
  ShapeVisitorMarker(visualization_msgs::msg::Marker* marker, bool use_mesh_triangle_list)
    : boost::static_visitor<void>(), use_mesh_triangle_list_(use_mesh_triangle_list), marker_(marker)
  {
  }

  void operator()(const shape_msgs::msg::Plane& /* shape_msg */) const
  {
    throw std::runtime_error("No visual markers can be constructed for planes");
  }

  void operator()(const shape_msgs::msg::Mesh& shape_msg) const
  {
    geometric_shapes::constructMarkerFromShape(shape_msg, *marker_, use_mesh_triangle_list_);
  }

  void operator()(const shape_msgs::msg::SolidPrimitive& shape_msg) const
  {
    geometric_shapes::constructMarkerFromShape(shape_msg, *marker_);
  }

private:
  bool use_mesh_triangle_list_;
  visualization_msgs::msg::Marker* marker_;
};
}  // namespace

bool constructMarkerFromShape(const Shape* shape, visualization_msgs::msg::Marker& marker, bool use_mesh_triangle_list)
{
  ShapeMsg shape_msg;
  if (constructMsgFromShape(shape, shape_msg))
  {
    bool ok = false;
    try
    {
      boost::apply_visitor(ShapeVisitorMarker(&marker, use_mesh_triangle_list), shape_msg);
      ok = true;
    }
    catch (std::runtime_error& ex)
    {
      CONSOLE_BRIDGE_logError("%s", ex.what());
    }
    if (ok)
      return true;
  }
  return false;
}

namespace
{
class ShapeVisitorComputeExtents : public boost::static_visitor<Eigen::Vector3d>
{
public:
  Eigen::Vector3d operator()(const shape_msgs::msg::Plane& /* shape_msg */) const
  {
    Eigen::Vector3d e(0.0, 0.0, 0.0);
    return e;
  }

  Eigen::Vector3d operator()(const shape_msgs::msg::Mesh& shape_msg) const
  {
    double x_extent, y_extent, z_extent;
    geometric_shapes::getShapeExtents(shape_msg, x_extent, y_extent, z_extent);
    Eigen::Vector3d e(x_extent, y_extent, z_extent);
    return e;
  }

  Eigen::Vector3d operator()(const shape_msgs::msg::SolidPrimitive& shape_msg) const
  {
    double x_extent, y_extent, z_extent;
    geometric_shapes::getShapeExtents(shape_msg, x_extent, y_extent, z_extent);
    Eigen::Vector3d e(x_extent, y_extent, z_extent);
    return e;
  }
};
}  // namespace

Eigen::Vector3d computeShapeExtents(const ShapeMsg& shape_msg)
{
  return boost::apply_visitor(ShapeVisitorComputeExtents(), shape_msg);
}

Eigen::Vector3d computeShapeExtents(const Shape* shape)
{
  if (shape->type == SPHERE)
  {
    double d = static_cast<const Sphere*>(shape)->radius * 2.0;
    return Eigen::Vector3d(d, d, d);
  }
  else if (shape->type == BOX)
  {
    const double* sz = static_cast<const Box*>(shape)->size;
    return Eigen::Vector3d(sz[0], sz[1], sz[2]);
  }
  else if (shape->type == CYLINDER)
  {
    double d = static_cast<const Cylinder*>(shape)->radius * 2.0;
    return Eigen::Vector3d(d, d, static_cast<const Cylinder*>(shape)->length);
  }
  else if (shape->type == CONE)
  {
    double d = static_cast<const Cone*>(shape)->radius * 2.0;
    return Eigen::Vector3d(d, d, static_cast<const Cone*>(shape)->length);
  }
  else if (shape->type == MESH)
  {
    const Mesh* mesh = static_cast<const Mesh*>(shape);
    if (mesh->vertex_count > 1)
    {
      std::vector<double> vmin(3, std::numeric_limits<double>::max());
      std::vector<double> vmax(3, -std::numeric_limits<double>::max());
      for (unsigned int i = 0; i < mesh->vertex_count; ++i)
      {
        unsigned int i3 = i * 3;
        for (unsigned int k = 0; k < 3; ++k)
        {
          unsigned int i3k = i3 + k;
          if (mesh->vertices[i3k] > vmax[k])
            vmax[k] = mesh->vertices[i3k];
          if (mesh->vertices[i3k] < vmin[k])
            vmin[k] = mesh->vertices[i3k];
        }
      }
      return Eigen::Vector3d(vmax[0] - vmin[0], vmax[1] - vmin[1], vmax[2] - vmin[2]);
    }
    else
      return Eigen::Vector3d(0.0, 0.0, 0.0);
  }
  else
    return Eigen::Vector3d(0.0, 0.0, 0.0);
}

void computeShapeBoundingSphere(const Shape* shape, Eigen::Vector3d& center, double& radius)
{
  center.x() = 0.0;
  center.y() = 0.0;
  center.z() = 0.0;
  radius = 0.0;

  if (shape->type == SPHERE)
  {
    radius = static_cast<const Sphere*>(shape)->radius;
  }
  else if (shape->type == BOX)
  {
    const double* sz = static_cast<const Box*>(shape)->size;
    double half_width = sz[0] * 0.5;
    double half_height = sz[1] * 0.5;
    double half_depth = sz[2] * 0.5;
    radius = std::sqrt(half_width * half_width + half_height * half_height + half_depth * half_depth);
  }
  else if (shape->type == CYLINDER)
  {
    double cyl_radius = static_cast<const Cylinder*>(shape)->radius;
    double half_len = static_cast<const Cylinder*>(shape)->length * 0.5;
    radius = std::sqrt(cyl_radius * cyl_radius + half_len * half_len);
  }
  else if (shape->type == CONE)
  {
    double cone_radius = static_cast<const Cone*>(shape)->radius;
    double cone_height = static_cast<const Cone*>(shape)->length;

    if (cone_height > cone_radius)
    {
      // center of sphere is intersection of perpendicular bisectors:
      double z = (cone_height - (cone_radius * cone_radius / cone_height)) * 0.5;
      center.z() = z - (cone_height * 0.5);
      radius = cone_height - z;
    }
    else
    {
      // short cone.  Bounding sphere touches base only.
      center.z() = -(cone_height * 0.5);
      radius = cone_radius;
    }
  }
  else if (shape->type == MESH)
  {
    const Mesh* mesh = static_cast<const Mesh*>(shape);
    if (mesh->vertex_count > 1)
    {
      double mx = std::numeric_limits<double>::max();
      Eigen::Vector3d min(mx, mx, mx);
      Eigen::Vector3d max(-mx, -mx, -mx);
      unsigned int cnt = mesh->vertex_count * 3;
      for (unsigned int i = 0; i < cnt; i += 3)
      {
        Eigen::Vector3d v(mesh->vertices[i + 0], mesh->vertices[i + 1], mesh->vertices[i + 2]);
        min = min.cwiseMin(v);
        max = max.cwiseMax(v);
      }

      center = (min + max) * 0.5;
      radius = (max - min).norm() * 0.5;
    }
  }
}

bool constructMsgFromShape(const Shape* shape, ShapeMsg& shape_msg)
{
  if (shape->type == SPHERE)
  {
    shape_msgs::msg::SolidPrimitive s;
    s.type = shape_msgs::msg::SolidPrimitive::SPHERE;
    s.dimensions.resize(geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::SPHERE>());
    s.dimensions[shape_msgs::msg::SolidPrimitive::SPHERE_RADIUS] = static_cast<const Sphere*>(shape)->radius;
    shape_msg = s;
  }
  else if (shape->type == BOX)
  {
    shape_msgs::msg::SolidPrimitive s;
    s.type = shape_msgs::msg::SolidPrimitive::BOX;
    const double* sz = static_cast<const Box*>(shape)->size;
    s.dimensions.resize(geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::BOX>());
    s.dimensions[shape_msgs::msg::SolidPrimitive::BOX_X] = sz[0];
    s.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Y] = sz[1];
    s.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Z] = sz[2];
    shape_msg = s;
  }
  else if (shape->type == CYLINDER)
  {
    shape_msgs::msg::SolidPrimitive s;
    s.type = shape_msgs::msg::SolidPrimitive::CYLINDER;
    s.dimensions.resize(geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CYLINDER>());
    s.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_RADIUS] = static_cast<const Cylinder*>(shape)->radius;
    s.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_HEIGHT] = static_cast<const Cylinder*>(shape)->length;
    shape_msg = s;
  }
  else if (shape->type == CONE)
  {
    shape_msgs::msg::SolidPrimitive s;
    s.type = shape_msgs::msg::SolidPrimitive::CONE;
    s.dimensions.resize(geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CONE>());
    s.dimensions[shape_msgs::msg::SolidPrimitive::CONE_RADIUS] = static_cast<const Cone*>(shape)->radius;
    s.dimensions[shape_msgs::msg::SolidPrimitive::CONE_HEIGHT] = static_cast<const Cone*>(shape)->length;
    shape_msg = s;
  }
  else if (shape->type == PLANE)
  {
    shape_msgs::msg::Plane s;
    const Plane* p = static_cast<const Plane*>(shape);
    s.coef[0] = p->a;
    s.coef[1] = p->b;
    s.coef[2] = p->c;
    s.coef[3] = p->d;
    shape_msg = s;
  }
  else if (shape->type == MESH)
  {
    shape_msgs::msg::Mesh s;
    const Mesh* mesh = static_cast<const Mesh*>(shape);
    s.vertices.resize(mesh->vertex_count);
    s.triangles.resize(mesh->triangle_count);

    for (unsigned int i = 0; i < mesh->vertex_count; ++i)
    {
      unsigned int i3 = i * 3;
      s.vertices[i].x = mesh->vertices[i3];
      s.vertices[i].y = mesh->vertices[i3 + 1];
      s.vertices[i].z = mesh->vertices[i3 + 2];
    }

    for (unsigned int i = 0; i < s.triangles.size(); ++i)
    {
      unsigned int i3 = i * 3;
      s.triangles[i].vertex_indices[0] = mesh->triangles[i3];
      s.triangles[i].vertex_indices[1] = mesh->triangles[i3 + 1];
      s.triangles[i].vertex_indices[2] = mesh->triangles[i3 + 2];
    }
    shape_msg = s;
  }
  else
  {
    CONSOLE_BRIDGE_logError("Unable to construct shape message for shape of type %d", (int)shape->type);
    return false;
  }

  return true;
}

void saveAsText(const Shape* shape, std::ostream& out)
{
  if (shape->type == SPHERE)
  {
    out << Sphere::STRING_NAME << std::endl;
    out << static_cast<const Sphere*>(shape)->radius << std::endl;
  }
  else if (shape->type == BOX)
  {
    out << Box::STRING_NAME << std::endl;
    const double* sz = static_cast<const Box*>(shape)->size;
    out << sz[0] << " " << sz[1] << " " << sz[2] << std::endl;
  }
  else if (shape->type == CYLINDER)
  {
    out << Cylinder::STRING_NAME << std::endl;
    out << static_cast<const Cylinder*>(shape)->radius << " " << static_cast<const Cylinder*>(shape)->length
        << std::endl;
  }
  else if (shape->type == CONE)
  {
    out << Cone::STRING_NAME << std::endl;
    out << static_cast<const Cone*>(shape)->radius << " " << static_cast<const Cone*>(shape)->length << std::endl;
  }
  else if (shape->type == PLANE)
  {
    out << Plane::STRING_NAME << std::endl;
    const Plane* p = static_cast<const Plane*>(shape);
    out << p->a << " " << p->b << " " << p->c << " " << p->d << std::endl;
  }
  else if (shape->type == MESH)
  {
    out << Mesh::STRING_NAME << std::endl;
    const Mesh* mesh = static_cast<const Mesh*>(shape);
    out << mesh->vertex_count << " " << mesh->triangle_count << std::endl;

    for (unsigned int i = 0; i < mesh->vertex_count; ++i)
    {
      unsigned int i3 = i * 3;
      out << mesh->vertices[i3] << " " << mesh->vertices[i3 + 1] << " " << mesh->vertices[i3 + 2] << std::endl;
    }

    for (unsigned int i = 0; i < mesh->triangle_count; ++i)
    {
      unsigned int i3 = i * 3;
      out << mesh->triangles[i3] << " " << mesh->triangles[i3 + 1] << " " << mesh->triangles[i3 + 2] << std::endl;
    }
  }
  else
  {
    CONSOLE_BRIDGE_logError("Unable to save shape of type %d", (int)shape->type);
  }
}

Shape* constructShapeFromText(std::istream& in)
{
  Shape* result = nullptr;
  if (in.good() && !in.eof())
  {
    std::string type;
    in >> type;
    if (in.good() && !in.eof())
    {
      if (type == Sphere::STRING_NAME)
      {
        double radius;
        in >> radius;
        result = new Sphere(radius);
      }
      else if (type == Box::STRING_NAME)
      {
        double x, y, z;
        in >> x >> y >> z;
        result = new Box(x, y, z);
      }
      else if (type == Cylinder::STRING_NAME)
      {
        double r, l;
        in >> r >> l;
        result = new Cylinder(r, l);
      }
      else if (type == Cone::STRING_NAME)
      {
        double r, l;
        in >> r >> l;
        result = new Cone(r, l);
      }
      else if (type == Plane::STRING_NAME)
      {
        double a, b, c, d;
        in >> a >> b >> c >> d;
        result = new Plane(a, b, c, d);
      }
      else if (type == Mesh::STRING_NAME)
      {
        unsigned int v, t;
        in >> v >> t;
        Mesh* m = new Mesh(v, t);
        result = m;
        for (unsigned int i = 0; i < m->vertex_count; ++i)
        {
          unsigned int i3 = i * 3;
          in >> m->vertices[i3] >> m->vertices[i3 + 1] >> m->vertices[i3 + 2];
        }
        for (unsigned int i = 0; i < m->triangle_count; ++i)
        {
          unsigned int i3 = i * 3;
          in >> m->triangles[i3] >> m->triangles[i3 + 1] >> m->triangles[i3 + 2];
        }
        m->computeTriangleNormals();
        m->computeVertexNormals();
      }
      else
        CONSOLE_BRIDGE_logError("Unknown shape type: '%s'", type.c_str());
    }
  }
  return result;
}

const std::string& shapeStringName(const Shape* shape)
{
  static const std::string unknown = "unknown";
  if (shape)
    switch (shape->type)
    {
      case SPHERE:
        return Sphere::STRING_NAME;
      case CYLINDER:
        return Cylinder::STRING_NAME;
      case CONE:
        return Cone::STRING_NAME;
      case BOX:
        return Box::STRING_NAME;
      case PLANE:
        return Plane::STRING_NAME;
      case MESH:
        return Mesh::STRING_NAME;
      case OCTREE:
        return OcTree::STRING_NAME;
      default:
        return unknown;
    }
  else
  {
    static const std::string empty;
    return empty;
  }
}
}  // namespace shapes
