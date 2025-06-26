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

#ifndef GEOMETRIC_SHAPES_SHAPE_OPERATIONS_
#define GEOMETRIC_SHAPES_SHAPE_OPERATIONS_

#include <geometric_shapes/shapes.h>
#include <geometric_shapes/shape_messages.h>
#include <geometric_shapes/mesh_operations.h>
#include <visualization_msgs/msg/marker.hpp>
#include <iostream>

namespace shapes
{
/** \brief Construct the shape that corresponds to the message. Return NULL on failure. */
Shape* constructShapeFromMsg(const shape_msgs::msg::SolidPrimitive& shape_msg);

/** \brief Construct the shape that corresponds to the message. Return NULL on failure. */
Shape* constructShapeFromMsg(const shape_msgs::msg::Plane& shape_msg);

/** \brief Construct the shape that corresponds to the message. Return NULL on failure. */
Shape* constructShapeFromMsg(const shape_msgs::msg::Mesh& shape_msg);

/** \brief Construct the shape that corresponds to the message. Return NULL on failure. */
Shape* constructShapeFromMsg(const ShapeMsg& shape_msg);

/** \brief Construct the message that corresponds to the shape. Return false on failure. */
bool constructMsgFromShape(const Shape* shape, ShapeMsg& shape_msg);

/** \brief Construct the marker that corresponds to the shape. Return false on failure. */
bool constructMarkerFromShape(const Shape* shape, visualization_msgs::msg::Marker& mk,
                              bool use_mesh_triangle_list = false);

/** \brief Compute the extents of a shape */
Eigen::Vector3d computeShapeExtents(const ShapeMsg& shape_msg);

/** \brief Compute the extents of a shape */
Eigen::Vector3d computeShapeExtents(const Shape* shape);

/** \brief Compute a sphere bounding a shape */
void computeShapeBoundingSphere(const Shape* shape, Eigen::Vector3d& center, double& radius);

/** \brief Get the string name of the shape */
const std::string& shapeStringName(const Shape* shape);

/** \brief Save all the information about this shape as plain text */
void saveAsText(const Shape* shape, std::ostream& out);

/** \brief Construct a shape from plain text description */
Shape* constructShapeFromText(std::istream& in);
}  // namespace shapes

#endif
