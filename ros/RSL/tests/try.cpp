#include <rsl/try.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>

// Required because try.hpp uses non-standard C++
#pragma GCC diagnostic ignored "-Wpedantic"

using ExpectedType = tl::expected<int, std::string>;

namespace {
ExpectedType check_try_macro(ExpectedType const& expected) {
    ExpectedType::value_type value = TRY(expected);
    return ++value;
}
}  // namespace

TEST_CASE("TRY") {
    CHECK(check_try_macro(ExpectedType(42)) == 43);
    CHECK(check_try_macro(tl::make_unexpected("Failed!")).error() == "Failed!");
}
