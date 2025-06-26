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

#include "srdfdom/model.h"
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <console_bridge/console.h>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>

using namespace tinyxml2;

const std::vector<srdf::Model::JointProperty> srdf::Model::empty_vector_;

/** \brief Helper function to convert a std::string to double in a locale-independent way.
 \throws std::runtime_exception if not a valid number
*/
double toDouble(const std::string& s)
{
  // convert from string using no locale
  std::istringstream stream(s);
  stream.imbue(std::locale::classic());
  double result;
  stream >> result;
  if (stream.fail() || !stream.eof())
  {
    throw std::invalid_argument("Failed converting string to real number");
  }
  return result;
}

bool srdf::Model::isValidJoint(const urdf::ModelInterface& urdf_model, const std::string& name) const
{
  if (urdf_model.getJoint(name))
  {
    return true;
  }
  for (const srdf::Model::VirtualJoint& vj : virtual_joints_)
  {
    if (vj.name_ == name)
    {
      return true;
    }
  }
  return false;
}

void srdf::Model::loadVirtualJoints(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* vj_xml = robot_xml->FirstChildElement("virtual_joint"); vj_xml;
       vj_xml = vj_xml->NextSiblingElement("virtual_joint"))
  {
    const char* jname = vj_xml->Attribute("name");
    const char* child = vj_xml->Attribute("child_link");
    const char* parent = vj_xml->Attribute("parent_frame");
    const char* type = vj_xml->Attribute("type");
    if (!jname)
    {
      CONSOLE_BRIDGE_logError("Name of virtual joint is not specified", NULL);
      continue;
    }
    if (!child)
    {
      CONSOLE_BRIDGE_logError("Child link of virtual joint is not specified", NULL);
      continue;
    }
    if (!urdf_model.getLink(boost::trim_copy(std::string(child))))
    {
      CONSOLE_BRIDGE_logError("Virtual joint does not attach to a link on the robot (link '%s' is not known)", child);
      continue;
    }
    if (!parent)
    {
      CONSOLE_BRIDGE_logError("Parent frame of virtual joint is not specified", NULL);
      continue;
    }
    if (!type)
    {
      CONSOLE_BRIDGE_logError("Type of virtual joint is not specified", NULL);
      continue;
    }
    VirtualJoint vj;
    vj.type_ = std::string(type);
    boost::trim(vj.type_);
    std::transform(vj.type_.begin(), vj.type_.end(), vj.type_.begin(), ::tolower);
    if (vj.type_ != "planar" && vj.type_ != "floating" && vj.type_ != "fixed")
    {
      CONSOLE_BRIDGE_logError("Unknown type of joint: '%s'. Assuming 'fixed' instead. Other known types are 'planar' "
                              "and 'floating'.",
                              type);
      vj.type_ = "fixed";
    }
    vj.name_ = std::string(jname);
    boost::trim(vj.name_);
    vj.child_link_ = std::string(child);
    boost::trim(vj.child_link_);
    vj.parent_frame_ = std::string(parent);
    boost::trim(vj.parent_frame_);
    virtual_joints_.push_back(vj);
  }
}

void srdf::Model::loadGroups(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* group_xml = robot_xml->FirstChildElement("group"); group_xml;
       group_xml = group_xml->NextSiblingElement("group"))
  {
    const char* gname = group_xml->Attribute("name");
    if (!gname)
    {
      CONSOLE_BRIDGE_logError("Group name not specified", NULL);
      continue;
    }
    Group g;
    g.name_ = std::string(gname);
    boost::trim(g.name_);

    // get the links in the groups
    for (XMLElement* link_xml = group_xml->FirstChildElement("link"); link_xml;
         link_xml = link_xml->NextSiblingElement("link"))
    {
      const char* lname = link_xml->Attribute("name");
      if (!lname)
      {
        CONSOLE_BRIDGE_logError("Link name not specified", NULL);
        continue;
      }
      std::string lname_str = boost::trim_copy(std::string(lname));
      if (!urdf_model.getLink(lname_str))
      {
        CONSOLE_BRIDGE_logError("Link '%s' declared as part of group '%s' is not known to the URDF", lname, gname);
        continue;
      }
      g.links_.push_back(lname_str);
    }

    // get the joints in the groups
    for (XMLElement* joint_xml = group_xml->FirstChildElement("joint"); joint_xml;
         joint_xml = joint_xml->NextSiblingElement("joint"))
    {
      const char* jname = joint_xml->Attribute("name");
      if (!jname)
      {
        CONSOLE_BRIDGE_logError("Joint name not specified", NULL);
        continue;
      }
      std::string jname_str = boost::trim_copy(std::string(jname));
      if (!isValidJoint(urdf_model, jname_str))
      {
        CONSOLE_BRIDGE_logError("Joint '%s' declared as part of group '%s' is not known to the URDF", jname, gname);
        continue;
      }
      g.joints_.push_back(jname_str);
    }

    // get the chains in the groups
    for (XMLElement* chain_xml = group_xml->FirstChildElement("chain"); chain_xml;
         chain_xml = chain_xml->NextSiblingElement("chain"))
    {
      const char* base = chain_xml->Attribute("base_link");
      const char* tip = chain_xml->Attribute("tip_link");
      if (!base)
      {
        CONSOLE_BRIDGE_logError("Base link name not specified for chain", NULL);
        continue;
      }
      if (!tip)
      {
        CONSOLE_BRIDGE_logError("Tip link name not specified for chain", NULL);
        continue;
      }
      std::string base_str = boost::trim_copy(std::string(base));
      std::string tip_str = boost::trim_copy(std::string(tip));
      if (!urdf_model.getLink(base_str))
      {
        CONSOLE_BRIDGE_logError("Link '%s' declared as part of a chain in group '%s' is not known to the URDF", base,
                                gname);
        continue;
      }
      if (!urdf_model.getLink(tip_str))
      {
        CONSOLE_BRIDGE_logError("Link '%s' declared as part of a chain in group '%s' is not known to the URDF", tip,
                                gname);
        continue;
      }
      bool found = false;
      urdf::LinkConstSharedPtr l = urdf_model.getLink(tip_str);
      std::set<std::string> seen;
      while (!found && l)
      {
        seen.insert(l->name);
        if (l->name == base_str)
          found = true;
        else
          l = l->getParent();
      }
      if (!found)
      {
        l = urdf_model.getLink(base_str);
        while (!found && l)
        {
          if (seen.find(l->name) != seen.end())
            found = true;
          else
            l = l->getParent();
        }
      }
      if (found)
        g.chains_.push_back(std::make_pair(base_str, tip_str));
      else
        CONSOLE_BRIDGE_logError("Links '%s' and '%s' do not form a chain. Not included in group '%s'", base, tip, gname);
    }

    // get the subgroups in the groups
    for (XMLElement* subg_xml = group_xml->FirstChildElement("group"); subg_xml;
         subg_xml = subg_xml->NextSiblingElement("group"))
    {
      const char* sub = subg_xml->Attribute("name");
      if (!sub)
      {
        CONSOLE_BRIDGE_logError("Group name not specified when included as subgroup", NULL);
        continue;
      }
      g.subgroups_.push_back(boost::trim_copy(std::string(sub)));
    }
    if (g.links_.empty() && g.joints_.empty() && g.chains_.empty() && g.subgroups_.empty())
      CONSOLE_BRIDGE_logWarn("Group '%s' is empty.", gname);
    groups_.push_back(g);
  }

  // check the subgroups
  std::set<std::string> known_groups;
  bool update = true;
  while (update)
  {
    update = false;
    for (std::size_t i = 0; i < groups_.size(); ++i)
    {
      if (known_groups.find(groups_[i].name_) != known_groups.end())
        continue;
      if (groups_[i].subgroups_.empty())
      {
        known_groups.insert(groups_[i].name_);
        update = true;
      }
      else
      {
        bool ok = true;
        for (std::size_t j = 0; ok && j < groups_[i].subgroups_.size(); ++j)
          if (known_groups.find(groups_[i].subgroups_[j]) == known_groups.end())
            ok = false;
        if (ok)
        {
          known_groups.insert(groups_[i].name_);
          update = true;
        }
      }
    }
  }

  // if there are erroneous groups, keep only the valid ones
  if (known_groups.size() != groups_.size())
  {
    std::vector<Group> correct;
    for (std::size_t i = 0; i < groups_.size(); ++i)
      if (known_groups.find(groups_[i].name_) != known_groups.end())
        correct.push_back(groups_[i]);
      else
        CONSOLE_BRIDGE_logError("Group '%s' has unsatisfied subgroups", groups_[i].name_.c_str());
    groups_.swap(correct);
  }
}

void srdf::Model::loadGroupStates(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* gstate_xml = robot_xml->FirstChildElement("group_state"); gstate_xml;
       gstate_xml = gstate_xml->NextSiblingElement("group_state"))
  {
    const char* sname = gstate_xml->Attribute("name");
    const char* gname = gstate_xml->Attribute("group");
    if (!sname)
    {
      CONSOLE_BRIDGE_logError("Name of group state is not specified", NULL);
      continue;
    }
    if (!gname)
    {
      CONSOLE_BRIDGE_logError("Name of group for state '%s' is not specified", sname);
      continue;
    }

    GroupState gs;
    gs.name_ = boost::trim_copy(std::string(sname));
    gs.group_ = boost::trim_copy(std::string(gname));

    bool found = false;
    for (std::size_t k = 0; k < groups_.size(); ++k)
      if (groups_[k].name_ == gs.group_)
      {
        found = true;
        break;
      }
    if (!found)
    {
      CONSOLE_BRIDGE_logError("Group state '%s' specified for group '%s', but that group is not known", sname, gname);
      continue;
    }

    // get the joint values in the group state
    for (XMLElement* joint_xml = gstate_xml->FirstChildElement("joint"); joint_xml;
         joint_xml = joint_xml->NextSiblingElement("joint"))
    {
      const char* jname = joint_xml->Attribute("name");
      const char* jval = joint_xml->Attribute("value");
      if (!jname)
      {
        CONSOLE_BRIDGE_logError("Joint name not specified in group state '%s'", sname);
        continue;
      }
      if (!jval)
      {
        CONSOLE_BRIDGE_logError("Joint name not specified for joint '%s' in group state '%s'", jname, sname);
        continue;
      }
      std::string jname_str = boost::trim_copy(std::string(jname));
      if (!isValidJoint(urdf_model, jname_str))
      {
        CONSOLE_BRIDGE_logError("Joint '%s' declared as part of group state '%s' is not known to the URDF", jname,
                                sname);
        continue;
      }
      try
      {
        std::string jval_str = std::string(jval);
        std::istringstream ss(jval_str);
        while (ss.good() && !ss.eof())
        {
          double val;
          ss >> val >> std::ws;
          gs.joint_values_[jname_str].push_back(val);
        }
      }
      catch (const std::invalid_argument& e)
      {
        CONSOLE_BRIDGE_logError("Unable to parse joint value '%s'", jval);
      }
      catch (const std::out_of_range& e)
      {
        CONSOLE_BRIDGE_logError("Unable to parse joint value '%s' (out of range)", jval);
      }

      if (gs.joint_values_.empty())
        CONSOLE_BRIDGE_logError("Unable to parse joint value ('%s') for joint '%s' in group state '%s'", jval, jname,
                                sname);
    }
    group_states_.push_back(gs);
  }
}

void srdf::Model::loadEndEffectors(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* eef_xml = robot_xml->FirstChildElement("end_effector"); eef_xml;
       eef_xml = eef_xml->NextSiblingElement("end_effector"))
  {
    const char* ename = eef_xml->Attribute("name");
    const char* gname = eef_xml->Attribute("group");
    const char* parent = eef_xml->Attribute("parent_link");
    const char* parent_group = eef_xml->Attribute("parent_group");
    if (!ename)
    {
      CONSOLE_BRIDGE_logError("Name of end effector is not specified", NULL);
      continue;
    }
    if (!gname)
    {
      CONSOLE_BRIDGE_logError("Group not specified for end effector '%s'", ename);
      continue;
    }
    EndEffector e;
    e.name_ = std::string(ename);
    boost::trim(e.name_);
    e.component_group_ = std::string(gname);
    boost::trim(e.component_group_);
    bool found = false;
    for (std::size_t k = 0; k < groups_.size(); ++k)
      if (groups_[k].name_ == e.component_group_)
      {
        found = true;
        break;
      }
    if (!found)
    {
      CONSOLE_BRIDGE_logError("End effector '%s' specified for group '%s', but that group is not known", ename, gname);
      continue;
    }
    if (!parent)
    {
      CONSOLE_BRIDGE_logError("Parent link not specified for end effector '%s'", ename);
      continue;
    }
    e.parent_link_ = std::string(parent);
    boost::trim(e.parent_link_);
    if (!urdf_model.getLink(e.parent_link_))
    {
      CONSOLE_BRIDGE_logError("Link '%s' specified as parent for end effector '%s' is not known to the URDF", parent,
                              ename);
      continue;
    }
    if (parent_group)
    {
      e.parent_group_ = std::string(parent_group);
      boost::trim(e.parent_group_);
    }
    end_effectors_.push_back(e);
  }
}

void srdf::Model::loadLinkSphereApproximations(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* cslink_xml = robot_xml->FirstChildElement("link_sphere_approximation"); cslink_xml;
       cslink_xml = cslink_xml->NextSiblingElement("link_sphere_approximation"))
  {
    int non_0_radius_sphere_cnt = 0;
    const char* link_name = cslink_xml->Attribute("link");
    if (!link_name)
    {
      CONSOLE_BRIDGE_logError("Name of link is not specified in link_collision_spheres", NULL);
      continue;
    }

    LinkSpheres link_spheres;
    link_spheres.link_ = boost::trim_copy(std::string(link_name));
    if (!urdf_model.getLink(link_spheres.link_))
    {
      CONSOLE_BRIDGE_logError("Link '%s' is not known to URDF.", link_name);
      continue;
    }

    // get the spheres for this link
    int cnt = 0;
    for (XMLElement* sphere_xml = cslink_xml->FirstChildElement("sphere"); sphere_xml;
         sphere_xml = sphere_xml->NextSiblingElement("sphere"), cnt++)
    {
      const char* s_center = sphere_xml->Attribute("center");
      const char* s_r = sphere_xml->Attribute("radius");
      if (!s_center || !s_r)
      {
        CONSOLE_BRIDGE_logError("Link collision sphere %d for link '%s' does not have both center and radius.", cnt,
                                link_name);
        continue;
      }

      Sphere sphere;
      try
      {
        std::stringstream center(s_center);
        center.exceptions(std::stringstream::failbit | std::stringstream::badbit);
        center >> sphere.center_x_ >> sphere.center_y_ >> sphere.center_z_;
        sphere.radius_ = toDouble(std::string(s_r));
      }
      catch (std::stringstream::failure& e)
      {
        CONSOLE_BRIDGE_logError("Link collision sphere %d for link '%s' has bad center attribute value.", cnt,
                                link_name);
        continue;
      }
      catch (const std::invalid_argument& e)
      {
        CONSOLE_BRIDGE_logError("Link collision sphere %d for link '%s' has bad radius attribute value.", cnt,
                                link_name);
        continue;
      }
      catch (const std::out_of_range& e)
      {
        CONSOLE_BRIDGE_logError("Link collision sphere %d for link '%s' has an out of range radius attribute value.",
                                cnt, link_name);
        continue;
      }

      // ignore radius==0 spheres unless there is only 1 of them
      //
      // NOTE:
      //  - If a link has no sphere_approximation then one will be generated
      //     automatically containing a single sphere which encloses the entire
      //     collision geometry.  Internally this is represented by not having
      //     a link_sphere_approximations_ entry for this link.
      //  - If a link has only spheres with radius 0 then it will not be
      //     considered for collision detection.  In this case the internal
      //     representation is a single radius=0 sphere.
      //  - If a link has at least one sphere with radius>0 then those spheres
      //     are used.  Any radius=0 spheres are eliminated.
      if (sphere.radius_ > std::numeric_limits<double>::epsilon())
      {
        if (non_0_radius_sphere_cnt == 0)
          link_spheres.spheres_.clear();
        link_spheres.spheres_.push_back(sphere);
        non_0_radius_sphere_cnt++;
      }
      else if (non_0_radius_sphere_cnt == 0)
      {
        sphere.center_x_ = 0.0;
        sphere.center_y_ = 0.0;
        sphere.center_z_ = 0.0;
        sphere.radius_ = 0.0;
        link_spheres.spheres_.clear();
        link_spheres.spheres_.push_back(sphere);
      }
    }

    if (!link_spheres.spheres_.empty())
      link_sphere_approximations_.push_back(link_spheres);
  }
}

void srdf::Model::loadCollisionDefaults(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* xml = robot_xml->FirstChildElement("disable_default_collisions"); xml;
       xml = xml->NextSiblingElement("disable_default_collisions"))
  {
    const char* link_ = xml->Attribute("link");
    if (!link_)
    {
      CONSOLE_BRIDGE_logError("A disable_default_collisions tag needs to specify a link name");
      continue;
    }
    std::string link = boost::trim_copy(std::string(link_));
    if (!urdf_model.getLink(link))
    {
      CONSOLE_BRIDGE_logWarn("Link '%s' is not known to URDF. Cannot specify collision default.", link_);
      continue;
    }
    no_default_collision_links_.push_back(link);
  }
}

void srdf::Model::loadCollisionPairs(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml,
                                     const char* tag_name, std::vector<CollisionPair>& pairs)
{
  for (XMLElement* c_xml = robot_xml->FirstChildElement(tag_name); c_xml; c_xml = c_xml->NextSiblingElement(tag_name))
  {
    const char* link1 = c_xml->Attribute("link1");
    const char* link2 = c_xml->Attribute("link2");
    if (!link1 || !link2)
    {
      CONSOLE_BRIDGE_logError("A pair of links needs to be specified to disable/enable collisions");
      continue;
    }
    const char* reason = c_xml->Attribute("reason");

    CollisionPair pair{ boost::trim_copy(std::string(link1)), boost::trim_copy(std::string(link2)),
                        reason ? reason : "" };
    if (!urdf_model.getLink(pair.link1_))
    {
      CONSOLE_BRIDGE_logWarn("Link '%s' is not known to URDF. Cannot disable/enable collisons.", link1);
      continue;
    }
    if (!urdf_model.getLink(pair.link2_))
    {
      CONSOLE_BRIDGE_logWarn("Link '%s' is not known to URDF. Cannot disable/enable collisons.", link2);
      continue;
    }
    pairs.push_back(pair);
  }
}

void srdf::Model::loadPassiveJoints(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  constexpr char JOINT[] = "passive_joint";
  for (XMLElement* c_xml = robot_xml->FirstChildElement(JOINT); c_xml; c_xml = c_xml->NextSiblingElement(JOINT))
  {
    const char* name = c_xml->Attribute("name");
    if (!name)
    {
      CONSOLE_BRIDGE_logError("No name specified for passive joint. Ignoring.", NULL);
      continue;
    }
    PassiveJoint pj;
    pj.name_ = boost::trim_copy(std::string(name));

    if (!isValidJoint(urdf_model, pj.name_))
    {
      CONSOLE_BRIDGE_logError("Joint '%s' marked as passive is not known to the URDF. Ignoring.", name);
      continue;
    }
    passive_joints_.push_back(pj);
  }
}

void srdf::Model::loadJointProperties(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  for (XMLElement* prop_xml = robot_xml->FirstChildElement("joint_property"); prop_xml;
       prop_xml = prop_xml->NextSiblingElement("joint_property"))
  {
    const char* jname = prop_xml->Attribute("joint_name");
    const char* pname = prop_xml->Attribute("property_name");
    const char* pval = prop_xml->Attribute("value");
    if (!jname)
    {
      CONSOLE_BRIDGE_logError("joint_property is missing a joint name");
      continue;
    }
    if (!pname)
    {
      CONSOLE_BRIDGE_logError("Property name for joint '%s' is not specified", jname);
      continue;
    }
    if (!pval)
    {
      CONSOLE_BRIDGE_logError("Value is not specified for property '%s' of joint '%s'", pname, jname);
      continue;
    }

    JointProperty jp;
    jp.joint_name_ = boost::trim_copy(std::string(jname));
    jp.property_name_ = boost::trim_copy(std::string(pname));
    jp.value_ = std::string(pval);

    if (!isValidJoint(urdf_model, jp.joint_name_))
    {
      CONSOLE_BRIDGE_logError("Property defined for a joint '%s' that is not known to the URDF. Ignoring.",
                              jp.joint_name_.c_str());
      continue;
    }
    joint_properties_[jp.joint_name_].push_back(jp);
  }
}

bool srdf::Model::initXml(const urdf::ModelInterface& urdf_model, XMLElement* robot_xml)
{
  clear();
  if (!robot_xml || strcmp(robot_xml->Value(), "robot") != 0)
  {
    CONSOLE_BRIDGE_logError("Could not find the 'robot' element in the xml file", NULL);
    return false;
  }

  // get the robot name
  const char* name = robot_xml->Attribute("name");
  if (!name)
    CONSOLE_BRIDGE_logError("No name given for the robot.", NULL);
  else
  {
    name_ = std::string(name);
    boost::trim(name_);
    if (name_ != urdf_model.getName())
      CONSOLE_BRIDGE_logError("Semantic description is not specified for the same robot as the URDF", NULL);
  }

  loadVirtualJoints(urdf_model, robot_xml);
  loadGroups(urdf_model, robot_xml);
  loadGroupStates(urdf_model, robot_xml);
  loadEndEffectors(urdf_model, robot_xml);
  loadLinkSphereApproximations(urdf_model, robot_xml);
  loadCollisionDefaults(urdf_model, robot_xml);
  loadCollisionPairs(urdf_model, robot_xml, "enable_collisions", enabled_collision_pairs_);
  loadCollisionPairs(urdf_model, robot_xml, "disable_collisions", disabled_collision_pairs_);
  loadPassiveJoints(urdf_model, robot_xml);
  loadJointProperties(urdf_model, robot_xml);

  return true;
}

bool srdf::Model::initXml(const urdf::ModelInterface& urdf_model, XMLDocument* xml)
{
  XMLElement* robot_xml = xml ? xml->FirstChildElement("robot") : NULL;
  return initXml(urdf_model, robot_xml);
}

bool srdf::Model::initFile(const urdf::ModelInterface& urdf_model, const std::string& filename)
{
  // get the entire file
  std::string xml_string;
  std::fstream xml_file(filename.c_str(), std::fstream::in);
  if (xml_file.is_open())
  {
    while (xml_file.good())
    {
      std::string line;
      std::getline(xml_file, line);
      xml_string += (line + "\n");
    }
    xml_file.close();
    return initString(urdf_model, xml_string);
  }
  else
  {
    CONSOLE_BRIDGE_logError("Could not open file [%s] for parsing.", filename.c_str());
    return false;
  }
}

bool srdf::Model::initString(const urdf::ModelInterface& urdf_model, const std::string& xmlstring)
{
  XMLDocument xml_doc;
  if (xml_doc.Parse(xmlstring.c_str()) != XML_SUCCESS)
  {
    CONSOLE_BRIDGE_logError("Could not parse the SRDF XML File. %s", xml_doc.ErrorStr());
    return false;
  }
  return initXml(urdf_model, &xml_doc);
}

void srdf::Model::clear()
{
  name_ = "";
  groups_.clear();
  group_states_.clear();
  virtual_joints_.clear();
  end_effectors_.clear();
  link_sphere_approximations_.clear();
  no_default_collision_links_.clear();
  enabled_collision_pairs_.clear();
  disabled_collision_pairs_.clear();
  passive_joints_.clear();
}
