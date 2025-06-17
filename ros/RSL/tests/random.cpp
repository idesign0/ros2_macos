#include <rsl/random.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <thread>

namespace {
auto const* const rng = &rsl::rng({0, 1});
}

TEST_CASE("rsl::rng") {
    SECTION("Repeated calls in thread yield same object") {
        CHECK(rng == &rsl::rng());
        CHECK(rng == &rsl::rng());
        CHECK(rng == &rsl::rng());
    }

    SECTION("Calls from separate threads yield separate objects") {
        // Test using randomly generated seed
        auto thread1 = std::thread([] { CHECK(rng != &rsl::rng()); });

        // Test that custom seed in separate thread does not throw
        auto thread2 = std::thread([] { CHECK(rng != &rsl::rng({2, 3})); });

        thread1.join();
        thread2.join();
    }

    SECTION("Throw if seeded after first call") {
        CHECK_NOTHROW(rsl::rng({}));
        CHECK_THROWS_WITH(rsl::rng({1, 2, 3, 4}), Catch::Matchers::ContainsSubstring(
                                                      "rng cannot be re-seeded on this thread"));
    }
}

TEST_CASE("rsl::uniform_real") {
    constexpr auto lower = -100.;
    constexpr auto upper = 100.;
    for (int i = 0; i < 1'000; ++i) {
        auto const value = rsl::uniform_real(lower, upper);
        CHECK(value >= lower);
        CHECK(value < upper);
    }
}

TEST_CASE("rsl::uniform_int") {
    constexpr auto lower = -100;
    constexpr auto upper = 100;
    for (int i = 0; i < 1'000; ++i) {
        auto const value = rsl::uniform_int(lower, upper);
        CHECK(value >= lower);
        CHECK(value <= upper);
    }
}

TEST_CASE("rsl::random_unit_quaternion") {
    for (int i = 0; i < 1'000; ++i)
        CHECK(rsl::random_unit_quaternion().norm() == Catch::Approx(1.).epsilon(0).margin(1e-6));
}
