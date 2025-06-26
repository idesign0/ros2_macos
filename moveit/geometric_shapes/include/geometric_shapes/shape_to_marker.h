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

#ifndef GEOMETRIC_SHAPES_SHAPE_TO_MARKER_
#define GEOMETRIC_SHAPES_SHAPE_TO_MARKER_

#include <shape_msgs/msg/solid_primitive.hpp>
#include <shape_msgs/msg/mesh.hpp>
#include <visualization_msgs/msg/marker.hpp>

namespace geometric_shapes
{
/** \brief Convert a shape_msgs::msg::Mesh \e shape_msg to a visualization_msgs::msg::Marker \e marker.

    The corresponding marker will be constructed as a LINE_LIST (if \e use_mesh_triangle_list
    is false) or as a TRIANGLE_LIST (if \e use_mesh_triangle_list is true).
    On incorrect input, this function throws a std::runtime_error. */
void constructMarkerFromShape(const shape_msgs::msg::Mesh& shape_msg, visualization_msgs::msg::Marker& marker,
                              bool use_mesh_triangle_list = true);

/** \brief Convert a shape_msgs::msg::SolidPrimitive \e shape_msg to a visualization_msgs::msg::Marker \e marker.

    On incorrect input, this function throws a std::runtime_error. */
void constructMarkerFromShape(const shape_msgs::msg::SolidPrimitive& shape_msg,
                              visualization_msgs::msg::Marker& marker);
}  // namespace geometric_shapes

#endif
