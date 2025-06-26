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

#ifndef GEOMETRIC_SHAPES_MESH_OPERATIONS_
#define GEOMETRIC_SHAPES_MESH_OPERATIONS_

#include "geometric_shapes/shapes.h"
#include <eigen_stl_containers/eigen_stl_containers.h>
#include <vector>

// forward declaration of aiScene (caller needs to include assimp if aiScene is used)
class aiScene;

namespace shapes
{
/** \brief Load a mesh from a set of vertices.

    Triangles are constructed using index values from the triangles vector.
    Triangle k has vertices at index values triangles[3k], triangles[3k+1], triangles[3k+2] */
Mesh* createMeshFromVertices(const EigenSTL::vector_Vector3d& vertices, const std::vector<unsigned int>& triangles);

/** \brief Load a mesh from a set of vertices.

    Every 3 vertices are considered a triangle. Repeating vertices are identified and
    the set of triangle indices is constructed. The normal at each triangle is also computed */
Mesh* createMeshFromVertices(const EigenSTL::vector_Vector3d& source);

/** \brief Load a mesh from a resource that contains a mesh that can be loaded by assimp */
Mesh* createMeshFromResource(const std::string& resource);

/** \brief Load a mesh from a resource that contains a mesh that can be loaded by assimp */
Mesh* createMeshFromResource(const std::string& resource, const Eigen::Vector3d& scale);

/** \brief Load a mesh from a binary stream that contains a mesh that can be loaded by assimp */
Mesh* createMeshFromBinary(const char* buffer, std::size_t size, const std::string& assimp_hint = std::string());

/** \brief Load a mesh from a resource that contains a mesh that can be loaded by assimp */
Mesh* createMeshFromBinary(const char* buffer, std::size_t size, const Eigen::Vector3d& scale,
                           const std::string& assimp_hint = std::string());

/** \brief Load a mesh from an assimp datastructure */
Mesh* createMeshFromAsset(const aiScene* scene, const Eigen::Vector3d& scale,
                          const std::string& assimp_hint = std::string());

/** \brief Load a mesh from an assimp datastructure */
Mesh* createMeshFromAsset(const aiScene* scene, const std::string& assimp_hint = std::string());

/** \brief Construct a mesh from a primitive shape that is NOT already a mesh. This call allocates a new object. */
Mesh* createMeshFromShape(const Shape* shape);

/** \brief Construct a mesh from a box */
Mesh* createMeshFromShape(const Box& box);

/** \brief Construct a mesh from a sphere */
Mesh* createMeshFromShape(const Sphere& sphere);

/** \brief Construct a mesh from a cylinder */
Mesh* createMeshFromShape(const Cylinder& cylinder);

/** \brief Construct a mesh from a cone */
Mesh* createMeshFromShape(const Cone& cone);

/** \brief Write the mesh to a buffer in STL format */
void writeSTLBinary(const Mesh* mesh, std::vector<char>& buffer);
}  // namespace shapes

#endif
