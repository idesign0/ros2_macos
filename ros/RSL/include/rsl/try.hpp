#pragma once

#include <tl_expected/expected.hpp>

/** @file */

/**
 * @brief Unwrap an expected into a stack variable or return the unexpected value
 *
 * This macro requires the use of non-standard C++. Compiler extensions must be enabled and
 * -Wpedantic cannot be used. With GCC and Clang, you can disable -Wpedantic for an entire
 * translation unit like this:
 *
 * ```cpp
 * #pragma GCC diagnostic ignored "-Wpedantic"
 * ```

 * You can disable -Wpedantic for a specific segment of code like so:
 *
 * ```cpp
 * #pragma GCC diagnostic push
 * #pragma GCC diagnostic ignored "-Wpedantic"
 * //...
 * #pragma GCC diagnostic pop
 * ```
 *
 * For more information on this particular compiler extension go to
 * https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 */
#define TRY(expected)                                                              \
    ({                                                                             \
        auto const& _expected = (expected); /* Uglify name to prevent shadowing */ \
        if (!_expected.has_value()) return tl::unexpected(_expected.error());      \
        _expected.value();                                                         \
    })
