#pragma once

#include <utility>

namespace rsl {

/** @file */

/**
 * @brief Class template for creating strong type aliases
 *
 * @tparam T value type
 * @tparam Tag Tag type to disambiguate separate type aliases
 */
template <typename T, typename Tag>
class StrongType {
    T value_;

   public:
    /**
     * @brief Construct from any type
     */
    constexpr explicit StrongType(T value) : value_(std::move(value)) {}

    /**
     * @brief Get non-const reference to underlying value
     */
    [[nodiscard]] constexpr T& get() { return value_; }

    /**
     * @brief Get const reference to underlying value
     */
    [[nodiscard]] constexpr const T& get() const { return value_; }

    /**
     * @brief Explicit conversion to underlying type
     */
    [[nodiscard]] constexpr explicit operator T() const { return value_; }
};

}  // namespace rsl
