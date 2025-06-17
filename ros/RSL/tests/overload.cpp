#include <rsl/overload.hpp>

#include <catch2/catch_test_macros.hpp>

#include <variant>

TEST_CASE("rsl::Overload") {
    enum class Type : std::uint8_t { INT, FLOAT, STRING };

    auto const overload =
        rsl::Overload{[](int) { return Type::INT; }, [](float) { return Type::FLOAT; },
                      [](std::string const&) { return Type::STRING; }};

    auto variant = std::variant<int, float, std::string>();
    CHECK(std::visit(overload, variant = 12) == Type::INT);
    CHECK(std::visit(overload, variant = 42.f) == Type::FLOAT);
    CHECK(std::visit(overload, variant = "PickNik") == Type::STRING);
}
