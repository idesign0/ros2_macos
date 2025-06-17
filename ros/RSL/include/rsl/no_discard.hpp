#pragma once

#include <utility>

namespace rsl {

/** @file */

/**
 * @brief Template for creating lambdas with the nodiscard attribute
 *
 * @tparam Fn  Lambda
 */
template <typename Fn>
class NoDiscard {
    static_assert(std::is_invocable_v<Fn()>, "Fn must be invocable");
    Fn fn_;

   public:
    NoDiscard(Fn const& fn) : fn_(fn) {}
    template <typename... Ts>
    [[nodiscard]] constexpr auto operator()(Ts&&... args) const
        noexcept(noexcept(fn_(std::forward<Ts>(args)...))) {
        return fn_(std::forward<Ts>(args)...);
    }
};

}  // namespace rsl
