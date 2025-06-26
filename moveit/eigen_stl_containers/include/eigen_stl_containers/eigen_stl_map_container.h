/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Ioan Sucan */

#ifndef EIGEN_STL_CONTAINERS_EIGEN_STL_MAP_CONTAINER_
#define EIGEN_STL_CONTAINERS_EIGEN_STL_MAP_CONTAINER_

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <map>
#include <string>
#include <functional>

namespace EigenSTL
{

typedef std::map<std::string, Eigen::Vector3d, std::less<std::string>,
                 Eigen::aligned_allocator<std::pair<const std::string, Eigen::Vector3d> > > map_string_Vector3d;

typedef std::map<std::string, Eigen::Vector3f, std::less<std::string>,
                 Eigen::aligned_allocator<std::pair<const std::string, Eigen::Vector3f> > > map_string_Vector3f;

typedef std::map<std::string, Eigen::Affine3d, std::less<std::string>,
                 Eigen::aligned_allocator<std::pair<const std::string, Eigen::Affine3d> > > map_string_Affine3d;

typedef std::map<std::string, Eigen::Affine3f, std::less<std::string>,
                 Eigen::aligned_allocator<std::pair<const std::string, Eigen::Affine3f> > > map_string_Affine3f;

}

#endif
