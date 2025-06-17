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

#ifndef KINEMATICS_INTERFACE__KINEMATICS_INTERFACE_HPP_
#define KINEMATICS_INTERFACE__KINEMATICS_INTERFACE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "Eigen/LU"
#include "rclcpp/logging.hpp"
#include "rclcpp/node_interfaces/node_parameters_interface.hpp"

namespace kinematics_interface
{
class KinematicsInterface
{
public:
  KinematicsInterface() = default;

  virtual ~KinematicsInterface() = default;

  /**
   * \brief Initialize plugin. This method must be called before any other.
   */
  virtual bool initialize(
    std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface> parameters_interface,
    const std::string & end_effector_name) = 0;

  /**
   * \brief Convert Cartesian delta-x to joint delta-theta, using the Jacobian.
   * \param[in] joint_pos joint positions of the robot in radians
   * \param[in] delta_x input Cartesian deltas (x, y, z, wx, wy, wz)
   * \param[in] link_name the link name at which delta_x is applied
   * \param[out] delta_theta outputs joint deltas
   * \return true if successful
   */
  virtual bool convert_cartesian_deltas_to_joint_deltas(
    const Eigen::VectorXd & joint_pos, const Eigen::Matrix<double, 6, 1> & delta_x,
    const std::string & link_name, Eigen::VectorXd & delta_theta) = 0;

  /**
   * \brief Convert joint delta-theta to Cartesian delta-x.
   * \param joint_pos joint positions of the robot in radians
   * \param[in] delta_theta joint deltas
   * \param[in] link_name the link name at which delta_x is calculated
   * \param[out] delta_x  Cartesian deltas (x, y, z, wx, wy, wz)
   * \return true if successful
   */
  virtual bool convert_joint_deltas_to_cartesian_deltas(
    const Eigen::VectorXd & joint_pos, const Eigen::VectorXd & delta_theta,
    const std::string & link_name, Eigen::Matrix<double, 6, 1> & delta_x) = 0;

  /**
   * \brief Calculates the joint transform for a specified link using provided joint positions.
   * \param[in] joint_pos joint positions of the robot in radians
   * \param[in] link_name the name of the link to find the transform for
   * \param[out] transform transformation matrix of the specified link
   * \return true if successful
   */
  virtual bool calculate_link_transform(
    const Eigen::VectorXd & joint_pos, const std::string & link_name,
    Eigen::Isometry3d & transform) = 0;

  /**
   * \brief Calculates the jacobian for a specified link using provided joint positions.
   * \param[in] joint_pos joint positions of the robot in radians
   * \param[in] link_name the name of the link to find the transform for
   * \param[out] jacobian Jacobian matrix of the specified link in column major format.
   * \return true if successful
   */
  virtual bool calculate_jacobian(
    const Eigen::VectorXd & joint_pos, const std::string & link_name,
    Eigen::Matrix<double, 6, Eigen::Dynamic> & jacobian) = 0;

  bool convert_cartesian_deltas_to_joint_deltas(
    std::vector<double> & joint_pos_vec, const std::vector<double> & delta_x_vec,
    const std::string & link_name, std::vector<double> & delta_theta_vec);

  bool convert_joint_deltas_to_cartesian_deltas(
    const std::vector<double> & joint_pos_vec, const std::vector<double> & delta_theta_vec,
    const std::string & link_name, std::vector<double> & delta_x_vec);

  bool calculate_link_transform(
    const std::vector<double> & joint_pos_vec, const std::string & link_name,
    Eigen::Isometry3d & transform);

  bool calculate_jacobian(
    const std::vector<double> & joint_pos_vec, const std::string & link_name,
    Eigen::Matrix<double, 6, Eigen::Dynamic> & jacobian);
};

}  // namespace kinematics_interface

#endif  // KINEMATICS_INTERFACE__KINEMATICS_INTERFACE_HPP_
