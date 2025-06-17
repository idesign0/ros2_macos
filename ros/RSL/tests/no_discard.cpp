#include <rsl/no_discard.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("rsl::NoDiscard") {
    auto const increment = rsl::NoDiscard([](int i) { return ++i; });
    (void)increment(10);  // Removing (void) should break the build
    CHECK(increment(10) == 11);

    auto const multiply = rsl::NoDiscard([](double x, double y) { return x * y; });
    (void)multiply(2, 3);  // Removing (void) should break the build
    CHECK(multiply(2, 3) == 6);
}
