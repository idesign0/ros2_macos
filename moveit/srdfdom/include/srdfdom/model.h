/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
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

/* Author Ioan Sucan */

#ifndef SRDF_MODEL_
#define SRDF_MODEL_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <tinyxml2.h>

#if __has_include(<urdf/model.hpp>)
#include <urdf/model.hpp>
#else
#include <urdf/model.h>
#endif

#include "visibility_control.h"

/// Main namespace
namespace srdf
{
/** \brief Representation of semantic information about the robot */
class SRDFDOM_PUBLIC Model
{
public:
  Model()
  {
  }

  ~Model()
  {
  }

  /// \brief Load Model from XMLElement
  bool initXml(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* xml);
  /// \brief Load Model from XMLDocument
  bool initXml(const urdf::ModelInterface& urdf_model, tinyxml2::XMLDocument* xml);
  /// \brief Load Model given a filename
  bool initFile(const urdf::ModelInterface& urdf_model, const std::string& filename);
  /// \brief Load Model from a XML-string
  bool initString(const urdf::ModelInterface& urdf_model, const std::string& xmlstring);

  /** \brief A group consists of a set of joints and the
      corresponding descendant links. There are multiple ways to
      specify a group. Directly specifying joints, links or
      chains, or referring to other defined groups. */
  struct Group
  {
    /// The name of the group
    std::string name_;

    /// Directly specified joints to be included in the
    /// group. Descendent links should be implicitly
    /// considered to be part of the group, although this
    /// parsed does not add them to links_. The joints are
    /// checked to be in the corresponding URDF.
    std::vector<std::string> joints_;

    /// Directly specified links to be included in the
    /// group. Parent joints should be implicitly considered
    /// to be part of the group. The links are checked to be
    /// in the corresponding URDF.
    std::vector<std::string> links_;

    /// Specify a chain of links (and the implicit joints) to
    /// be added to the group. Each chain is specified as a
    /// pair of base link and tip link. It is checked that the
    /// chain is indeed a chain in the specified URDF.
    std::vector<std::pair<std::string, std::string>> chains_;

    /// It is sometimes convenient to refer to the content of
    /// another group. A group can include the content of the
    /// referenced groups
    std::vector<std::string> subgroups_;
  };

  /// In addition to the joints specified in the URDF it is
  /// sometimes convenient to add special (virtual) joints. For
  /// example, to connect the robot to the environment in a
  /// meaningful way.
  struct VirtualJoint
  {
    /// The name of the new joint
    std::string name_;

    /// The type of this new joint. This can be "fixed" (0 DOF), "planar" (3 DOF: x,y,yaw) or "floating" (6DOF)
    std::string type_;

    /// The transform applied by this joint to the robot model brings that model to a particular frame.
    std::string parent_frame_;

    /// The link this joint applies to
    std::string child_link_;
  };

  /// Representation of an end effector
  struct EndEffector
  {
    /// The name of the end effector
    std::string name_;

    /// The name of the link this end effector connects to
    std::string parent_link_;

    /// The name of the group to be considered the parent (this group should contain parent_link_)
    /// If not specified, this member is empty.
    std::string parent_group_;

    /// The name of the group that includes the joints & links this end effector consists of
    std::string component_group_;
  };

  /// A named state for a particular group
  struct GroupState
  {
    /// The name of the state
    std::string name_;

    /// The name of the group this state is specified for
    std::string group_;

    /// The values of joints for this state. Each joint can have a value. We use a vector for the 'value' to support
    /// multi-DOF joints
    std::map<std::string, std::vector<double>> joint_values_;
  };

  /// The definition of a sphere
  struct Sphere
  {
    /// The center of the sphere in the link collision frame
    double center_x_;
    double center_y_;
    double center_z_;

    /// The radius of the sphere
    double radius_;
  };

  /// The definition of a list of spheres for a link.
  struct LinkSpheres
  {
    /// The name of the link (as in URDF).
    std::string link_;

    /// The spheres for the link.
    std::vector<Sphere> spheres_;
  };

  /// The definition of a disabled/enabled collision between two links
  struct CollisionPair
  {
    /// The name of the first link (as in URDF) of the disabled collision
    std::string link1_;

    /// The name of the second link (as in URDF) of the disabled collision
    std::string link2_;

    /// The reason why the collision check was disabled/enabled
    std::string reason_;
  };

  // Some joints can be passive (not actuated). This structure specifies information about such joints
  struct PassiveJoint
  {
    /// The name of the new joint
    std::string name_;
  };

  // Some joints may have additional properties.
  struct JointProperty
  {
    /// The name of the joint that this property belongs to
    std::string joint_name_;

    /// The name of the property
    std::string property_name_;

    /// The value of the property. Type not specified.
    std::string value_;
  };

  /// Get the name of this model
  const std::string& getName() const
  {
    return name_;
  }

  /// Get the list of links that should have collision checking disabled by default (and only selectively enabled)
  const std::vector<std::string>& getNoDefaultCollisionLinks() const
  {
    return no_default_collision_links_;
  }

  /// Get the list of pairs of links for which we explicitly re-enable collision (after having disabled it via a default)
  const std::vector<CollisionPair>& getEnabledCollisionPairs() const
  {
    return enabled_collision_pairs_;
  }

  /// Get the list of pairs of links for which we explicitly disable collision
  const std::vector<CollisionPair>& getDisabledCollisionPairs() const
  {
    return disabled_collision_pairs_;
  }

  /// Get the list of groups defined for this model
  const std::vector<Group>& getGroups() const
  {
    return groups_;
  }

  /// Get the list of virtual joints defined for this model
  const std::vector<VirtualJoint>& getVirtualJoints() const
  {
    return virtual_joints_;
  }

  /// Get the list of end effectors defined for this model
  const std::vector<EndEffector>& getEndEffectors() const
  {
    return end_effectors_;
  }

  /// Get the list of group states defined for this model
  const std::vector<GroupState>& getGroupStates() const
  {
    return group_states_;
  }

  /// Get the list of known passive joints
  const std::vector<PassiveJoint>& getPassiveJoints() const
  {
    return passive_joints_;
  }

  /// Get the collision spheres list
  const std::vector<LinkSpheres>& getLinkSphereApproximations() const
  {
    return link_sphere_approximations_;
  }

  /// Get the joint properties for a particular joint (empty vector if none)
  const std::vector<JointProperty>& getJointProperties(const std::string& joint_name) const
  {
    std::map<std::string, std::vector<JointProperty>>::const_iterator iter = joint_properties_.find(joint_name);
    if (iter == joint_properties_.end())
    {
      // We return a standard empty vector here rather than insert a new empty vector
      // into the map in order to keep the method const
      return empty_vector_;
    }
    return iter->second;
  }

  /// Get the joint properties list
  const std::map<std::string, std::vector<JointProperty>>& getJointProperties() const
  {
    return joint_properties_;
  }

  /// Clear the model
  void clear();

private:
  bool isValidJoint(const urdf::ModelInterface& urdf_model, const std::string& name) const;

  void loadVirtualJoints(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadGroups(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadGroupStates(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadEndEffectors(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadLinkSphereApproximations(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadCollisionDefaults(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadCollisionPairs(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml, const char* tag_name,
                          std::vector<CollisionPair>& pairs);
  void loadPassiveJoints(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);
  void loadJointProperties(const urdf::ModelInterface& urdf_model, tinyxml2::XMLElement* robot_xml);

  std::string name_;
  std::vector<Group> groups_;
  std::vector<GroupState> group_states_;
  std::vector<VirtualJoint> virtual_joints_;
  std::vector<EndEffector> end_effectors_;
  std::vector<LinkSpheres> link_sphere_approximations_;
  std::vector<std::string> no_default_collision_links_;
  std::vector<CollisionPair> enabled_collision_pairs_;
  std::vector<CollisionPair> disabled_collision_pairs_;
  std::vector<PassiveJoint> passive_joints_;
  std::map<std::string, std::vector<JointProperty>> joint_properties_;

  // Empty joint property vector
  static const std::vector<JointProperty> empty_vector_;
};
typedef std::shared_ptr<Model> ModelSharedPtr;
typedef std::shared_ptr<const Model> ModelConstSharedPtr;
}  // namespace srdf
#endif
