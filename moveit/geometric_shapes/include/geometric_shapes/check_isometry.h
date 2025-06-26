// Copyright 2020 Martin Pecka
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
//    * Neither the name of the Martin Pecka nor the names of its
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

/* Author: Martin Pecka, Robert Haschke */

#ifndef GEOMETRIC_SHAPES_CHECK_ISOMETRY_H
#define GEOMETRIC_SHAPES_CHECK_ISOMETRY_H

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

/** \brief This file provides functions and macros that can be used to verify that an Eigen::Isometry3d is really an
 * isometry. Eigen itself doesn't do the checks because they're expensive. If the isometry object is constructed in
 * a wrong way (e.g. from an AngleAxisd with non-unit axis), it can represent a non-isometry. But some methods in the
 * Isometry3d class perform isometry-specific operations which return wrong results when called on a non-isometry. E.g.
 * for isometries, .linear() and .rotation() should be the same, but for non-isometries, the result of .linear()
 * contains also the scaling factor, whether .rotation() is only the rotation part.
 *
 * These checks are primarily meant to be performed only in debug mode (via the ASSERT_ISOMETRY macro), but you can call
 * checkIsometry() even in release mode. This check should be mainly performed on transforms input by the user.
 */

#ifndef CHECK_ISOMETRY_PRECISION
/** \brief The default precision to which the transform has to correspond to an isometry. */
#define CHECK_ISOMETRY_PRECISION Eigen::NumTraits<double>::dummy_precision()
#endif

/**
 * \brief Check whether the given transform is really (mathematically) an isometry.
 * \param transform The possibly non-isometric transform.
 * \param precision Precision to which the transform has to correspond to an isometry (element-wise).
 * \param printError Whether an error should be printed to std::cerr with details about the false isometry.
 * \return Whether the given transform is close to an isometry or not.
 */
inline bool checkIsometry(const Eigen::Isometry3d& transform, const double precision = CHECK_ISOMETRY_PRECISION,
                          const bool printError = true)
{
  if (!transform.matrix().row(3).isApprox(Eigen::Vector4d::UnitW().transpose(), precision))
  {
    if (printError)
    {
      std::cerr << "The given transform is not an isometry! Its last row deviates from [0 0 0 1] by ["
                << (transform.matrix().row(3) - Eigen::Vector4d::UnitW().transpose())
                << "] but the required precision is " << precision << "." << std::endl;
    }
    return false;
  }

  Eigen::Isometry3d::LinearMatrixType scale;
  transform.computeRotationScaling((Eigen::Isometry3d::LinearMatrixType*)nullptr, &scale);
  if (!scale.isApprox(Eigen::Matrix3d::Identity(), precision))
  {
    if (printError)
    {
      std::cerr << "The given transform is not an isometry! Its linear part involves non-unit scaling. The scaling "
                   "matrix diagonal differs from [1 1 1] by ["
                << (scale.diagonal().transpose() - Eigen::Vector3d::Ones().transpose())
                << "] but the required precision is " << precision << "." << std::endl;
    }
    return false;
  }

  return true;
}

// To be able to use the more efficient Eigen::Transform::linear() instead of rotation(),
// we need to check the user has really passed an isometry. To avoid runtime costs,
// this check is only done as assert, which gets compiled-out in release builds
#ifdef NDEBUG
/** \brief Assert that the given transform is an isometry (no-op in release mode). */
#define ASSERT_ISOMETRY(transform) (void)sizeof(transform);  // this is a no-op, but prevents unused variable warnings
#else
/** \brief Assert that the given transform is an isometry. */
#define ASSERT_ISOMETRY(transform)                                                                                     \
  {                                                                                                                    \
    if (!::checkIsometry(transform, CHECK_ISOMETRY_PRECISION))                                                         \
      assert(!"Invalid isometry transform");                                                                           \
  }
#endif

#endif  // GEOMETRIC_SHAPES_CHECK_ISOMETRY_H
