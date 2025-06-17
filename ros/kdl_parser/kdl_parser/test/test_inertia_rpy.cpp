/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2015 Open Source Robotics Foundation, Inc.
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
*   * Neither the name of the copyright holder nor the names of its
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

#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "kdl/chainidsolver_recursive_newton_euler.hpp"
#include "kdl/jntarray.hpp"
#include "kdl_parser/kdl_parser.hpp"

int g_argc;
char ** g_argv;

class TestInertiaRPY : public testing::Test
{
public:
  // NOLINT

protected:
  /// constructor
  TestInertiaRPY()
  {
  }

  /// Destructor
  ~TestInertiaRPY()
  {
  }
};


TEST_F(TestInertiaRPY, test_torques) {
  // workaround for segfault issue with parsing 2 trees instantiated on the stack
  KDL::Tree * tree_1 = new KDL::Tree;
  KDL::Tree * tree_2 = new KDL::Tree;
  KDL::JntArray torques_1;
  KDL::JntArray torques_2;

  {
    ASSERT_TRUE(kdl_parser::treeFromFile(g_argv[1], *tree_1));
    KDL::Vector gravity(0, 0, -9.81);
    KDL::Chain chain;
    std::cout << "number of joints: " << tree_1->getNrOfJoints() << std::endl;
    std::cout << "number of segments: " << tree_1->getNrOfSegments() << std::endl;

    ASSERT_TRUE(tree_1->getChain("base_link", "link2", chain));
    KDL::ChainIdSolver_RNE solver(chain, gravity);
    KDL::JntArray q(chain.getNrOfJoints());
    KDL::JntArray qdot(chain.getNrOfJoints());
    KDL::JntArray qdotdot(chain.getNrOfJoints());
    std::vector<KDL::Wrench> wrenches(chain.getNrOfJoints());
    solver.CartToJnt(q, qdot, qdotdot, wrenches, torques_1);

    delete tree_1;
    tree_1 = NULL;
  }

  {
    ASSERT_TRUE(kdl_parser::treeFromFile(g_argv[2], *tree_2));
    KDL::Vector gravity(0, 0, -9.81);
    KDL::Chain chain;

    ASSERT_TRUE(tree_2->getChain("base_link", "link2", chain));
    KDL::ChainIdSolver_RNE solver(chain, gravity);
    KDL::JntArray q(chain.getNrOfJoints());
    KDL::JntArray qdot(chain.getNrOfJoints());
    KDL::JntArray qdotdot(chain.getNrOfJoints());
    std::vector<KDL::Wrench> wrenches(chain.getNrOfJoints());
    solver.CartToJnt(q, qdot, qdotdot, wrenches, torques_2);

    delete tree_2;
    tree_2 = NULL;
  }

  ASSERT_TRUE(torques_1 == torques_2);

  SUCCEED();
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  for (size_t i = 0; i < argc; ++i) {
    std::cout << argv[i] << std::endl;
  }
  g_argc = argc;
  g_argv = argv;
  return RUN_ALL_TESTS();
}
