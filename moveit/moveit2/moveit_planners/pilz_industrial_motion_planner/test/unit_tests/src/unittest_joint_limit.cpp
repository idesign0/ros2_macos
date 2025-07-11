/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2018 Pilz GmbH & Co. KG
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
 *   * Neither the name of Pilz GmbH & Co. KG nor the names of its
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

#include <rclcpp/rclcpp.hpp>

#include <gtest/gtest.h>

#include <pilz_industrial_motion_planner/joint_limits_interface_extension.h>

using namespace pilz_industrial_motion_planner;
using namespace pilz_industrial_motion_planner::joint_limits_interface;

namespace pilz_extensions_tests
{
class JointLimitTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    node_ = rclcpp::Node::make_shared("unittest_joint_limits_extended");
  }
  rclcpp::Node::SharedPtr node_;
};

TEST_F(JointLimitTest, SimpleRead)
{
  // Joints limits interface
  JointLimits joint_limits_extended;

  EXPECT_TRUE(declareParameters("joint_1", "", node_));
  EXPECT_TRUE(getJointLimits("joint_1", "", node_, joint_limits_extended));

  EXPECT_EQ(1, joint_limits_extended.max_acceleration);
  EXPECT_EQ(-1, joint_limits_extended.max_deceleration);
}

TEST_F(JointLimitTest, readNonExistingJointLimit)
{
  // Joints limits interface
  JointLimits joint_limits_extended;

  EXPECT_FALSE(getJointLimits("anything", "", node_, joint_limits_extended));
}

/**
 * @brief Test reading a joint limit representing an invalid parameter key
 *
 * For full coverage.
 */
TEST_F(JointLimitTest, readInvalidParameterName)
{
  // Joints limits interface
  JointLimits joint_limits_extended;

  EXPECT_FALSE(getJointLimits("~anything", "", node_, joint_limits_extended));
}

TEST_F(JointLimitTest, OldRead)
{
  // Joints limits interface
  JointLimits joint_limits;

  EXPECT_TRUE(declareParameters("joint_1", "", node_));
  EXPECT_TRUE(getJointLimits("joint_1", "", node_, joint_limits));

  EXPECT_EQ(1, joint_limits.max_acceleration);
}
}  // namespace pilz_extensions_tests

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
