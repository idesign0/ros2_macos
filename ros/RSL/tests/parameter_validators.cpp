#include <rsl/parameter_validators.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace std::literals;
using rclcpp::Parameter;

TEST_CASE("rsl::unique") {
    CHECK(rsl::unique<std::string>(Parameter("", std::vector<std::string>{"", "1", "2"})));
    CHECK(rsl::unique<std::string>(Parameter("", std::vector<std::string>{})));
    CHECK(!rsl::unique<std::string>(Parameter("", std::vector<std::string>{"foo", "foo"})));

    CHECK(rsl::unique<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1})));
    CHECK(rsl::unique<double>(Parameter("", std::vector<double>{})));
    CHECK(!rsl::unique<double>(Parameter("", std::vector<double>{1.1, 1.1})));

    CHECK(rsl::unique<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3})));
    CHECK(rsl::unique<int64_t>(Parameter("", std::vector<int64_t>{})));
    CHECK(!rsl::unique<int64_t>(Parameter("", std::vector<int64_t>{-1, -1})));

    CHECK(rsl::unique<bool>(Parameter("", std::vector<bool>{true, false})));
    CHECK(rsl::unique<bool>(Parameter("", std::vector<bool>{})));
    CHECK(!rsl::unique<bool>(Parameter("", std::vector<bool>{false, false})));

    auto const result =
        rsl::unique<std::string>(Parameter("test", std::vector<std::string>{"foo", "foo"}));
    REQUIRE(!result);
    CHECK(result.error() == "Parameter 'test' must only contain unique values");
}

TEST_CASE("rsl::subset_of") {
    CHECK(rsl::subset_of<std::string>(Parameter("", std::vector<std::string>{"", "1", "2"}),
                                      std::vector<std::string>{"", "1", "2", "three"}));
    CHECK(rsl::subset_of<std::string>(Parameter("", std::vector<std::string>{}),
                                      std::vector<std::string>{"", "1", "2", "three"}));
    CHECK(!rsl::subset_of<std::string>(Parameter("", std::vector<std::string>{"foo", "foo"}),
                                       std::vector<std::string>{"", "1", "2", "three"}));

    CHECK(rsl::subset_of<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}),
                                 std::vector<double>{1.0, 2.2, 1.1}));
    CHECK(
        rsl::subset_of<double>(Parameter("", std::vector<double>{}), std::vector<double>{10, 22}));
    CHECK(!rsl::subset_of<double>(Parameter("", std::vector<double>{1.1, 1.1}),
                                  std::vector<double>{1.0, 2.2}));

    CHECK(rsl::subset_of<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}),
                                  std::vector<int64_t>{1, 2, 3}));
    CHECK(rsl::subset_of<int64_t>(Parameter("", std::vector<int64_t>{}),
                                  std::vector<int64_t>{1, 2, 3}));
    CHECK(!rsl::subset_of<int64_t>(Parameter("", std::vector<int64_t>{-1, -1}),
                                   std::vector<int64_t>{1, 2, 3}));

    CHECK(rsl::subset_of<bool>(Parameter("", std::vector<bool>{true, false}),
                               std::vector<bool>{true, false}));
    CHECK(rsl::subset_of<bool>(Parameter("", std::vector<bool>{}), std::vector<bool>{true, false}));
    CHECK(!rsl::subset_of<bool>(Parameter("", std::vector<bool>{false, false}),
                                std::vector<bool>{true}));

    auto const result =
        rsl::subset_of<std::string>(Parameter("test", std::vector<std::string>{"foo", "foo"}),
                                    std::vector<std::string>{"", "1", "2", "three"});
    REQUIRE(!result);
    CHECK(result.error() == "Entry 'foo' in parameter 'test' is not in the set '{, 1, 2, three}'");
}

TEST_CASE("rsl::fixed_size") {
    CHECK(rsl::fixed_size<std::string>(Parameter("", "foo"), 3));
    CHECK(rsl::fixed_size<std::string>(Parameter("", ""), 0));
    CHECK(!rsl::fixed_size<std::string>(Parameter("", "foo"), 0));
    CHECK(!rsl::fixed_size<std::string>(Parameter("", "foo"), 5));

    CHECK(rsl::fixed_size<std::string>(Parameter("", std::vector<std::string>{"", "1", "2"}), 3));
    CHECK(rsl::fixed_size<std::string>(Parameter("", std::vector<std::string>{}), 0));
    CHECK(!rsl::fixed_size<std::string>(Parameter("", std::vector<std::string>{"foo", "foo"}), 3));

    CHECK(rsl::fixed_size<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}), 3));
    CHECK(rsl::fixed_size<double>(Parameter("", std::vector<double>{}), 0));
    CHECK(!rsl::fixed_size<double>(Parameter("", std::vector<double>{1.1, 1.1}), 3));

    CHECK(rsl::fixed_size<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 3));
    CHECK(rsl::fixed_size<int64_t>(Parameter("", std::vector<int64_t>{}), 0));
    CHECK(!rsl::fixed_size<int64_t>(Parameter("", std::vector<int64_t>{-1, -1}), 0));

    CHECK(rsl::fixed_size<bool>(Parameter("", std::vector<bool>{true, false}), 2));
    CHECK(rsl::fixed_size<bool>(Parameter("", std::vector<bool>{}), 0));
    CHECK(!rsl::fixed_size<bool>(Parameter("", std::vector<bool>{false, false}), 0));

    auto const result = rsl::fixed_size<std::string>(Parameter("test", "foo"), 0);
    REQUIRE(!result);
    CHECK(result.error() == "Length of parameter 'test' is '3' but must be equal to '0'");
}

TEST_CASE("rsl::size_gt") {
    CHECK(rsl::size_gt<std::string>(Parameter("", "foo"), 2));
    CHECK(!rsl::size_gt<std::string>(Parameter("", ""), 0));
    CHECK(!rsl::size_gt<std::string>(Parameter("", "foo"), 5));

    CHECK(rsl::size_gt<std::string>(Parameter("", std::vector<std::string>{"", "1", "2"}), 1));
    CHECK(!rsl::size_gt<std::string>(Parameter("", std::vector<std::string>{}), 0));
    CHECK(!rsl::size_gt<std::string>(Parameter("", std::vector<std::string>{"foo", "foo"}), 3));

    CHECK(rsl::size_gt<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}), 2));
    CHECK(!rsl::size_gt<double>(Parameter("", std::vector<double>{}), 0));
    CHECK(!rsl::size_gt<double>(Parameter("", std::vector<double>{1.1, 1.1}), 3));

    CHECK(rsl::size_gt<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 2));
    CHECK(!rsl::size_gt<int64_t>(Parameter("", std::vector<int64_t>{}), 0));
    CHECK(rsl::size_gt<int64_t>(Parameter("", std::vector<int64_t>{-1, -1}), 0));

    CHECK(rsl::size_gt<bool>(Parameter("", std::vector<bool>{true, false}), 0));
    CHECK(!rsl::size_gt<bool>(Parameter("", std::vector<bool>{}), 0));
    CHECK(rsl::size_gt<bool>(Parameter("", std::vector<bool>{false, false}), 0));

    auto const result = rsl::size_gt<std::string>(Parameter("test", ""), 0);
    REQUIRE(!result);
    CHECK(result.error() == "Length of parameter 'test' is '0' but must be greater than '0'");
}

TEST_CASE("rsl::size_lt") {
    CHECK(rsl::size_lt<std::string>(Parameter("", "foo"), 5));
    CHECK(!rsl::size_lt<std::string>(Parameter("", ""), 0));
    CHECK(!rsl::size_lt<std::string>(Parameter("", "foo"), 3));

    CHECK(!rsl::size_lt<std::string>(Parameter("", std::vector<std::string>{"", "1", "2"}), 1));
    CHECK(!rsl::size_lt<std::string>(Parameter("", std::vector<std::string>{}), 0));
    CHECK(rsl::size_lt<std::string>(Parameter("", std::vector<std::string>{"foo", "foo"}), 3));

    CHECK(!rsl::size_lt<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}), 2));
    CHECK(!rsl::size_lt<double>(Parameter("", std::vector<double>{}), 0));
    CHECK(rsl::size_lt<double>(Parameter("", std::vector<double>{1.1, 1.1}), 3));

    CHECK(!rsl::size_lt<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 2));
    CHECK(!rsl::size_lt<int64_t>(Parameter("", std::vector<int64_t>{}), 0));
    CHECK(!rsl::size_lt<int64_t>(Parameter("", std::vector<int64_t>{-1, -1}), 0));

    CHECK(rsl::size_lt<bool>(Parameter("", std::vector<bool>{true, false}), 3));
    CHECK(!rsl::size_lt<bool>(Parameter("", std::vector<bool>{}), 0));
    CHECK(!rsl::size_lt<bool>(Parameter("", std::vector<bool>{false, false}), 0));

    auto const result = rsl::size_lt<std::string>(Parameter("test", "foo"), 3);
    REQUIRE(!result);
    CHECK(result.error() == "Length of parameter 'test' is '3' but must be less than '3'");
}

TEST_CASE("rsl::not_empty") {
    CHECK(rsl::not_empty<std::string>(Parameter("", "foo")));
    CHECK(!rsl::not_empty<std::string>(Parameter("", "")));

    CHECK(rsl::not_empty<std::string>(Parameter("", std::vector<std::string>{"", "1", "2"})));
    CHECK(!rsl::not_empty<std::string>(Parameter("", std::vector<std::string>{})));

    CHECK(rsl::not_empty<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1})));
    CHECK(!rsl::not_empty<double>(Parameter("", std::vector<double>{})));

    CHECK(rsl::not_empty<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3})));
    CHECK(!rsl::not_empty<int64_t>(Parameter("", std::vector<int64_t>{})));

    CHECK(rsl::not_empty<bool>(Parameter("", std::vector<bool>{true, false})));
    CHECK(!rsl::not_empty<bool>(Parameter("", std::vector<bool>{})));

    auto const result = rsl::not_empty<std::string>(Parameter("test", ""));
    REQUIRE(!result);
    CHECK(result.error() == "Parameter 'test' cannot be empty");
}

TEST_CASE("rsl::element_bounds") {
    CHECK(rsl::element_bounds<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}), 0.0, 3.0));
    CHECK(rsl::element_bounds<double>(Parameter("", std::vector<double>{}), 0.0, 1.0));

    CHECK(rsl::element_bounds<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 0, 5));
    CHECK(!rsl::element_bounds<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), -5, 0));
    CHECK(rsl::element_bounds<int64_t>(Parameter("", std::vector<int64_t>{}), 0, 1));

    CHECK(rsl::element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), false, true));
    CHECK(!rsl::element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), true, false));
    CHECK(!rsl::element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), false, false));
    CHECK(rsl::element_bounds<bool>(Parameter("", std::vector<bool>{}), true, false));

    auto const result =
        rsl::element_bounds<int64_t>(Parameter("test", std::vector<int64_t>{1, 2, 3}), -5, 0);
    REQUIRE(!result);
    CHECK(result.error() == "Value '1' in parameter 'test' must be within bounds '[-5, 0]'");
}

TEST_CASE("rsl::lower_element_bounds") {
    CHECK(
        rsl::lower_element_bounds<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}), 0.0));
    CHECK(rsl::lower_element_bounds<double>(Parameter("", std::vector<double>{}), 0.0));

    CHECK(rsl::lower_element_bounds<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 0));
    CHECK(!rsl::lower_element_bounds<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 3));
    CHECK(rsl::lower_element_bounds<int64_t>(Parameter("", std::vector<int64_t>{}), 0));

    CHECK(rsl::lower_element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), false));
    CHECK(!rsl::lower_element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), true));
    CHECK(rsl::lower_element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), false));
    CHECK(rsl::lower_element_bounds<bool>(Parameter("", std::vector<bool>{}), true));

    auto const result =
        rsl::lower_element_bounds<int64_t>(Parameter("test", std::vector<int64_t>{1, 2, 3}), 3);
    REQUIRE(!result);
    CHECK(result.error() == "Value '1' in parameter 'test' must be above lower bound of '3'");
}

TEST_CASE("rsl::upper_element_bounds") {
    CHECK(
        !rsl::upper_element_bounds<double>(Parameter("", std::vector<double>{1.0, 2.2, 1.1}), 0.0));
    CHECK(rsl::upper_element_bounds<double>(Parameter("", std::vector<double>{}), 0.0));

    CHECK(!rsl::upper_element_bounds<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 0));
    CHECK(rsl::upper_element_bounds<int64_t>(Parameter("", std::vector<int64_t>{1, 2, 3}), 3));
    CHECK(rsl::upper_element_bounds<int64_t>(Parameter("", std::vector<int64_t>{}), 0));

    CHECK(!rsl::upper_element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), false));
    CHECK(rsl::upper_element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), true));
    CHECK(!rsl::upper_element_bounds<bool>(Parameter("", std::vector<bool>{true, false}), false));
    CHECK(rsl::upper_element_bounds<bool>(Parameter("", std::vector<bool>{}), true));

    auto const result = rsl::upper_element_bounds<double>(
        Parameter("test", std::vector<double>{1.0, 2.2, 1.1}), 0.0);
    REQUIRE(!result);
    CHECK(result.error() == "Value '1' in parameter 'test' must be below upper bound of '0'");
}

TEST_CASE("rsl::bounds") {
    CHECK(rsl::bounds<double>(Parameter("", 1.0), 1.0, 5.0));
    CHECK(rsl::bounds<double>(Parameter("", 4.3), 1.0, 5.0));
    CHECK(rsl::bounds<double>(Parameter("", 5.0), 1.0, 5.0));
    CHECK(!rsl::bounds<double>(Parameter("", -4.3), 1.0, 5.0));
    CHECK(!rsl::bounds<double>(Parameter("", 10.2), 1.0, 5.0));

    CHECK(rsl::bounds<int64_t>(Parameter("", 1), 1, 5));
    CHECK(rsl::bounds<int64_t>(Parameter("", 4), 1, 5));
    CHECK(rsl::bounds<int64_t>(Parameter("", 5), 1, 5));
    CHECK(!rsl::bounds<int64_t>(Parameter("", -4), 1, 5));
    CHECK(!rsl::bounds<int64_t>(Parameter("", 10), 1, 5));

    CHECK(rsl::bounds<bool>(Parameter("", true), false, true));
    CHECK(!rsl::bounds<bool>(Parameter("", false), true, true));
    CHECK(!rsl::bounds<bool>(Parameter("", true), false, false));

    CHECK_THROWS(rsl::bounds<int64_t>(Parameter("", ""), 1, 5));
    CHECK_THROWS(rsl::bounds<std::string>(Parameter("", 4), "", "foo"));

    auto const result = rsl::bounds<double>(Parameter("test", -4.3), 1.0, 5.0);
    REQUIRE(!result);
    CHECK(result.error() ==
          "Parameter 'test' with the value '-4.3' must be within bounds '[1, 5]'");
}

TEST_CASE("rsl::lt") {
    CHECK(!rsl::lt<double>(Parameter("", 2.0), 2.0));
    CHECK(!rsl::lt<double>(Parameter("", 4.3), 1.0));
    CHECK(rsl::lt<double>(Parameter("", -4.3), 1.0));

    CHECK(!rsl::lt<int64_t>(Parameter("", 1), 1));
    CHECK(!rsl::lt<int64_t>(Parameter("", 4), 1));
    CHECK(rsl::lt<int64_t>(Parameter("", -4), 1));

    CHECK(!rsl::lt<bool>(Parameter("", true), false));
    CHECK(rsl::lt<bool>(Parameter("", false), true));
    CHECK(!rsl::lt<bool>(Parameter("", true), true));

    auto const result = rsl::lt<double>(Parameter("test", 4.3), 1.0);
    REQUIRE(!result);
    CHECK(result.error() == "Parameter 'test' with the value '4.3' must be less than '1'");
}

TEST_CASE("rsl::gt") {
    CHECK(!rsl::gt<double>(Parameter("", 2.0), 2.0));
    CHECK(rsl::gt<double>(Parameter("", 4.3), 1.0));
    CHECK(!rsl::gt<double>(Parameter("", -4.3), 1.0));

    CHECK(!rsl::gt<int64_t>(Parameter("", 1), 1));
    CHECK(rsl::gt<int64_t>(Parameter("", 4), 1));
    CHECK(!rsl::gt<int64_t>(Parameter("", -4), 1));

    CHECK(rsl::gt<bool>(Parameter("", true), false));
    CHECK(!rsl::gt<bool>(Parameter("", false), true));
    CHECK(!rsl::gt<bool>(Parameter("", true), true));

    auto const result = rsl::gt<int64_t>(Parameter("test", 1), 1);
    REQUIRE(!result);
    CHECK(result.error() == "Parameter 'test' with the value '1' must be greater than '1'");
}

TEST_CASE("rsl::lt_eq") {
    CHECK(rsl::lt_eq<double>(Parameter("", 2.0), 2.0));
    CHECK(!rsl::lt_eq<double>(Parameter("", 4.3), 1.0));
    CHECK(rsl::lt_eq<double>(Parameter("", -4.3), 1.0));

    CHECK(rsl::lt_eq<int64_t>(Parameter("", 1), 1));
    CHECK(!rsl::lt_eq<int64_t>(Parameter("", 4), 1));
    CHECK(rsl::lt_eq<int64_t>(Parameter("", -4), 1));

    CHECK(!rsl::lt_eq<bool>(Parameter("", true), false));
    CHECK(rsl::lt_eq<bool>(Parameter("", false), true));
    CHECK(rsl::lt_eq<bool>(Parameter("", true), true));

    auto const result = rsl::lt_eq<double>(Parameter("test", 4.3), 1.0);
    REQUIRE(!result);
    CHECK(result.error() ==
          "Parameter 'test' with the value '4.3' must be less than or equal to '1'");
}

TEST_CASE("rsl::gt_eq") {
    CHECK(rsl::gt_eq<double>(Parameter("", 2.0), 2.0));
    CHECK(rsl::gt_eq<double>(Parameter("", 4.3), 1.0));
    CHECK(!rsl::gt_eq<double>(Parameter("", -4.3), 1.0));

    CHECK(rsl::gt_eq<int64_t>(Parameter("", 1), 1));
    CHECK(rsl::gt_eq<int64_t>(Parameter("", 4), 1));
    CHECK(!rsl::gt_eq<int64_t>(Parameter("", -4), 1));

    CHECK(rsl::gt_eq<bool>(Parameter("", true), false));
    CHECK(!rsl::gt_eq<bool>(Parameter("", false), true));
    CHECK(rsl::gt_eq<bool>(Parameter("", true), true));

    auto const result = rsl::gt_eq<double>(Parameter("test", -4.3), 1.0);
    REQUIRE(!result);
    CHECK(result.error() ==
          "Parameter 'test' with the value '-4.3' must be greater than or equal to '1'");
}

TEST_CASE("rsl::one_of") {
    CHECK(rsl::one_of<double>(Parameter("", 2.0), std::vector<double>{2.0}));
    CHECK(rsl::one_of<double>(Parameter("", 2.0), std::vector<double>{1.0, 2.0, 3.5}));
    CHECK(!rsl::one_of<double>(Parameter("", 0.0), std::vector<double>{1.0, 2.0, 3.5}));
    CHECK(!rsl::one_of<double>(Parameter("", 0.0), std::vector<double>{}));

    CHECK(rsl::one_of<int64_t>(Parameter("", 1), std::vector<int64_t>{1}));
    CHECK(rsl::one_of<int64_t>(Parameter("", 1), std::vector<int64_t>{1, 2, 3, 4}));
    CHECK(!rsl::one_of<int64_t>(Parameter("", 0), std::vector<int64_t>{1, 2, 3, 4}));
    CHECK(!rsl::one_of<int64_t>(Parameter("", 0), std::vector<int64_t>{}));

    CHECK(!rsl::one_of<bool>(Parameter("", true), std::vector<bool>{false}));
    CHECK(rsl::one_of<bool>(Parameter("", true), std::vector<bool>{false, true}));
    CHECK(rsl::one_of<bool>(Parameter("", true), std::vector<bool>{true}));
    CHECK(!rsl::one_of<bool>(Parameter("", true), std::vector<bool>{}));

    CHECK(rsl::one_of(Parameter("", "foo"), std::vector<std::string>{"foo", "baz"}));
    CHECK(!rsl::one_of(Parameter("", ""), std::vector<std::string>{"foo", "baz"}));

    auto const result =
        rsl::one_of<double>(Parameter("test", 0.0), std::vector<double>{1.0, 2.0, 3.5});
    REQUIRE(!result);
    CHECK(result.error() == "Parameter 'test' with the value '0' is not in the set '{1, 2, 3.5}'");
}

TEST_CASE("rsl::to_parameter_result_msg") {
    SECTION("Has value") {
        auto const parameter_result = rsl::to_parameter_result_msg({});
        CHECK(parameter_result.successful);
        CHECK(parameter_result.reason.empty());
    }

    SECTION("Has error") {
        auto const parameter_result = rsl::to_parameter_result_msg(tl::unexpected("test"s));
        CHECK(!parameter_result.successful);
        CHECK(parameter_result.reason == "test");
    }
}
