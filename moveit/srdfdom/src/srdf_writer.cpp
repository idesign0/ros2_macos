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
 *   * Neither the name of Willow Garage nor the names of its
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

/* Author: Dave Coleman */

#include <srdfdom/srdf_writer.h>

using namespace tinyxml2;

namespace
{
template <class T>
std::string toString(const T& t)
{
  // convert to string using no locale
  std::ostringstream oss;
  oss.imbue(std::locale::classic());
  oss << t;
  return oss.str();
}
}  // namespace

namespace srdf
{
// ******************************************************************************************
// Constructor
// ******************************************************************************************
SRDFWriter::SRDFWriter()
{
  // Intialize the SRDF model
  srdf_model_.reset(new srdf::Model());
}

// ******************************************************************************************
// Destructor
// ******************************************************************************************
SRDFWriter::~SRDFWriter()
{
}

// ******************************************************************************************
// Load SRDF data from a pre-populated string
// ******************************************************************************************
bool SRDFWriter::initString(const urdf::ModelInterface& robot_model, const std::string& srdf_string)
{
  // Parse string into srdf_model_ and Error check
  if (!srdf_model_->initString(robot_model, srdf_string))
  {
    return false;  // error loading file. improper format?
  }

  // copy fields into this object
  initModel(robot_model, *srdf_model_);

  return true;
}

// ******************************************************************************************
// Load SRDF data from a pre-populated string
// ******************************************************************************************
void SRDFWriter::initModel(const urdf::ModelInterface& robot_model, const srdf::Model& srdf_model)
{
  // copy to internal srdf_model_
  if (srdf_model_.get() != &srdf_model)
  {
    *srdf_model_ = srdf_model;
  }

  // Copy all read-only data from srdf model to this object
  no_default_collision_links_ = srdf_model_->getNoDefaultCollisionLinks();
  enabled_collision_pairs_ = srdf_model_->getEnabledCollisionPairs();
  disabled_collision_pairs_ = srdf_model_->getDisabledCollisionPairs();
  link_sphere_approximations_ = srdf_model_->getLinkSphereApproximations();
  groups_ = srdf_model_->getGroups();
  virtual_joints_ = srdf_model_->getVirtualJoints();
  end_effectors_ = srdf_model_->getEndEffectors();
  group_states_ = srdf_model_->getGroupStates();
  passive_joints_ = srdf_model_->getPassiveJoints();
  joint_properties_ = srdf_model_->getJointProperties();

  // Copy the robot name b/c the root xml element requires this attribute
  robot_name_ = robot_model.getName();
}

// ******************************************************************************************
// Update the SRDF Model class using a new SRDF string
// ******************************************************************************************
void SRDFWriter::updateSRDFModel(const urdf::ModelInterface& robot_model)
{
  // Get an up to date SRDF String
  const std::string srdf_string = getSRDFString();

  // Error check
  if (!srdf_model_->initString(robot_model, srdf_string))
  {
    throw std::runtime_error("Unable to update the SRDF Model");
  }
}

// ******************************************************************************************
// Save to file a generated SRDF document
// ******************************************************************************************
bool SRDFWriter::writeSRDF(const std::string& file_path)
{
  // Generate the SRDF
  XMLDocument document;
  generateSRDF(document);

  // Save to file
  return document.SaveFile(file_path.c_str()) == XML_SUCCESS;
}

// ******************************************************************************************
// Get a string of a generated SRDF document
// ******************************************************************************************
std::string SRDFWriter::getSRDFString()
{
  // Generate the SRDF
  XMLDocument document;
  generateSRDF(document);

  // Setup printer
  XMLPrinter printer;
  document.Accept(&printer);

  // Return string
  return printer.CStr();
}

// ******************************************************************************************
// Generate SRDF XML of all contained data
// ******************************************************************************************
void SRDFWriter::generateSRDF(XMLDocument& document)
{
  XMLDeclaration* decl = document.NewDeclaration();
  document.InsertEndChild(decl);

  // Convenience comments
  XMLComment* comment = document.NewComment("This does not replace URDF, and is not an extension of URDF.\n    "
                                            "This is a format for representing semantic information about the robot "
                                            "structure.\n    "
                                            "A URDF file must exist for this robot as well, "
                                            "where the joints and the links that are referenced are defined\n");
  document.InsertEndChild(comment);

  // Root
  XMLElement* robot_root = document.NewElement("robot");
  robot_root->SetAttribute("name", robot_name_.c_str());  // robot name
  document.InsertEndChild(robot_root);

  // Add Groups
  createGroupsXML(robot_root);

  // Add Group States
  createGroupStatesXML(robot_root);

  // Add End Effectors
  createEndEffectorsXML(robot_root);

  // Add Virtual Joints
  createVirtualJointsXML(robot_root);

  // Add Passive Joints
  createPassiveJointsXML(robot_root);

  // Add Joint Properties
  createJointPropertiesXML(robot_root);

  // Add Link Sphere approximations
  createLinkSphereApproximationsXML(robot_root);

  // Create disable_default_collisions tags and tags to re-enable specific pairs
  createCollisionDefaultsXML(robot_root);

  // Add Disabled Collisions
  createDisabledCollisionPairsXML(robot_root);
}

// ******************************************************************************************
// Generate XML for SRDF groups
// ******************************************************************************************
void SRDFWriter::createGroupsXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  if (groups_.size())  // only show comments if there are corresponding elements
  {
    XMLComment* comment;
    comment = doc->NewComment("GROUPS: Representation of a set of joints and links. This can be useful for specifying "
                              "DOF to plan for, defining arms, end effectors, etc");
    root->InsertEndChild(comment);
    comment = doc->NewComment("LINKS: When a link is specified, the parent joint of that link (if it exists) is "
                              "automatically included");
    root->InsertEndChild(comment);
    comment = doc->NewComment("JOINTS: When a joint is specified, the child link of that joint (which will always "
                              "exist) is automatically included");
    root->InsertEndChild(comment);
    comment = doc->NewComment("CHAINS: When a chain is specified, all the links along the chain (including endpoints) "
                              "are included in the group. Additionally, all the joints that are parents to included "
                              "links are also included. This means that joints along the chain and the parent joint "
                              "of the base link are included in the group");
    root->InsertEndChild(comment);
    comment = doc->NewComment("SUBGROUPS: Groups can also be formed by referencing to already defined group names");
    root->InsertEndChild(comment);
  }

  // Loop through all of the top groups
  for (std::vector<srdf::Model::Group>::iterator group_it = groups_.begin(); group_it != groups_.end(); ++group_it)
  {
    // Create group element
    XMLElement* group = doc->NewElement("group");
    group->SetAttribute("name", group_it->name_.c_str());  // group name
    root->InsertEndChild(group);

    // LINKS
    for (std::vector<std::string>::const_iterator link_it = group_it->links_.begin(); link_it != group_it->links_.end();
         ++link_it)
    {
      XMLElement* link = doc->NewElement("link");
      link->SetAttribute("name", (*link_it).c_str());  // link name
      group->InsertEndChild(link);
    }

    // JOINTS
    for (std::vector<std::string>::const_iterator joint_it = group_it->joints_.begin();
         joint_it != group_it->joints_.end(); ++joint_it)
    {
      XMLElement* joint = doc->NewElement("joint");
      joint->SetAttribute("name", (*joint_it).c_str());  // joint name
      group->InsertEndChild(joint);
    }

    // CHAINS
    for (std::vector<std::pair<std::string, std::string> >::const_iterator chain_it = group_it->chains_.begin();
         chain_it != group_it->chains_.end(); ++chain_it)
    {
      XMLElement* chain = doc->NewElement("chain");
      chain->SetAttribute("base_link", chain_it->first.c_str());
      chain->SetAttribute("tip_link", chain_it->second.c_str());
      group->InsertEndChild(chain);
    }

    // SUBGROUPS
    for (std::vector<std::string>::const_iterator subgroup_it = group_it->subgroups_.begin();
         subgroup_it != group_it->subgroups_.end(); ++subgroup_it)
    {
      XMLElement* subgroup = doc->NewElement("group");
      subgroup->SetAttribute("name", (*subgroup_it).c_str());  // subgroup name
      group->InsertEndChild(subgroup);
    }
  }
}

// ******************************************************************************************
// Generate XML for SRDF link collision spheres
// ******************************************************************************************
void SRDFWriter::createLinkSphereApproximationsXML(XMLElement* root)
{
  if (link_sphere_approximations_.empty())  // skip it if there are none
    return;

  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  XMLComment* comment = doc->NewComment("COLLISION SPHERES: Purpose: Define a set of spheres that bounds a link.");
  root->InsertEndChild(comment);

  for (std::vector<srdf::Model::LinkSpheres>::const_iterator link_sphere_it = link_sphere_approximations_.begin();
       link_sphere_it != link_sphere_approximations_.end(); ++link_sphere_it)
  {
    if (link_sphere_it->spheres_.empty())  // skip if no spheres for this link
      continue;

    // Create new element for the link
    XMLElement* link = doc->NewElement("link_sphere_approximation");
    link->SetAttribute("link", link_sphere_it->link_.c_str());
    root->InsertEndChild(link);

    // Add all spheres for the link
    for (std::vector<srdf::Model::Sphere>::const_iterator sphere_it = link_sphere_it->spheres_.begin();
         sphere_it != link_sphere_it->spheres_.end(); ++sphere_it)
    {
      XMLElement* sphere = doc->NewElement("sphere");
      std::stringstream center;
      center.precision(20);
      center << sphere_it->center_x_ << " " << sphere_it->center_y_ << " " << sphere_it->center_z_;
      sphere->SetAttribute("center", center.str().c_str());
      sphere->SetAttribute("radius", toString(sphere_it->radius_).c_str());
      link->InsertEndChild(sphere);
    }
  }
}

// ******************************************************************************************
// Generate XML for SRDF collision defaults of robot links
// ******************************************************************************************
void SRDFWriter::createCollisionDefaultsXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  if (!no_default_collision_links_.empty())  // only show comments if there are corresponding elements
  {
    XMLComment* comment = doc->NewComment("DEFAULT COLLISIONS: By default it is assumed that any link of the robot "
                                          "could potentially come into collision with any other link in the robot. "
                                          "This tag allows to revert this behavior and disable collisions by default.");
    root->InsertEndChild(comment);
  }

  for (const std::string& name : no_default_collision_links_)
  {
    XMLElement* entry = doc->NewElement("disable_default_collisions");
    entry->SetAttribute("link", name.c_str());
    root->InsertEndChild(entry);
  }
  // Write enabled collision pairs
  createCollisionPairsXML(root, "enable_collisions", enabled_collision_pairs_);
}

// ******************************************************************************************
// Generate XML for SRDF disabled/enabled collisions of robot link pairs
// ******************************************************************************************
void SRDFWriter::createCollisionPairsXML(XMLElement* root, const char* tag_name,
                                         const std::vector<Model::CollisionPair>& pairs)
{
  XMLDocument* doc = root->GetDocument();

  for (const srdf::Model::CollisionPair& pair : pairs)
  {
    // Create new element for each link pair
    XMLElement* entry = doc->NewElement(tag_name);
    entry->SetAttribute("link1", pair.link1_.c_str());
    entry->SetAttribute("link2", pair.link2_.c_str());
    entry->SetAttribute("reason", pair.reason_.c_str());

    root->InsertEndChild(entry);
  }
}

// ******************************************************************************************
// Generate XML for SRDF disabled collisions of robot link pairs
// ******************************************************************************************
void SRDFWriter::createDisabledCollisionPairsXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  if (disabled_collision_pairs_.empty())
    return;

  XMLComment* comment = doc->NewComment("DISABLE COLLISIONS: By default it is assumed that any link of the robot "
                                        "could potentially come into collision with any other link in the robot. "
                                        "This tag disables collision checking between a specified pair of links. ");
  root->InsertEndChild(comment);
  createCollisionPairsXML(root, "disable_collisions", disabled_collision_pairs_);
}

// ******************************************************************************************
// Generate XML for SRDF group states
// ******************************************************************************************
void SRDFWriter::createGroupStatesXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  if (group_states_.size())  // only show comments if there are corresponding elements
  {
    XMLComment* comment = doc->NewComment("GROUP STATES: Purpose: Define a named state for a particular group, in "
                                          "terms of joint values. This is useful to define states like 'folded arms'");
    root->InsertEndChild(comment);
  }

  for (std::vector<srdf::Model::GroupState>::const_iterator state_it = group_states_.begin();
       state_it != group_states_.end(); ++state_it)
  {
    // Create new element for each group state
    XMLElement* state = doc->NewElement("group_state");
    state->SetAttribute("name", state_it->name_.c_str());
    state->SetAttribute("group", state_it->group_.c_str());
    root->InsertEndChild(state);

    // Add all joints
    for (std::map<std::string, std::vector<double> >::const_iterator value_it = state_it->joint_values_.begin();
         value_it != state_it->joint_values_.end(); ++value_it)
    {
      XMLElement* joint = doc->NewElement("joint");
      joint->SetAttribute("name", value_it->first.c_str());                 // joint name
      joint->SetAttribute("value", toString(value_it->second[0]).c_str());  // joint value

      // TODO: use the vector to support multi-DOF joints
      state->InsertEndChild(joint);
    }
  }
}

// ******************************************************************************************
// Generate XML for SRDF end effectors
// ******************************************************************************************
void SRDFWriter::createEndEffectorsXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  if (end_effectors_.size())  // only show comments if there are corresponding elements
  {
    XMLComment* comment = doc->NewComment("END EFFECTOR: Purpose: Represent information about an end effector.");
    root->InsertEndChild(comment);
  }

  for (std::vector<srdf::Model::EndEffector>::const_iterator effector_it = end_effectors_.begin();
       effector_it != end_effectors_.end(); ++effector_it)
  {
    // Create new element for each link pair
    XMLElement* effector = doc->NewElement("end_effector");
    effector->SetAttribute("name", effector_it->name_.c_str());
    effector->SetAttribute("parent_link", effector_it->parent_link_.c_str());
    effector->SetAttribute("group", effector_it->component_group_.c_str());
    if (!effector_it->parent_group_.empty())
      effector->SetAttribute("parent_group", effector_it->parent_group_.c_str());
    root->InsertEndChild(effector);
  }
}

// ******************************************************************************************
// Generate XML for SRDF virtual joints
// ******************************************************************************************
void SRDFWriter::createVirtualJointsXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  // Convenience comments
  if (virtual_joints_.size())  // only show comments if there are corresponding elements
  {
    XMLComment* comment = doc->NewComment("VIRTUAL JOINT: Purpose: this element defines a virtual joint between a "
                                          "robot link and an external frame of reference (considered fixed with "
                                          "respect to the robot)");
    root->InsertEndChild(comment);
  }

  for (std::vector<srdf::Model::VirtualJoint>::const_iterator virtual_it = virtual_joints_.begin();
       virtual_it != virtual_joints_.end(); ++virtual_it)
  {
    // Create new element for each link pair
    XMLElement* virtual_joint = doc->NewElement("virtual_joint");
    virtual_joint->SetAttribute("name", virtual_it->name_.c_str());
    virtual_joint->SetAttribute("type", virtual_it->type_.c_str());
    virtual_joint->SetAttribute("parent_frame", virtual_it->parent_frame_.c_str());
    virtual_joint->SetAttribute("child_link", virtual_it->child_link_.c_str());

    root->InsertEndChild(virtual_joint);
  }
}

void SRDFWriter::createPassiveJointsXML(XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  if (passive_joints_.size())
  {
    XMLComment* comment = doc->NewComment("PASSIVE JOINT: Purpose: this element is used to mark joints that are not "
                                          "actuated");
    root->InsertEndChild(comment);
  }
  for (std::vector<srdf::Model::PassiveJoint>::const_iterator p_it = passive_joints_.begin();
       p_it != passive_joints_.end(); ++p_it)
  {
    // Create new element for each link pair
    XMLElement* p_joint = doc->NewElement("passive_joint");
    p_joint->SetAttribute("name", p_it->name_.c_str());
    root->InsertEndChild(p_joint);
  }
}

void SRDFWriter::createJointPropertiesXML(tinyxml2::XMLElement* root)
{
  XMLDocument* doc = root->GetDocument();

  if (!joint_properties_.empty())
  {
    XMLComment* comment = doc->NewComment(
        "JOINT PROPERTIES: Purpose: Define a property for a particular joint (could be a virtual joint)");
    root->InsertEndChild(comment);
  }
  for (const auto& joint_properties : joint_properties_)
  {
    for (const auto& joint_property : joint_properties.second)
    {
      XMLElement* p_joint = doc->NewElement("joint_property");
      p_joint->SetAttribute("joint_name", joint_property.joint_name_.c_str());
      p_joint->SetAttribute("property_name", joint_property.property_name_.c_str());
      p_joint->SetAttribute("value", joint_property.value_.c_str());
      root->InsertEndChild(p_joint);
    }
  }
}
}  // namespace srdf
