#pragma once

#include <rsl/algorithm.hpp>
#include <rsl/export.hpp>
#include <rsl/static_string.hpp>
#include <rsl/static_vector.hpp>

#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <rclcpp/parameter.hpp>
#include <tl_expected/expected.hpp>

#include <fmt/ranges.h>

namespace rsl {

/** @file */

/**
 * @cond DETAIL
 */
namespace detail {
template <typename T, typename Fn>
[[nodiscard]] auto size_compare(rclcpp::Parameter const& parameter, size_t const size,
                                std::string const& predicate_description, Fn const& predicate)
    -> tl::expected<void, std::string> {
    static constexpr auto format_string = "Length of parameter '{}' is '{}' but must be {} '{}'";
    switch (parameter.get_type()) {
        case rclcpp::ParameterType::PARAMETER_STRING:
            if (auto value = parameter.get_value<std::string>(); !predicate(value.size(), size))
                return tl::unexpected(fmt::format(format_string, parameter.get_name(), value.size(),
                                                  predicate_description, size));
            break;
        default:
            if (auto value = parameter.get_value<std::vector<T>>(); !predicate(value.size(), size))
                return tl::unexpected(fmt::format(format_string, parameter.get_name(), value.size(),
                                                  predicate_description, size));
    }
    return {};
}

template <typename T, typename Fn>
[[nodiscard]] auto compare(rclcpp::Parameter const& parameter, T const& value,
                           std::string const& predicate_description, Fn const& predicate)
    -> tl::expected<void, std::string> {
    if (auto const param_value = parameter.get_value<T>(); !predicate(param_value, value))
        return tl::unexpected(fmt::format("Parameter '{}' with the value '{}' must be {} '{}'",
                                          parameter.get_name(), param_value, predicate_description,
                                          value));
    return {};
}
}  // namespace detail
/**
 * @endcond
 */

/**
 * @brief Is every element of rclcpp::Parameter unique?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto unique(rclcpp::Parameter const& parameter) -> tl::expected<void, std::string> {
    if (is_unique(parameter.get_value<std::vector<T>>())) return {};
    return tl::unexpected(
        fmt::format("Parameter '{}' must only contain unique values", parameter.get_name()));
}

/**
 * @brief Are the values in parameter a subset of the valid values?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto subset_of(rclcpp::Parameter const& parameter, std::vector<T> const& valid_values)
    -> tl::expected<void, std::string> {
    auto const& values = parameter.get_value<std::vector<T>>();
    for (auto const& value : values)
        if (!contains(valid_values, value))
            return tl::unexpected(
                fmt::format("Entry '{}' in parameter '{}' is not in the set '{{{}}}'", value,
                            parameter.get_name(), fmt::join(valid_values, ", ")));
    return {};
}

/**
 * @brief Is the array size of parameter equal to passed in size?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto fixed_size(rclcpp::Parameter const& parameter, size_t const size) {
    return detail::size_compare<T>(parameter, size, "equal to", std::equal_to<>());
}

/**
 * @brief Is the array size of parameter greater than passed in size?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto size_gt(rclcpp::Parameter const& parameter, size_t const size) {
    return detail::size_compare<T>(parameter, size, "greater than", std::greater<>());
}

/**
 * @brief Is the array size of parameter less than passed in size?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto size_lt(rclcpp::Parameter const& parameter, size_t const size) {
    return detail::size_compare<T>(parameter, size, "less than", std::less<>());
}

/**
 * @brief Is the size of the value passed in not zero?
 * @pre rclcpp::Parameter must be an array type or a string
 * @tparam T Interior type of array or std::string; e.g. for parameter type double_array, T
 * = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto not_empty(rclcpp::Parameter const& parameter)
    -> tl::expected<void, std::string> {
    switch (parameter.get_type()) {
        case rclcpp::ParameterType::PARAMETER_STRING:
            if (auto param_value = parameter.get_value<std::string>(); param_value.empty())
                return tl::unexpected(
                    fmt::format("Parameter '{}' cannot be empty", parameter.get_name()));
            break;
        default:
            if (auto param_value = parameter.get_value<std::vector<T>>(); param_value.empty())
                return tl::unexpected(
                    fmt::format("Parameter '{}' cannot be empty", parameter.get_name()));
    }
    return {};
}

/**
 * @brief Are all elements of parameter within the bounds (inclusive)?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto element_bounds(rclcpp::Parameter const& parameter, T const& lower,
                                  T const& upper) -> tl::expected<void, std::string> {
    auto const& param_value = parameter.get_value<std::vector<T>>();
    for (auto val : param_value)
        if (val < lower || val > upper)
            return tl::unexpected(
                fmt::format("Value '{}' in parameter '{}' must be within bounds '[{}, {}]'", val,
                            parameter.get_name(), lower, upper));
    return {};
}

/**
 * @brief Are all elements of parameter greater than lower bound?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto lower_element_bounds(rclcpp::Parameter const& parameter, T const& lower)
    -> tl::expected<void, std::string> {
    auto const& param_value = parameter.get_value<std::vector<T>>();
    for (auto val : param_value)
        if (val < lower)
            return tl::unexpected(
                fmt::format("Value '{}' in parameter '{}' must be above lower bound of '{}'", val,
                            parameter.get_name(), lower));
    return {};
}

/**
 * @brief Are all elements of parameter less than some upper bound?
 * @pre rclcpp::Parameter must be an array type
 * @tparam T Interior type of array; e.g. for parameter type double_array, T = double
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto upper_element_bounds(rclcpp::Parameter const& parameter, T const& upper)
    -> tl::expected<void, std::string> {
    auto const& param_value = parameter.get_value<std::vector<T>>();
    for (auto val : param_value)
        if (val > upper)
            return tl::unexpected(
                fmt::format("Value '{}' in parameter '{}' must be below upper bound of '{}'", val,
                            parameter.get_name(), upper));
    return {};
}

/**
 * @brief Is parameter within bounds (inclusive)?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto bounds(rclcpp::Parameter const& parameter, T const& lower, T const& upper)
    -> tl::expected<void, std::string> {
    auto const& param_value = parameter.get_value<T>();
    if (param_value < lower || param_value > upper)
        return tl::unexpected(
            fmt::format("Parameter '{}' with the value '{}' must be within bounds '[{}, {}]'",
                        parameter.get_name(), param_value, lower, upper));
    return {};
}

/**
 * @brief Is parameter within some lower bound (same as gt_eq)?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[deprecated("Replace with rsl::gt_eq"), nodiscard]] auto lower_bounds(
    rclcpp::Parameter const& parameter, T const& value) {
    return detail::compare(parameter, value, "above lower bound of", std::greater_equal<T>());
}

/**
 * @brief Is parameter within some upper bound (same as lt_eq)?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[deprecated("Replace with rsl::lt_eq"), nodiscard]] auto upper_bounds(
    rclcpp::Parameter const& parameter, T const& value) {
    return detail::compare(parameter, value, "below upper bound of", std::less_equal<T>());
}

/**
 * @brief Is parameter less than some value?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto lt(rclcpp::Parameter const& parameter, T const& value) {
    return detail::compare(parameter, value, "less than", std::less<T>());
}

/**
 * @brief Is parameter greater than some value?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto gt(rclcpp::Parameter const& parameter, T const& value) {
    return detail::compare(parameter, value, "greater than", std::greater<T>());
}

/**
 * @brief Is parameter less than or equal to some value?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto lt_eq(rclcpp::Parameter const& parameter, T const& value) {
    return detail::compare(parameter, value, "less than or equal to", std::less_equal<T>());
}

/**
 * @brief Is parameter greater than or equal to some value?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto gt_eq(rclcpp::Parameter const& parameter, T const& value) {
    return detail::compare(parameter, value, "greater than or equal to", std::greater_equal<T>());
}

/**
 * @brief Is the parameter value one of a set of values?
 * @pre rclcpp::Parameter must be a non-array type
 * @tparam T Interior type of parameter; e.g. for parameter type int, T = int64_t
 * @return Help string if the parameter is invalid, otherwise void
 */
template <typename T>
[[nodiscard]] auto one_of(rclcpp::Parameter const& parameter, std::vector<T> const& collection)
    -> tl::expected<void, std::string> {
    auto const& param_value = parameter.get_value<T>();
    if (contains(collection, param_value)) return {};
    return tl::unexpected(fmt::format(
        "Parameter '{}' with the value '{}' is not in the set '{{{}}}'", parameter.get_name(),
        param_value, fmt::format("{}", fmt::join(collection, ", "))));
}

/**
 * @brief Convert the result of a validator function into a SetParametersResult msg
 */
[[nodiscard]] RSL_EXPORT auto to_parameter_result_msg(tl::expected<void, std::string> const& result)
    -> rcl_interfaces::msg::SetParametersResult;

}  // namespace rsl
