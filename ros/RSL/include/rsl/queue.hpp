#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace rsl {

/** @file */

/**
 * @brief Thread-safe queue. Particularly useful when multiple threads need to write to and/or read
 * from a queue.
 */
template <typename T>
class Queue {
    std::queue<T> queue_;
    std::condition_variable cv_;
    mutable std::mutex mutex_;

   public:
    /**
     * @brief Get the size of the queue
     * @return Queue size
     */
    [[nodiscard]] auto size() const noexcept {
        auto const lock = std::lock_guard(mutex_);
        return queue_.size();
    }

    /**
     * @brief Check if the queue is empty
     * @return True if the queue is empty, otherwise false
     */
    [[nodiscard]] auto empty() const noexcept {
        auto const lock = std::lock_guard(mutex_);
        return queue_.empty();
    }

    /**
     * @brief Push data into the queue
     * @param value Data to push into the queue
     */
    void push(T value) noexcept {
        auto const lock = std::lock_guard(mutex_);
        queue_.push(std::move(value));
        cv_.notify_one();
    }

    /**
     * @brief Clear the queue
     */
    void clear() noexcept {
        auto const lock = std::lock_guard(mutex_);

        // Swap queue with an empty queue of the same type to ensure queue_ is left in a
        // default-constructed state
        decltype(queue_)().swap(queue_);
    }

    /**
     * @brief Wait for given duration then pop from the queue and return the element
     * @param wait_time Maximum time to wait for queue to be non-empty
     * @return Data popped from the queue or error
     */
    [[nodiscard]] auto pop(std::chrono::nanoseconds wait_time = {}) -> std::optional<T> {
        auto lock = std::unique_lock(mutex_);

        // If queue is empty after wait_time, return nothing
        if (!cv_.wait_for(lock, wait_time, [this] { return !queue_.empty(); })) return std::nullopt;

        auto value = queue_.front();
        queue_.pop();
        return value;
    }
};
}  // namespace rsl
