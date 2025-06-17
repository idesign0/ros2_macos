#include <rsl/strong_type.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("rsl::StrongType") {
    using StrongInt = rsl::StrongType<int, struct StrongIntTag>;  // For testing constexpr support
    using StrongString =
        rsl::StrongType<std::string, struct StringStringTag>;  // For testing non-constexpr types

    SECTION("Type traits") {
        STATIC_CHECK(std::is_copy_constructible_v<StrongInt>);
        STATIC_CHECK(std::is_copy_assignable_v<StrongInt>);
        STATIC_CHECK(std::is_nothrow_move_constructible_v<StrongInt>);
        STATIC_CHECK(std::is_nothrow_move_assignable_v<StrongInt>);
    }

    SECTION("Construction") {
        constexpr auto strong_int = StrongInt(42);
        STATIC_CHECK(strong_int.get() == 42);
        STATIC_CHECK(int{strong_int} == 42);
        STATIC_CHECK(int(strong_int) == 42);

        auto const strong_string = StrongString("abcdefg");
        CHECK(strong_string.get() == "abcdefg");
        CHECK(std::string{strong_string} == "abcdefg");
        CHECK(std::string(strong_string) == "abcdefg");
    }

    SECTION("get()") {
        auto strong_int = StrongInt(1337);
        strong_int.get() = 100;
        CHECK(strong_int.get() == 100);
    }
}
