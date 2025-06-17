#include <rsl/monad.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <range/v3/all.hpp>

#include <cmath>
#include <string>

using namespace std::string_literals;

namespace {
constexpr auto maybe_non_zero(int in) { return (in != 0) ? std::optional(in) : std::nullopt; }

auto maybe_lt_3_round(double in) {
    return (in < 3) ? std::optional<int>(std::round(in)) : std::nullopt;
}

auto unsafe_divide_4_by(double val) {
    if (val == 0) throw std::runtime_error("divide by zero");
    return 4.0 / val;
}

template <typename T>
using Result = tl::expected<T, std::string>;

Result<double> divide(double x, double y) {
    if (y == 0) return tl::unexpected("divide by 0"s);
    return x / y;
}

Result<double> multiply(double x, double y) { return x * y; }

Result<double> divide_3(double x) { return divide(3, x); }

Result<double> multiply_3(double x) { return multiply(3, x); }
}  // namespace

TEST_CASE("rsl::mbind") {
    SECTION("Optional value") {
        // GIVEN optional value 4
        constexpr auto opt = std::optional(4);

        // WHEN we rsl::mbind it with the function maybe_non_zero
        // THEN we expect it true
        STATIC_REQUIRE(rsl::mbind(opt, maybe_non_zero));
    }

    SECTION("Overloaded optional value") {
        // GIVEN optional value -4.0
        constexpr auto opt = std::optional(-4.0);

        // WHEN we rsl::mbind it with two functions chained with operator| overload
        // THEN we expect it true
        CHECK((opt | maybe_lt_3_round | maybe_non_zero));
    }

    SECTION("Optional no value") {
        // GIVEN optional non value
        constexpr auto opt = std::optional<int>();

        // WHEN we rsl::mbind it with the function maybe_non_zero
        // THEN we expect it false
        CHECK_FALSE(rsl::mbind(opt, maybe_non_zero));
    }

    SECTION("Optional no value output") {
        // GIVEN optional value with 0.0
        constexpr auto opt = std::optional(0);

        // WHEN we rsl::mbind it with the function maybe_non_zero
        // THEN we expect it false
        CHECK_FALSE(rsl::mbind(opt, maybe_non_zero));
    }

    SECTION("Try") {
        // GIVEN input value of 0.0
        constexpr auto input = 0.0;

        // WHEN we pass unsafe_divide_4_by to rsl::mtry with out input
        // THEN we expect it to not throw
        CHECK_NOTHROW(rsl::mtry(std::bind(&unsafe_divide_4_by, input)));
    }

    SECTION("Try ok") {
        // GIVEN input value of 4.3
        constexpr auto input = 4.3;

        // WHEN we pass unsafe_divide_4_by to rsl::mtry with out input
        // THEN we expect it to not throw
        CHECK_NOTHROW(rsl::mtry(std::bind(&unsafe_divide_4_by, input)));
    }

    SECTION("Compose two") {
        // GIVEN the functions maybe_non_zero and maybe_lt_3_round and an input opt
        constexpr auto opt = std::optional(-4.0);

        // WHEN we compose them together and then bind them with an input
        auto const compose_fn = rsl::mcompose(maybe_lt_3_round, maybe_non_zero);
        auto const compose_result = opt | compose_fn;

        // THEN we expect the result to be the same as if we chained the calls together
        auto const chain_result = opt | maybe_lt_3_round | maybe_non_zero;

        CHECK(compose_result == chain_result);
    }

    SECTION("Compose three") {
        // GIVEN the functions maybe_non_zero and maybe_lt_3_round and an input opt
        auto const opt = std::optional(-4.0);

        // WHEN we compose them together multiple times and then bind them with an input
        auto const compose_result =
            opt | rsl::mcompose(maybe_lt_3_round, maybe_non_zero, maybe_non_zero);

        // THEN we expect the result to be the same as if we chained the calls together
        auto const chain_result = opt | maybe_lt_3_round | maybe_non_zero | maybe_non_zero;

        CHECK(compose_result == chain_result);
    }

    SECTION("Pass unexpected value through bind") {
        Result<double> const input = tl::unexpected("foo"s);
        auto const result = rsl::mbind(input, multiply_3);
        REQUIRE(rsl::has_error(result));
        CHECK(result.error() == "foo");
    }

    SECTION("Divide by 0 produces error with valid input") {
        auto const input = Result<double>{0.0};
        auto const result = rsl::mbind(input, divide_3);
        REQUIRE(rsl::has_error(result));
        CHECK(result.error() == "divide by 0");
    }

    SECTION("operator| divide by 0") {
        auto const input = Result<double>{0.0};
        auto const result = input | divide_3;
        REQUIRE(rsl::has_error(result));
        CHECK(result.error() == "divide by 0");
    }

    SECTION("operator| valid result") {
        CHECK((Result<double>{5.0} | divide_3).value() == Catch::Approx(3 / 5.));
    }

    SECTION("operator| two operations") {
        CHECK((Result<double>{5.0} | divide_3 | multiply_3).value() == Catch::Approx(3 * (3 / 5.)));
    }

    SECTION("operator| different types") {
        auto const result = Result<double>{5.0} | [](double in) -> Result<int> {
            auto const out = static_cast<int>(in);
            auto const check = static_cast<double>(out);
            if (check != in) {
                return tl::unexpected("no worky casty"s);
            }
            return out;
        };
        CHECK(result.value() == 5);
    }
}

TEST_CASE("rsl::has_error") {
    SECTION("Error") {
        // GIVEN expected type containing error
        auto const exp = tl::expected<int, double>(tl::unexpected(0.1));

        // WHEN calling rsl::has_value and tl::expected::has_value
        // THEN we expect rsl::has_value is true, and tl::expected::has_value is false
        CHECK(rsl::has_error(exp));
        CHECK_FALSE(exp.has_value());
    }

    SECTION("Value") {
        // GIVEN expected type containing value
        auto const exp = tl::expected<int, double>(1);

        // WHEN calling rsl::has_value and tl::expected::has_value
        // THEN we expect rsl::has_value is false, and tl::expected::has_value is true
        CHECK_FALSE(rsl::has_error(exp));
        CHECK(exp.has_value());
    }
}

TEST_CASE("rsl::has_value") {
    SECTION("Error") {
        // GIVEN expected type containing error
        auto const exp = tl::expected<int, double>(tl::unexpected(0.1));

        // WHEN calling rsl::has_value and tl::expected::has_value
        // THEN we expect has_error is false, and has_value is false
        CHECK_FALSE(rsl::has_value(exp));
        CHECK_FALSE(exp.has_value());
    }

    SECTION("Value") {
        // GIVEN expected type containing value
        auto const exp = tl::expected<int, double>(1);

        // WHEN calling rsl::has_value and tl::expected::has_value
        // THEN we expect has_error is true, and has_value is true
        CHECK(rsl::has_value(exp));
        CHECK(exp.has_value());
    }
}

TEST_CASE("operator|") {
    auto const i = [](auto x) { return x; };
    auto const k = [](auto x) { return [=](auto) { return x; }; };
    auto const mul = [](auto x, auto y) { return x * y; };
    auto const bind1 = [](auto fn, auto x) { return [=](auto y) { return fn(x, y); }; };
    auto const three = [](auto) { return 3; };
    auto const wrap = [](auto x) { return Result<decltype(x)>{x}; };

    CHECK((int{5} | i) == 5);
    CHECK((int{5} | i | k(3)) == 3);
    CHECK(((int{7} | i | k)(3)) == 7);
    CHECK((int{4} | bind1(mul, 3)) == 12);
    CHECK((double{5} | three) == 3);
    CHECK((double{5} | wrap).value() == Catch::Approx(5));
    CHECK((double{5} | wrap | multiply_3).value() == Catch::Approx(5 * 3));

    SECTION("range-v3 operator| still works") {
        auto v = {20, 10, 15};
        auto r_inv = v | ranges::views::transform([](int x) { return 1.0 / x; });
        auto val = 1.0 / ranges::accumulate(r_inv, 0.0);
        CHECK(val == Catch::Approx(4.61538));
    }
}

TEST_CASE("rsl::maybe_error") {
    SECTION("No error") {
        CHECK(rsl::maybe_error(tl::expected<int, std::string>(42)) == std::nullopt);
        CHECK(rsl::maybe_error(tl::expected<int, std::string>(1), tl::expected<int, std::string>(2),
                               tl::expected<int, std::string>(3), tl::expected<int, std::string>(4),
                               tl::expected<int, std::string>(5)) == std::nullopt);
    }

    SECTION("Errors") {
        CHECK(rsl::maybe_error<std::string, int>(tl::unexpected<std::string>("oops")) ==
              std::optional<std::string>("oops"));
        CHECK(rsl::maybe_error(tl::expected<int, std::string>(tl::unexpect, "oops1"),
                               tl::expected<int, std::string>(tl::unexpect, "oops2"),
                               tl::expected<int, std::string>(tl::unexpect, "oops3")) ==
              std::optional<std::string>("oops1"));
        CHECK(rsl::maybe_error(tl::expected<int, std::string>(tl::unexpect, "oops1"),
                               tl::expected<int, std::string>(1337),
                               tl::expected<int, std::string>(tl::unexpect, "oops2"),
                               tl::expected<int, std::string>(tl::unexpect, "oops3")) ==
              std::optional<std::string>("oops1"));
    }
}
