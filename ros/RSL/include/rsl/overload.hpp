#pragma once

namespace rsl {

/** @file */

/**
 * @brief Class template for creating overloads sets to use with std::visit
 *
 * @tparam Ts Types
 */
template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Overload(Ts...) -> Overload<Ts...>;

}  // namespace rsl
