#include <rsl/queue.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <atomic>
#include <thread>
#include <type_traits>

using namespace std::chrono_literals;

// NOLINTBEGIN(readability-container-size-empty)

TEST_CASE("rsl::Queue") {
    SECTION("Type traits") {
        STATIC_CHECK(!std::is_copy_constructible_v<rsl::Queue<int>>);
        STATIC_CHECK(!std::is_copy_assignable_v<rsl::Queue<int>>);
        STATIC_CHECK(!std::is_nothrow_move_constructible_v<rsl::Queue<int>>);
        STATIC_CHECK(!std::is_nothrow_move_assignable_v<rsl::Queue<int>>);
    }

    SECTION("Default constructor") {
        auto const queue = rsl::Queue<int>();
        CHECK(queue.size() == 0);
        CHECK(queue.empty());
    }

    SECTION("push()") {
        auto queue = rsl::Queue<int>();
        queue.push(42);
        CHECK(queue.size() == 1);
        CHECK(!queue.empty());

        for (int i = 0; i < 99; ++i) queue.push(i);
        CHECK(queue.size() == 100);
        CHECK(!queue.empty());
    }

    SECTION("clear()") {
        auto queue = rsl::Queue<int>();
        for (int i = 0; i < 100; ++i) queue.push(i);
        CHECK(queue.size() == 100);
        CHECK(!queue.empty());
        queue.clear();
        CHECK(queue.size() == 0);
        CHECK(queue.empty());
    }

    SECTION("pop()") {
        SECTION("Sequentially") {
            auto queue = rsl::Queue<int>();

            CHECK(!queue.pop().has_value());
            queue.push(999);
            queue.push(1000);

            CHECK(queue.pop(-1ms).value() == 999);
            CHECK(queue.size() == 1);

            CHECK(queue.pop().value() == 1000);
            CHECK(queue.size() == 0);

            CHECK(!queue.pop(1ms).has_value());
            CHECK(queue.size() == 0);
        }

        SECTION("Concurrently") {
            auto queue = rsl::Queue<int>();

            constexpr auto thread_count = size_t(10);
            constexpr auto item_count = size_t(100);

            auto producers = std::array<std::thread, thread_count>();
            for (auto& producer : producers) {
                producer = std::thread([&queue] {
                    for (size_t i = 0; i < item_count; ++i) queue.push(int(i));
                });
            }

            auto items_removed = std::atomic<size_t>(0);
            auto consumers = std::array<std::thread, thread_count>();
            for (auto& consumer : consumers) {
                consumer = std::thread([&queue, &items_removed] {
                    for (size_t i = 0; i < item_count; ++i)
                        if (queue.pop(1ms).has_value()) ++items_removed;
                });
            }

            for (auto& producer : producers) producer.join();
            for (auto& consumer : consumers) consumer.join();

            CHECK(queue.size() == thread_count * item_count - items_removed);
        }
    }
}

// NOLINTEND(readability-container-size-empty)
