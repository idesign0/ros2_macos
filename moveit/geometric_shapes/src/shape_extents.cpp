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

#include <geometric_shapes/shape_extents.h>
#include <geometric_shapes/solid_primitive_dims.h>
#include <limits>

void geometric_shapes::getShapeExtents(const shape_msgs::msg::SolidPrimitive& shape_msg, double& x_extent,
                                       double& y_extent, double& z_extent)
{
  x_extent = y_extent = z_extent = 0.0;

  if (shape_msg.type == shape_msgs::msg::SolidPrimitive::SPHERE)
  {
    if (shape_msg.dimensions.size() >=
        geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::SPHERE>())
      x_extent = y_extent = z_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::SPHERE_RADIUS] * 2.0;
  }
  else if (shape_msg.type == shape_msgs::msg::SolidPrimitive::BOX)
  {
    if (shape_msg.dimensions.size() >= geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::BOX>())
    {
      x_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_X];
      y_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Y];
      z_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Z];
    }
  }
  else if (shape_msg.type == shape_msgs::msg::SolidPrimitive::CYLINDER)
  {
    if (shape_msg.dimensions.size() >=
        geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CYLINDER>())
    {
      x_extent = y_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_RADIUS] * 2.0;
      z_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CYLINDER_HEIGHT];
    }
  }
  else if (shape_msg.type == shape_msgs::msg::SolidPrimitive::CONE)
  {
    if (shape_msg.dimensions.size() >=
        geometric_shapes::solidPrimitiveDimCount<shape_msgs::msg::SolidPrimitive::CONE>())
    {
      x_extent = y_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CONE_RADIUS] * 2.0;
      z_extent = shape_msg.dimensions[shape_msgs::msg::SolidPrimitive::CONE_HEIGHT];
    }
  }
}

void geometric_shapes::getShapeExtents(const shape_msgs::msg::Mesh& shape_msg, double& x_extent, double& y_extent,
                                       double& z_extent)
{
  x_extent = y_extent = z_extent = 0.0;
  if (!shape_msg.vertices.empty())
  {
    double xmin = std::numeric_limits<double>::max(), ymin = std::numeric_limits<double>::max(),
           zmin = std::numeric_limits<double>::max();
    double xmax = -std::numeric_limits<double>::max(), ymax = -std::numeric_limits<double>::max(),
           zmax = -std::numeric_limits<double>::max();
    for (const geometry_msgs::msg::Point& vertex : shape_msg.vertices)
    {
      if (vertex.x > xmax)
        xmax = vertex.x;
      if (vertex.x < xmin)
        xmin = vertex.x;
      if (vertex.y > ymax)
        ymax = vertex.y;
      if (vertex.y < ymin)
        ymin = vertex.y;
      if (vertex.z > zmax)
        zmax = vertex.z;
      if (vertex.z < zmin)
        zmin = vertex.z;
    }
    x_extent = xmax - xmin;
    y_extent = ymax - ymin;
    z_extent = zmax - zmin;
  }
}
