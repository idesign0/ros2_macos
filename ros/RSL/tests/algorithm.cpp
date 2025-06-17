#include <rsl/algorithm.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <set>
#include <vector>

TEST_CASE("rsl::contains") {
    SECTION("No items") {
        CHECK_FALSE(rsl::contains(std::array<int, 0>{}, 0));
        CHECK_FALSE(rsl::contains(std::vector<int>{}, 0));
    }

    SECTION("One item") {
        auto const values = std::vector{-1};
        CHECK(rsl::contains(values, -1));
        CHECK_FALSE(rsl::contains(values, 0));
    }

    SECTION("Two items that are the same") {
        auto const values = std::array{-1., -1.};
        CHECK(rsl::contains(values, -1.));
        CHECK_FALSE(rsl::contains(values, 0.));
    }

    SECTION("Two items that are different") {
        auto const values = std::set{-1, 1};
        CHECK(rsl::contains(values, -1));
        CHECK(rsl::contains(values, 1));
        CHECK_FALSE(rsl::contains(values, 0));
    }
}

TEST_CASE("rsl::is_unique") {
    SECTION("No items") {
        CHECK(rsl::is_unique(std::array<int, 0>{}));
        CHECK(rsl::is_unique(std::vector<int>{}));
    }

    SECTION("One item") {
        CHECK(rsl::is_unique(std::array{42, 117}));
        CHECK(rsl::is_unique(std::vector<int>{-1}));
    }

    SECTION("Two items that are the same") {
        CHECK_FALSE(rsl::is_unique(std::array{99, 99}));
        CHECK_FALSE(rsl::is_unique(std::vector<int>{-1, -1}));
    }

    SECTION("Two items that are different") {
        CHECK(rsl::is_unique(std::array{-1, 1}));
        CHECK(rsl::is_unique(std::vector<int>{-1, 1}));
    }
}
