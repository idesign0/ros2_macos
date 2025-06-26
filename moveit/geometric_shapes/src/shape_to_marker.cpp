// Copyright 2012 Willow Garage, Inc.
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

#include <geometric_shapes/shape_to_marker.h>
#include <geometric_shapes/solid_primitive_dims.h>
#include <sstream>
#include <stdexcept>

void geometric_shapes::constructMarkerFromShape(const shape_msgs::msg::SolidPrimitive& shape_msg,
                                                visualization_msgs::msg::Marker& mk)
{
  switch (shape_msg.type)
  {
    case shape_msgs::msg::SolidPrimitive::SPHERE:
      if (shape_msg.dimensions.size() <
          geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::SPHERE>())
        throw std::runtime_error("Insufficient dimensions in sphere definition");
      else
      {
        mk.type = visualization_msgs::msg::Marker::SPHERE;
        mk.scale.x = mk.scale.y = mk.scale.z =
            shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::SPHERE_RADIUS] * 2.0;
      }
      break;
    case shape_msgs::msg::SolidPrimitive::BOX:
      if (shape_msg.dimensions.size() <
          geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::BOX>())
        throw std::runtime_error("Insufficient dimensions in box definition");
      else
      {
        mk.type = visualization_msgs::msg::Marker::CUBE;
        mk.scale.x = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_X];
        mk.scale.y = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Y];
        mk.scale.z = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Z];
      }
      break;
    case shape_msgs::msg::SolidPrimitive::CONE:
      if (shape_msg.dimensions.size() <
          geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CONE>())
        throw std::runtime_error("Insufficient dimensions in cone definition");
      else
      {
        // there is no CONE marker, so this produces a cylinder marker as well
        mk.type = visualization_msgs::msg::Marker::CYLINDER;
        mk.scale.x = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CONE_RADIUS] * 2.0;
        mk.scale.y = mk.scale.x;
        mk.scale.z = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CONE_HEIGHT];
      }
      break;
    case shape_msgs::msg::SolidPrimitive::CYLINDER:
      if (shape_msg.dimensions.size() <
          geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CYLINDER>())
        throw std::runtime_error("Insufficient dimensions in cylinder definition");
      else
      {
        mk.type = visualization_msgs::msg::Marker::CYLINDER;
        mk.scale.x = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_RADIUS] * 2.0;
        mk.scale.y = mk.scale.x;
        mk.scale.z = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_HEIGHT];
      }
      break;
    default:
    {
      std::stringstream ss;
      ss << shape_msg.type;
      throw std::runtime_error("Unknown shape type: " + ss.str());
    }
  }
}

void geometric_shapes::constructMarkerFromShape(const shape_msgs::msg::Mesh& shape_msg,
                                                visualization_msgs::msg::Marker& mk, bool use_mesh_triangle_list)
{
  if (shape_msg.triangles.empty() || shape_msg.vertices.empty())
    throw std::runtime_error("Mesh definition is empty");
  if (use_mesh_triangle_list)
  {
    mk.type = visualization_msgs::msg::Marker::TRIANGLE_LIST;
    mk.scale.x = mk.scale.y = mk.scale.z = 1.0;
    for (std::size_t i = 0; i < shape_msg.triangles.size(); ++i)
    {
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[0]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[1]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[2]]);
    }
  }
  else
  {
    mk.type = visualization_msgs::msg::Marker::LINE_LIST;
    mk.scale.x = mk.scale.y = mk.scale.z = 0.01;
    for (std::size_t i = 0; i < shape_msg.triangles.size(); ++i)
    {
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[0]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[1]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[0]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[2]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[1]]);
      mk.points.push_back(shape_msg.vertices[shape_msg.triangles[i].vertex_indices[2]]);
    }
  }
}
