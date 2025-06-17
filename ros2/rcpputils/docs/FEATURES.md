# rcpputils Features

This package includes convenience functions for cross-platform c++ functionality and for mathematical utilties. These functionalities are siloed into the `rcpputils` and the `rcppmath` namespaces respectively:
* [`rcpputils` - General convenience functions](#rcpputils-general-convenience-functions)
  * [Assertion functions](#assertion-functions)
  * [Clang thread safety annotation macros](#clang-thread-safety-annotation-macros)
  * [Endianness helpers](#endianness-helpers)
  * [Library discovery](#library-discovery)
  * [String helpers](#string-helpers)
  * [File system helpers](#file-system-helpers)
  * [Type traits helpers](#type-traits-helpers)
  * [Visibility macros](#visibility-macros)
  * [Shared Libraries](#shared-libraries)
  * [Process helpers](#process-helpers)
  * [Environment helpers](#environment-helpers)
  * [Scope guard support](#scope-guard-support)
* [`rcppmath` - Mathematical utilties](#rcppmath-mathematical-utilities)
  * [Clamping functionality](#clamping-functionality)
  * [Rolling mean accumulator](#rolling-mean-accumulator)

## `rcpputils` - General convenience functions {#rcpputils-general-convenience-functions}
### Assertion Functions {#assertion-functions}
The `rcpputils/asserts.hpp` header provides the helper functions:
* `rcpputils::require_true()`: for validating function inputs. Throws an `std::invalid_argument` exception if the condition fails.
* `rcpputils::check_true()`: for checking states. Throws a `rcpputils::IllegalStateException` if the condition fails.
* `rcpputils::assert_true()`: for verifying results. Throws a `rcpputils::AssertionException` if the condition fails. This function becomes a no-op in release builds.

These helper functions can be used to improve readability of C++ functions.
Example usage:
```c++
ResultType some_function(ArgType arg)
{
  // Check if the required internal state for calling `some_function` is valid.
  // There's usually no reason to operate on the input if the state is invalid.
  rcpputils::check_true(internal_state.is_valid);

  // Check if the arguments are valid.
  // It's usually best practice to only process valid inputs.
  rcpputils::require_true(arg.is_valid);

  auto result = do_logic(arg);

  // Assert that the result is valid.
  // This will only throw when ran in debug mode if result is invalid.
  // Throwing early when invalid may help find points of error when debugging.
  rcpputils::assert_true(result.is_valid);
  return result
}
```

### Clang Thread Safety Annotation Macros {#clang-thread-safety-annotation-macros}
The `rcpputils/thread_safety_annotations.hpp` header provides macros for Clang's [Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) feature.

The macros allow you to annotate your code, but expand to nothing when using a non-clang compiler, so they are safe for cross-platform use.

To use thread safety annotation in your package (in a Clang+libcxx build), enable the `-Wthread-safety` compiler flag.

For example usage, see [the documentation of this feature](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) and the tests in `test/test_thread_safety_annotations.cpp`.

### Endianness helpers {#endianness-helpers}
The `rcpputils/endian.hpp` header emulates the features of `std::endian` if it is not available.
See [cppreference](https://en.cppreference.com/w/cpp/types/endian) for more information.

### Library Discovery {#library-discovery}
The `rcpputils/find_library.hpp` facilitates finding a library located in the OS's library paths environment variable.

* `rcpputils::find_library_path(const std::string &)`: Searches for the given library name in a OS's library paths environment variable, and returns an absolute filesystem path, including the platform-specific prefix and extension. If the library is not found, returns an empty string.
  * For dynamically loading user-defined plugins in C++, please use [`pluginlib`](https://github.com/ros/pluginlib) instead.

### String Helpers {#string-helpers}
String helper utilities can be found in the `rcpputils/find_and_replace.hpp`, `rcpputils/join.hpp`, and `rcpputils/split.hpp` headers.

These headers provide the following functionality respectively:
* `rcpputils::find_and_replace()`: On an input string, finds and replaces all instances of a string with another string.
* `rcpputils::join()`: Values in a provided container are converted to strings, and joined by a provided delimiter.
* `rcpputils::split()`: This is an overloaded method. It splits a specified input into string tokens using a delimiter, and:
  * If an iterator to a storage container is provided, stores tokens in said container.
  * Else, returns a `std::vector<string>` containing the tokens.

### File system helpers {#file-system-helpers}
`rcpputils/filesystem_helper.hpp` provides `std::filesystem`-like functionality on systems that do not yet include those features. See the [cppreference](https://en.cppreference.com/w/cpp/header/filesystem) for more information.

### Type traits helpers {#type-traits-helpers}
`rcpputils/pointer_traits.hpp` provides several type trait definitions for pointers and smart pointers.

### Visibility definitions and macros {#visibility-definitions-and-macros}
`rcpputils/visibility_control.hpp` provides macros and definitions for controlling the visibility of class members. The logic was borrowed and then namespaced from [https://gcc.gnu.org/wiki/Visibility](https://gcc.gnu.org/wiki/Visibility).

### Shared Libraries
The `rcpputils/shared_library.hpp` header provides utilities to dynamically load, unload, and get symbols from shared libraries at run-time.

Example usage:
```c++
// Obtain platform-specific library name from the provided base name.
const std::string library_name = rcpputils::get_platform_library_name("foo_lib");

// The shared library is loaded in the constructor.
auto library = std::make_shared<rcpputils::SharedLibrary>(library_name);

// Check if a symbol exists in the shared library.
// If so, obtain a pointer to the symbol.
if (library->has_symbol("example_symbol")) {
    std::shared_ptr<void> symbol(library->get_symbol("example_symbol"));
    // Use shared library symbol pointer.
}
```

## Process helpers {#process-helpers}
The `rcpputils/process.hpp` header contains process utilities.

Namely, this header provides the `rcpputils::get_executable_name()` function, which retrieves and returns the current program name as a string.

## Environment helpers {#environment-helpers}
The `rcpputils/env.hpp` header provides functionality to lookup the value of a provided environment variable through the `rcpputils::get_env_var(const char *)` function and set/un-set the value of a named, process-scoped environment variable through the `rcpputils::set_env_var(const char *, const char *)` function.

## Scope guard support {#scope-guard-support}
Support for a general-purpose scope guard is provided in the `rcpputils/scope_exit.hpp` header.

This utility provides a convenient tool for resource management when exception safety is an issue. It also facilitates the placement of cleanup code next to resource intialization code, improving ease of maintainability. The header provides the functions:

* `rcpputils::make_scope_exit(CallableT &&)`: Create a scope guard with a callable object containing resource cleanup/release code.
* `rcpputils::scope_exit::cancel()`: Makes the scope guard inactive.

Expected use:
```c++
auto resource_handle = acquire_resource();

// `make_scope_exit` creates a scope guard that will call
// `release_resource(resource_handle)` once the scope guard
// goes out of scope.
auto cleanup_resource_handle = rcpputils::make_scope_exit(
    [resource_handle]() {
        release_resource(resource_handle);
    });

// Use the resource.
...

// At this point, the resource may be released.
release_resource(resource);
// Since the resource has been released, cancel the "cleanup code" in the scope guard.
cleanup_resource_handle.cancel();
```

## `rcppmath` - Mathematical utilities {#rcppmath-mathematical-utilities}
### Clamping functionality {#clamping-functionality}
`rcppmath/clamp.hpp` provides functionality to perform clamping - which restricts a value between two bounds.
The `rcppmath::clamp()` function is overloaded as follows:

* `rcppmath::clamp(const T &, const T &, const T &)`: Takes a value to clamp, and the lower and upper boundaries to clamp against.
* `rcppmath::clamp(const T &, const T &, const T &, Compare)`: In addition to the previous signature, accepts a comparison object that returns `true` if its first argument is less than the second.

### Rolling mean accumulator {#rolling-mean-accumulator}
The `rcppmath/rolling_mean_accumulator.hpp` facilitates computing the rolling mean of a window of accumulated items.
The `rcppmath::RollingMeanAccumulator` can be constructed with an unsigned integral `rolling_window_size` value.
Values can be accumulated and the rolling mean can be obtained through the `rcppmath::RollingMeanAccumulator::accumulate(T)` method and the `rcppmath::RollingMeanAccumulator::getRollingMean()` methods respectively.
