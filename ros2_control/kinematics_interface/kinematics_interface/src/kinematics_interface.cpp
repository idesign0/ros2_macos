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
/// \description: Base class for kinematics interface

#include "kinematics_interface/kinematics_interface.hpp"

namespace kinematics_interface
{

rclcpp::Logger LOGGER = rclcpp::get_logger("kinematics_interface");

bool KinematicsInterface::convert_cartesian_deltas_to_joint_deltas(
  std::vector<double> & joint_pos_vec, const std::vector<double> & delta_x_vec,
  const std::string & link_name, std::vector<double> & delta_theta_vec)
{
  auto joint_pos = Eigen::Map<const Eigen::VectorXd>(joint_pos_vec.data(), joint_pos_vec.size());
  auto delta_x = Eigen::Map<const Eigen::VectorXd>(delta_x_vec.data(), delta_x_vec.size());
  // TODO(anyone): heap allocation should be removed for realtime use
  Eigen::VectorXd delta_theta =
    Eigen::Map<Eigen::VectorXd>(delta_theta_vec.data(), delta_theta_vec.size());

  bool ret = convert_cartesian_deltas_to_joint_deltas(joint_pos, delta_x, link_name, delta_theta);
  for (auto i = 0ul; i < delta_theta_vec.size(); i++)
  {
    delta_theta_vec[i] = delta_theta[i];
  }
  return ret;
}

bool KinematicsInterface::convert_joint_deltas_to_cartesian_deltas(
  const std::vector<double> & joint_pos_vec, const std::vector<double> & delta_theta_vec,
  const std::string & link_name, std::vector<double> & delta_x_vec)
{
  auto joint_pos = Eigen::Map<const Eigen::VectorXd>(joint_pos_vec.data(), joint_pos_vec.size());
  Eigen::VectorXd delta_theta =
    Eigen::Map<const Eigen::VectorXd>(delta_theta_vec.data(), delta_theta_vec.size());
  if (delta_x_vec.size() != 6)
  {
    RCLCPP_ERROR(
      LOGGER, "The length of the cartesian delta vector (%zu) must be 6.", delta_x_vec.size());
    return false;
  }
  Eigen::Matrix<double, 6, 1> delta_x(delta_x_vec.data());
  bool ret = convert_joint_deltas_to_cartesian_deltas(joint_pos, delta_theta, link_name, delta_x);
  for (auto i = 0ul; i < delta_x_vec.size(); i++)
  {
    delta_x_vec[i] = delta_x[i];
  }
  return ret;
}

bool KinematicsInterface::calculate_link_transform(
  const std::vector<double> & joint_pos_vec, const std::string & link_name,
  Eigen::Isometry3d & transform)
{
  auto joint_pos = Eigen::Map<const Eigen::VectorXd>(joint_pos_vec.data(), joint_pos_vec.size());

  return calculate_link_transform(joint_pos, link_name, transform);
}

bool KinematicsInterface::calculate_jacobian(
  const std::vector<double> & joint_pos_vec, const std::string & link_name,
  Eigen::Matrix<double, 6, Eigen::Dynamic> & jacobian)
{
  auto joint_pos = Eigen::Map<const Eigen::VectorXd>(joint_pos_vec.data(), joint_pos_vec.size());

  return calculate_jacobian(joint_pos, link_name, jacobian);
}

}  // namespace kinematics_interface
