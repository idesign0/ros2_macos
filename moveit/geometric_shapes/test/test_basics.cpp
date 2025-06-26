// Copyright 2019 Bielefeld University
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Bielefeld University nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <geometric_shapes/check_isometry.h>
#include <gtest/gtest.h>

TEST(Utils, checkIsometry)
{
  Eigen::Isometry3d t(Eigen::AngleAxisd(0.42, Eigen::Vector3d(1, 1, 1).normalized()));
  ASSERT_TRUE(::checkIsometry(t));

  const Eigen::Vector3d oldDiagonal = t.linear().diagonal();
  t.linear() = t.linear() * Eigen::DiagonalMatrix<double, 3, 3>(1.0, 2.0, 3.0);
  ASSERT_FALSE(::checkIsometry(t));

  t.matrix().row(3) = Eigen::Vector4d(1e-6, 1e-6, 1e-6, 1 - 1e-6);
  ASSERT_FALSE(::checkIsometry(t, CHECK_ISOMETRY_PRECISION));

  t.linear().diagonal() = oldDiagonal;
  ASSERT_TRUE(::checkIsometry(t, 1e-0, false));
  ASSERT_FALSE(::checkIsometry(t, 1e-1));
  ASSERT_FALSE(::checkIsometry(t, 1e-2, false));

  std::cerr.flush();
}

TEST(Utils, assertIsometry)
{
  Eigen::Isometry3d t(Eigen::AngleAxisd(0.42, Eigen::Vector3d(1, 1, 1).normalized()));
  ASSERT_ISOMETRY(t);

  t.linear() = t.linear() * Eigen::DiagonalMatrix<double, 3, 3>(1.0, 2.0, 3.0);
#ifdef NDEBUG
  ASSERT_ISOMETRY(t)  // noop in release mode
#else
  ASSERT_DEATH(ASSERT_ISOMETRY(t), "Invalid isometry transform");
#endif

  t.matrix().row(3) = Eigen::Vector4d(1e-6, 1e-6, 1e-6, 1 - 1e-6);
#ifdef NDEBUG
  ASSERT_ISOMETRY(t)  // noop in release mode
#else
  ASSERT_DEATH(ASSERT_ISOMETRY(t), "Invalid isometry transform");
#endif

  std::cerr.flush();
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
