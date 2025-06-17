/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2008, Willow Garage, Inc.
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

/* Author: Wim Meeussen */

#include "kdl_parser/kdl_parser.hpp"

#include <string>
#include <vector>

#include "kdl/frames_io.hpp"
#include "urdf/model.h"
#include "urdf/urdfdom_compatibility.h"
#include "rcutils/logging_macros.h"


namespace kdl_parser
{

// construct vector
KDL::Vector toKdl(urdf::Vector3 v)
{
  return KDL::Vector(v.x, v.y, v.z);
}

// construct rotation
KDL::Rotation toKdl(urdf::Rotation r)
{
  return KDL::Rotation::Quaternion(r.x, r.y, r.z, r.w);
}

// construct pose
KDL::Frame toKdl(urdf::Pose p)
{
  return KDL::Frame(toKdl(p.rotation), toKdl(p.position));
}

// construct joint
KDL::Joint toKdl(urdf::JointSharedPtr jnt)
{
  KDL::Frame F_parent_jnt = toKdl(jnt->parent_to_joint_origin_transform);

  switch (jnt->type) {
    case urdf::Joint::FIXED: {
        return KDL::Joint(jnt->name, KDL::Joint::None);
      }
    case urdf::Joint::REVOLUTE: {
        KDL::Vector axis = toKdl(jnt->axis);
        return KDL::Joint(jnt->name, F_parent_jnt.p, F_parent_jnt.M * axis, KDL::Joint::RotAxis);
      }
    case urdf::Joint::CONTINUOUS: {
        KDL::Vector axis = toKdl(jnt->axis);
        return KDL::Joint(jnt->name, F_parent_jnt.p, F_parent_jnt.M * axis, KDL::Joint::RotAxis);
      }
    case urdf::Joint::PRISMATIC: {
        KDL::Vector axis = toKdl(jnt->axis);
        return KDL::Joint(jnt->name, F_parent_jnt.p, F_parent_jnt.M * axis, KDL::Joint::TransAxis);
      }
    default: {
        RCUTILS_LOG_WARN_NAMED(
          "kdl_parser",
          "Converting unknown joint type of joint '%s' into a fixed joint", jnt->name.c_str());
        return KDL::Joint(jnt->name, KDL::Joint::None);
      }
  }
  return KDL::Joint();
}

// construct inertia
KDL::RigidBodyInertia toKdl(urdf::InertialSharedPtr i)
{
  KDL::Frame origin = toKdl(i->origin);

  // the mass is frame independent
  double kdl_mass = i->mass;

  // kdl and urdf both specify the com position in the reference frame of the link
  KDL::Vector kdl_com = origin.p;

  // kdl specifies the inertia matrix in the reference frame of the link,
  // while the urdf specifies the inertia matrix in the inertia reference frame
  KDL::RotationalInertia urdf_inertia =
    KDL::RotationalInertia(i->ixx, i->iyy, i->izz, i->ixy, i->ixz, i->iyz);

  // Rotation operators are not defined for rotational inertia,
  // so we use the RigidBodyInertia operators (with com = 0) as a workaround
  KDL::RigidBodyInertia kdl_inertia_wrt_com_workaround =
    origin.M * KDL::RigidBodyInertia(0, KDL::Vector::Zero(), urdf_inertia);

  // Note that the RigidBodyInertia constructor takes the 3d inertia wrt the com
  // while the getRotationalInertia method returns the 3d inertia wrt the frame origin
  // (but having com = Vector::Zero() in kdl_inertia_wrt_com_workaround they match)
  KDL::RotationalInertia kdl_inertia_wrt_com =
    kdl_inertia_wrt_com_workaround.getRotationalInertia();

  return KDL::RigidBodyInertia(kdl_mass, kdl_com, kdl_inertia_wrt_com);
}


// recursive function to walk through tree
bool addChildrenToTree(urdf::LinkConstSharedPtr root, KDL::Tree & tree)
{
  std::vector<urdf::LinkSharedPtr> children = root->child_links;
  RCUTILS_LOG_DEBUG_NAMED(
    "kdl_parser",
    "Link %s had %zu children.", root->name.c_str(), children.size());

  // constructs the optional inertia
  KDL::RigidBodyInertia inert(0);
  if (root->inertial) {
    inert = toKdl(root->inertial);
  }

  // constructs the kdl joint
  KDL::Joint jnt = toKdl(root->parent_joint);

  // construct the kdl segment
  KDL::Segment sgm(root->name, jnt, toKdl(
      root->parent_joint->parent_to_joint_origin_transform), inert);

  // add segment to tree
  tree.addSegment(sgm, root->parent_joint->parent_link_name);

  // recurslively add all children
  for (size_t i = 0; i < children.size(); i++) {
    if (!addChildrenToTree(children[i], tree)) {
      return false;
    }
  }
  return true;
}


bool treeFromFile(const std::string & file, KDL::Tree & tree)
{
  std::ifstream t(file);
  std::stringstream buffer;
  buffer << t.rdbuf();

  return treeFromString(buffer.str(), tree);
}

bool treeFromString(const std::string & xml, KDL::Tree & tree)
{
  urdf::Model robot_model;
  if (!robot_model.initString(xml.c_str())) {
    RCUTILS_LOG_ERROR_NAMED("kdl_parser", "Could not generate robot model.");
    return false;
  }
  return treeFromUrdfModel(robot_model, tree);
}

bool treeFromUrdfModel(const urdf::ModelInterface & robot_model, KDL::Tree & tree)
{
  if (!robot_model.getRoot()) {
    return false;
  }

  tree = KDL::Tree(robot_model.getRoot()->name);

  // warn if root link has inertia. KDL does not support this
  if (robot_model.getRoot()->inertial) {
    RCUTILS_LOG_WARN_NAMED(
      "kdl_parser",
      "The root link %s has an inertia specified in the URDF, but KDL "
      "does not support a root link with an inertia.  As a workaround, you can add "
      "an extra dummy link to your URDF.", robot_model.getRoot()->name.c_str());
  }

  //  add all children
  for (size_t i = 0; i < robot_model.getRoot()->child_links.size(); i++) {
    if (!addChildrenToTree(robot_model.getRoot()->child_links[i], tree)) {
      return false;
    }
  }

  return true;
}

}  // namespace kdl_parser
