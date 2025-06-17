#include <rsl/static_string.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace std::literals;

TEST_CASE("rsl::StaticString") {
    SECTION("Type traits") {
        STATIC_CHECK(std::is_copy_constructible_v<rsl::StaticString<8>>);
        STATIC_CHECK(std::is_copy_assignable_v<rsl::StaticString<8>>);
        STATIC_CHECK(std::is_nothrow_move_constructible_v<rsl::StaticString<8>>);
        STATIC_CHECK(std::is_nothrow_move_assignable_v<rsl::StaticString<8>>);
    }

    SECTION("Construction") {
        SECTION("Default constructor") {
            auto const static_string = rsl::StaticString<10>();
            CHECK(static_string.begin() == static_string.end());
        }

        SECTION("Collection constructor") {
            auto const string = "Hello, world!"s;
            auto const static_string = rsl::StaticString<14>(string);
            CHECK(static_string.begin() != static_string.end());
            auto begin = static_string.begin();  // NOLINT(readability-qualified-auto)
            CHECK(*begin++ == 'H');
            CHECK(*begin++ == 'e');
            CHECK(*begin++ == 'l');
            CHECK(*begin++ == 'l');
            CHECK(*begin++ == 'o');
            CHECK(*begin++ == ',');
            CHECK(*begin++ == ' ');
            CHECK(*begin++ == 'w');
            CHECK(*begin++ == 'o');
            CHECK(*begin++ == 'r');
            CHECK(*begin++ == 'l');
            CHECK(*begin++ == 'd');
        }
    }

    SECTION("begin()") {
        auto const static_string = rsl::StaticString<5>("foo");
        CHECK(static_string.begin() == std::begin(static_string));
        CHECK(static_string.begin() == std::cbegin(static_string));
        CHECK(*static_string.begin() == 'f');
    }

    SECTION("end()") {
        auto const static_string = rsl::StaticString<5>("bar");
        CHECK(static_string.end() == std::end(static_string));
        CHECK(static_string.end() == std::cend(static_string));
    }

    SECTION("std::string_view()") {
        auto const static_vector = rsl::StaticString<16>("PickNik Robotics");
        auto const string_view = std::string_view(static_vector);
        CHECK(string_view[0] == 'P');
        CHECK(string_view[1] == 'i');
        CHECK(string_view[2] == 'c');
        CHECK(string_view[3] == 'k');
        CHECK(string_view[4] == 'N');
        CHECK(string_view[5] == 'i');
        CHECK(string_view[6] == 'k');
        CHECK(string_view[7] == ' ');
        CHECK(string_view[8] == 'R');
        CHECK(string_view[9] == 'o');
        CHECK(string_view[10] == 'b');
        CHECK(string_view[11] == 'o');
        CHECK(string_view[12] == 't');
        CHECK(string_view[13] == 'i');
        CHECK(string_view[14] == 'c');
        CHECK(string_view[15] == 's');
    }
}

TEST_CASE("rsl::to_string") {
    CHECK(rsl::to_string(rsl::StaticString<0>()).empty());
    CHECK(rsl::to_string(rsl::StaticString<10>("happy"s)) == "happy"s);
}
