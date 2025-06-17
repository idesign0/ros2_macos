#pragma once

#include <array>
#include <cassert>
#include <string>
#include <string_view>

namespace rsl {

/** @file */

/**
 * @brief Fixed capacity string with an implicit conversion to std::string_view. Capacity is
 * specified as a template parameter. At runtime one may use up to the specified capacity.
 */
template <size_t capacity>
class StaticString {
    std::array<std::string::value_type, capacity> data_{};
    size_t size_{};

   public:
    /**
     * @brief Construct an empty string
     */
    StaticString() = default;

    /**
     * @brief Construct from a std::string
     */
    StaticString(std::string const& string) : size_(std::min(string.size(), capacity)) {
        assert(string.size() <= capacity &&
               "rsl::StaticString::StaticString: Input exceeds capacity");
        std::copy(string.cbegin(), string.cbegin() + std::string::difference_type(size_),
                  data_.begin());
    }
    /**
     * @brief Get a const begin iterator
     */
    [[nodiscard]] auto begin() const { return data_.cbegin(); }

    /**
     * @brief Get a const end iterator
     */
    [[nodiscard]] auto end() const { return data_.cbegin() + size_; }

    /**
     * @brief Implicit conversion to std::string_view
     */
    operator std::string_view() const { return std::string_view(data_.data(), size_); }
};

/**
 * @brief Explicit conversion to std::string
 */
template <size_t capacity>
[[nodiscard]] auto to_string(StaticString<capacity> const& static_string) {
    return std::string(static_string);
}

}  // namespace rsl
