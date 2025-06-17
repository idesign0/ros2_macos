#include <rsl/random.hpp>

int main() {
    auto& rng = rsl::rng();
    return std::uniform_int_distribution(0, 1)(rng);
}
