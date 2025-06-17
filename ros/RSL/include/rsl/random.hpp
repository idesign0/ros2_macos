#pragma once

#include <rsl/export.hpp>

#include <Eigen/Geometry>
#include <cassert>
#include <random>
#include <type_traits>

namespace rsl {

/** @file */

/**
 * @brief Get a random number generator
 *
 * The first time this function is called it creates a thread_local random number generator. If a
 * seed sequence is provided on that first call it is used to create the generator, otherwise the
 * random device is used to seed the generator. After the first call to this function, if a seed
 * sequence is provided this function throws. The returned value is a reference to a thread_local
 * static generator.
 *
 * @param seed_sequence Seed sequence for random number generator
 *
 * @return Seeded random number generator
 */
RSL_EXPORT auto rng(std::seed_seq seed_sequence = {}) -> std::mt19937&;

/**
 * @brief Get a uniform real number in a given range
 *
 * @param lower Lower bound, inclusive
 * @param upper Upper bound, exclusive
 *
 * @tparam RealType Floating point type
 *
 * @return Uniform real in range [lower, upper)
 */
template <typename RealType>
[[nodiscard]] auto uniform_real(RealType lower, RealType upper) {
    static_assert(std::is_floating_point_v<RealType>, "RealType must be a floating point type");
    assert(lower < upper && "rsl::uniform_real: Lower bound be less than upper bound");
    return std::uniform_real_distribution(lower, upper)(rng());
}

/**
 * @brief Get a uniform integer number in a given range
 *
 * @param lower Lower bound, inclusive
 * @param upper Upper bound, inclusive
 *
 * @tparam IntType Integral type
 *
 * @return Uniform integer in range [lower, upper]
 */
template <typename IntType>
[[nodiscard]] auto uniform_int(IntType lower, IntType upper) {
    static_assert(std::is_integral_v<IntType>, "IntType must be an integral type");
    assert(lower <= upper &&
           "rsl::uniform_int: Lower bound must be less than or equal to upper bound");
    return std::uniform_int_distribution(lower, upper)(rng());
}

/**
 * @brief Generate a random unit quaternion of doubles
 * @return Random unit quaternion
 */
[[nodiscard]] RSL_EXPORT auto random_unit_quaternion() -> Eigen::Quaterniond;

}  // namespace rsl
