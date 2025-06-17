// Copyright (c) 2022, PickNik, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
/// \author: Andy Zelenak, Paul Gesel
/// \description: KDL plugin for kinematics interface

#ifndef KINEMATICS_INTERFACE_KDL__KINEMATICS_INTERFACE_KDL_HPP_
#define KINEMATICS_INTERFACE_KDL__KINEMATICS_INTERFACE_KDL_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/LU"
#include "kdl/chainfksolverpos_recursive.hpp"
#include "kdl/chainfksolvervel_recursive.hpp"
#include "kdl/chainjnttojacsolver.hpp"
#include "kdl/treejnttojacsolver.hpp"
#include "kdl_parser/kdl_parser.hpp"
#include "kinematics_interface/kinematics_interface.hpp"
#include "rclcpp/node_interfaces/node_parameters_interface.hpp"
#include "tf2_eigen_kdl/tf2_eigen_kdl.hpp"

namespace kinematics_interface_kdl
{
class KinematicsInterfaceKDL : public kinematics_interface::KinematicsInterface
{
public:
  bool initialize(
    std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface> parameters_interface,
    const std::string & end_effector_name) override;

  bool convert_cartesian_deltas_to_joint_deltas(
    const Eigen::VectorXd & joint_pos, const Eigen::Matrix<double, 6, 1> & delta_x,
    const std::string & link_name, Eigen::VectorXd & delta_theta) override;

  bool convert_joint_deltas_to_cartesian_deltas(
    const Eigen::VectorXd & joint_pos, const Eigen::VectorXd & delta_theta,
    const std::string & link_name, Eigen::Matrix<double, 6, 1> & delta_x) override;

  bool calculate_link_transform(
    const Eigen::VectorXd & joint_pos, const std::string & link_name,
    Eigen::Isometry3d & transform) override;

  bool calculate_jacobian(
    const Eigen::VectorXd & joint_pos, const std::string & link_name,
    Eigen::Matrix<double, 6, Eigen::Dynamic> & jacobian) override;

private:
  // verification methods
  bool verify_initialized();
  bool verify_link_name(const std::string & link_name);
  bool verify_joint_vector(const Eigen::VectorXd & joint_vector);
  bool verify_jacobian(const Eigen::Matrix<double, 6, Eigen::Dynamic> & jacobian);

  bool initialized = false;
  std::string root_name_;
  size_t num_joints_;
  KDL::Chain chain_;
  std::shared_ptr<KDL::ChainFkSolverPos_recursive> fk_pos_solver_;
  KDL::JntArray q_;
  KDL::Frame frame_;
  std::shared_ptr<KDL::Jacobian> jacobian_;
  std::shared_ptr<KDL::ChainJntToJacSolver> jac_solver_;
  std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface> parameters_interface_;
  std::unordered_map<std::string, int> link_name_map_;
  double alpha;  // damping term for Jacobian inverse
  Eigen::MatrixXd I;
};

}  // namespace kinematics_interface_kdl

#endif  // KINEMATICS_INTERFACE_KDL__KINEMATICS_INTERFACE_KDL_HPP_
