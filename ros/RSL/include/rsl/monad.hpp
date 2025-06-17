#pragma once

#include <tl_expected/expected.hpp>

#include <optional>

namespace rsl {

/** @file */

/**
 * @brief Monad optional bind
 *
 * @param opt Input optional
 * @param fn  Function, must return a optional
 *
 * @tparam T  Input type
 * @tparam Fn Function
 *
 * @return Return type of fn
 */
template <typename T, typename Fn>
[[nodiscard]] constexpr auto mbind(std::optional<T> const& opt, Fn fn)
    -> std::invoke_result_t<Fn, T> {
    static_assert(std::is_convertible_v<std::nullopt_t, std::invoke_result_t<Fn, T>>,
                  "Fn must return a std::optional");
    if (opt) return fn(opt.value());
    return std::invoke_result_t<Fn, T>{std::nullopt};
}

/**
 * @brief Monad tl::expected<T,E>
 *
 * @param exp tl::expected<T,E> input
 * @param fn  Function to apply
 *
 * @tparam T  Type for the input expected
 * @tparam E  Error type
 * @tparam Fn Function
 *
 * @return Return type of the function
 */
template <typename T, typename E, typename Fn>
[[nodiscard]] constexpr auto mbind(tl::expected<T, E> const& exp, Fn fn)
    -> std::invoke_result_t<Fn, T> {
    if (exp) return fn(exp.value());
    return tl::unexpected(exp.error());
}

/**
 * @brief Monadic try, used to lift a function that throws an exception into one that returns an
 * tl::expected<T, std::exception_ptr>
 *
 * @param fn Function to call
 *
 * @tparam Fn Function type
 *
 * @return Return value of the function
 */
template <typename Fn>
[[nodiscard]] auto mtry(Fn fn) -> tl::expected<std::invoke_result_t<Fn>, std::exception_ptr> try {
    return fn();
} catch (...) {
    return tl::unexpected(std::current_exception());
}

/**
 * @brief Monadic compose two monad functions
 *
 * @param fn First function
 * @param g  Second function
 *
 * @tparam Fn Type of the first function
 * @tparam G  Type of the second function
 *
 * @return A functional composition of two monad functions
 */
template <typename Fn, typename G>
[[nodiscard]] constexpr auto mcompose(Fn fn, G g) {
    return [=](auto value) { return mbind(fn(value), g); };
}

/**
 * @brief Variadic mcompose
 *
 * @param t    First function
 * @param g    Second function
 * @param vars Rest of the functions
 *
 * @tparam T  Type of the first function
 * @tparam G  Type of the second function
 * @tparam Ts Types of the rest of the functions
 *
 * @return A functional composition of variadic monad functions
 */
template <typename T, typename G, typename... Ts>
[[nodiscard]] constexpr auto mcompose(T t, G g, Ts... vars) {
    auto exp = mcompose(t, g);
    return mcompose(exp, vars...);
}

/**
 * @brief Test if expected type is Error
 *
 * @param exp Input tl::expected<T,E> value
 *
 * @return True if expected paraemter is Error
 */
template <typename T, typename E>
[[nodiscard]] constexpr auto has_error(tl::expected<T, E> const& exp) {
    return !exp.has_value();
}

/**
 * @brief Test if expected type is Value
 *
 * @param exp Input tl::expected<T,E> value
 *
 * @return True if expected paraemter is Value
 */
template <typename T, typename E>
[[nodiscard]] constexpr auto has_value(tl::expected<T, E> const& exp) {
    return exp.has_value();
}

/**
 * @brief Tests if any of the expected args passed in has an error.
 *
 * @param args tl::expected<T, E> parameter pack
 *
 * @tparam E    The error type
 * @tparam Args The value types for the tl::expected<T, E> args
 *
 * @return The first error found or nothing
 */
template <typename E, typename... Args>
[[nodiscard]] constexpr auto maybe_error(tl::expected<Args, E>... args) {
    auto maybe = std::optional<E>();
    (
        [&](auto& exp) {
            if (maybe.has_value()) return;
            if (has_error(exp)) maybe = exp.error();
        }(args),
        ...);
    return maybe;
}

template <typename>
constexpr inline bool is_optional_impl = false;
template <typename T>
constexpr inline bool is_optional_impl<std::optional<T>> = true;
template <typename T>
constexpr inline bool is_optional = is_optional_impl<std::remove_cv_t<std::remove_reference_t<T>>>;

}  // namespace rsl

/**
 * @brief Overload of the | operator as bind
 *
 * @param opt Input optional
 * @param fn  Function
 *
 * @tparam T  Input type
 * @tparam Fn Function
 *
 * @return Return type of f
 */
template <typename T, typename Fn, typename = std::enable_if_t<rsl::is_optional<T>>,
          typename = std::enable_if_t<std::is_invocable_v<
              Fn, typename std::remove_cv_t<std::remove_reference_t<T>>::value_type>>>
[[nodiscard]] constexpr auto operator|(T&& opt, Fn&& fn) {
    return rsl::mbind(std::forward<T>(opt), std::forward<Fn>(fn));
}

/**
 * @brief Overload of the | operator as bind
 *
 * @param exp Input tl::expected<T,E> value
 * @param fn  Function to apply
 *
 * @tparam T  Type for the input expected
 * @tparam E  Error type
 * @tparam Fn Function
 *
 * @return Return type of fn
 */
template <typename T, typename E, typename Fn>
[[nodiscard]] constexpr auto operator|(tl::expected<T, E> const& exp, Fn fn) {
    return rsl::mbind(exp, fn);
}

/**
 * @brief Overload of the | operator for unary functions
 *
 * @param val Input value
 * @param fn  Function to apply on val
 *
 * @tparam T  Type for the input
 * @tparam Fn Function
 *
 * @return Return the result of invoking the function on val
 */
template <typename T, typename Fn, typename = std::enable_if_t<!rsl::is_optional<T>>>
[[nodiscard]] constexpr auto operator|(T&& val, Fn&& fn) ->
    typename std::enable_if_t<std::is_invocable_v<Fn, T>, std::invoke_result_t<Fn, T>> {
    return std::invoke(std::forward<Fn>(fn), std::forward<T>(val));
}
