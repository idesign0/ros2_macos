#include <rsl/parameter_validators.hpp>

namespace rsl {

auto to_parameter_result_msg(tl::expected<void, std::string> const& result)
    -> rcl_interfaces::msg::SetParametersResult {
    auto msg = rcl_interfaces::msg::SetParametersResult();
    msg.successful = result.has_value();
    msg.reason = result.has_value() ? "" : result.error();
    return msg;
}

}  // namespace rsl
