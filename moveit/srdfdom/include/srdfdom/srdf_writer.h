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

#ifndef _SRDFDOM_SRDF_WRITER_
#define _SRDFDOM_SRDF_WRITER_

#include <memory>
#include <srdfdom/model.h>  // use their struct datastructures

namespace srdf
{
// ******************************************************************************************
// ******************************************************************************************
// Class
// ******************************************************************************************
// ******************************************************************************************

class SRDFWriter
{
public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************
  /**
   * Constructor
   */
  SRDFWriter();

  /**
   * Destructor
   */
  ~SRDFWriter();

  /**
   * Initialize the SRDF writer with an exisiting SRDF file (optional)
   *
   * @param urdf_model a preloaded urdf model reference
   * @param srdf_string the text contents of an SRDF file
   *
   * @return bool if initialization was successful
   */
  bool initString(const urdf::ModelInterface& robot_model, const std::string& srdf_string);

  /**
   * Initialize the SRDF writer with an exisiting SRDF model (optional)
   *
   * @param urdf_model a preloaded urdf model reference
   * @param srdf_model a preloaded srdf model reference
   */
  void initModel(const urdf::ModelInterface& robot_model, const srdf::Model& srdf_model);

  /**
   * Update the SRDF Model class using a new SRDF string
   *
   * @param robot_model a loaded URDF model
   */
  void updateSRDFModel(const urdf::ModelInterface& robot_model);

  /**
   * Generate SRDF XML of all contained data and save to file
   *
   * @param file_path - string path location to save SRDF
   * @return bool - true if save was successful
   */
  bool writeSRDF(const std::string& file_path);

  /**
   * Get a string of a generated SRDF document
   *
   * @return string of XML of current SRDF contents
   */
  std::string getSRDFString();

  /**
   * Generate SRDF XML of all contained data
   *
   * @return TinyXML document that contains current SRDF data in this class
   */
  void generateSRDF(tinyxml2::XMLDocument& document);

  /**
   * Generate XML for SRDF groups
   *
   * @param root - TinyXML root element to attach sub elements to
   */
  void createGroupsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF link collision spheres
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createLinkSphereApproximationsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF collision defaults
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createCollisionDefaultsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF disabled collisions of robot link pairs
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createDisabledCollisionPairsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF group states of each joint's position
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createGroupStatesXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF end effectors
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createEndEffectorsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF virtual joints
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createVirtualJointsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF passive joints
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createPassiveJointsXML(tinyxml2::XMLElement* root);

  /**
   * Generate XML for SRDF joint properties
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createJointPropertiesXML(tinyxml2::XMLElement* root);

protected:
  /**
   * Generate XML for SRDF disabled/enabled collisions of robot link pairs
   *
   * @param root  - TinyXML root element to attach sub elements to
   */
  void createCollisionPairsXML(tinyxml2::XMLElement* root, const char* tag_name,
                               const std::vector<Model::CollisionPair>& pairs);

public:
  // ******************************************************************************************
  // Group Datastructures
  // ******************************************************************************************

  std::vector<Model::Group> groups_;
  std::vector<Model::GroupState> group_states_;
  std::vector<Model::VirtualJoint> virtual_joints_;
  std::vector<Model::EndEffector> end_effectors_;
  std::vector<Model::LinkSpheres> link_sphere_approximations_;
  std::vector<std::string> no_default_collision_links_;
  std::vector<Model::CollisionPair> disabled_collision_pairs_;
  std::vector<Model::CollisionPair> enabled_collision_pairs_;
  std::vector<Model::PassiveJoint> passive_joints_;
  std::map<std::string, std::vector<srdf::Model::JointProperty>> joint_properties_;

  // Store the SRDF Model for updating the kinematic_model
  ModelSharedPtr srdf_model_;

  // Robot name
  std::string robot_name_;
};

// ******************************************************************************************
// Typedef
// ******************************************************************************************
typedef std::shared_ptr<SRDFWriter> SRDFWriterPtr;
}  // namespace srdf

#endif
