#pragma once

#include <tcb_span/span.hpp>

#include <array>
#include <cassert>
#include <vector>

namespace rsl {

/** @file */

/**
 * @brief Fixed capacity vector with an implicit conversion to tcb::span. Capacity is specified as
 * a template parameter. At runtime one may use up to the specified capacity.
 */
template <typename T, size_t capacity>
class StaticVector {
    std::array<T, capacity> data_{};
    size_t size_{};

   public:
    /**
     * @brief Construct an empty vector
     */
    StaticVector() = default;

    /**
     * @brief Construct from another container
     */
    template <typename Collection>
    StaticVector(Collection const& collection) : size_(std::min(collection.size(), capacity)) {
        assert(collection.size() <= capacity &&
               "rsl::StaticVector::StaticVector: Input exceeds capacity");
        std::copy(collection.cbegin(),
                  collection.cbegin() + typename Collection::difference_type(size_), data_.begin());
    }

    /**
     * @brief Construct from a std::initializer_list
     */
    StaticVector(std::initializer_list<T> const& collection)
        : StaticVector(std::vector<T>(collection)) {}

    /**
     * @brief Get a mutable begin iterator
     */
    [[nodiscard]] auto begin() { return data_.begin(); }

    /**
     * @brief Get a const begin iterator
     */
    [[nodiscard]] auto begin() const { return data_.cbegin(); }

    /**
     * @brief Get a mutable end iterator
     */
    [[nodiscard]] auto end() { return data_.begin() + size_; }

    /**
     * @brief Get a const end iterator
     */
    [[nodiscard]] auto end() const { return data_.cbegin() + size_; }

    /**
     * @brief Implicit conversion to tcb::span<T>
     */
    operator tcb::span<T>() { return tcb::span<T>(data_.data(), size_); }

    /**
     * @brief Implicit conversion to tcb::span<T const>
     */
    operator tcb::span<T const>() const { return tcb::span<T const>(data_.data(), size_); }
};

/**
 * @brief Explicit conversion to std::vector<T>
 */
template <typename T, size_t capacity>
[[nodiscard]] auto to_vector(StaticVector<T, capacity> const& static_vector) {
    return std::vector<T>(static_vector.begin(), static_vector.end());
}

}  // namespace rsl
