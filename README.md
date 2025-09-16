# dynamic_complex.h

[![Version](https://img.shields.io/badge/version-v0.1.0-blue.svg)](https://github.com/edadma/dynamic_complex.h/releases)
[![Language](https://img.shields.io/badge/language-C11-blue.svg)](https://en.cppreference.com/w/c/11)
[![License](https://img.shields.io/badge/license-MIT%20OR%20Unlicense-green.svg)](#license)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS%20%7C%20MCU-lightgrey.svg)](#platform-support)
[![Tests](https://img.shields.io/badge/tests-25%20passing-brightgreen.svg)](#testing)

A single-header C library implementing arbitrary precision complex numbers with reference counting. Supports three distinct complex number types: integer (Gaussian), rational, and floating-point complexes.

## Features

- **Three Complex Types**: Gaussian integers, rational complex, and floating-point complex
- **Single Header**: Easy integration - just include `dynamic_complex.h`
- **Arbitrary Precision**: Built on dynamic_int.h and dynamic_fraction.h for exact arithmetic
- **Reference Counting**: Atomic memory management with configurable thread safety
- **Separate APIs**: Distinct function sets for each type (`dc_int_*`, `dc_frac_*`, `dc_double_*`)
- **Type Conversions**: Lossless upward conversions, rounding downward conversions
- **C99 Complex Integration**: Full transcendental function support for floating-point complex
- **Mathematical String Format**: Clean output like "3+4i", "2-3i", "i", "-i"
- **Fail-Fast Error Handling**: Assertions on invalid inputs for immediate bug detection
- **Comprehensive Testing**: 25 test cases with 100% function coverage

## Quick Start

First, ensure you have the required dependencies available in your include path:
- `dynamic_int.h`
- `dynamic_fraction.h`

Then include and use the library:

```c
// Include dependencies first (with their implementations if needed)
#define DI_IMPLEMENTATION  // if using dynamic_int.h directly
#include "dynamic_int.h"
#define DF_IMPLEMENTATION  // if using dynamic_fraction.h directly
#include "dynamic_fraction.h"

// Now include dynamic_complex.h
#define DC_IMPLEMENTATION
#include "dynamic_complex.h"

int main() {
    // Integer complex (Gaussian integers)
    dc_complex_int a = dc_int_from_ints(3, 4);    // 3 + 4i
    dc_complex_int b = dc_int_from_ints(1, -2);   // 1 - 2i
    dc_complex_int sum = dc_int_add(a, b);        // 4 + 2i

    // Rational complex (exact fractions)
    dc_complex_frac c = dc_frac_from_ints(1, 2, 3, 4);  // 1/2 + 3/4i
    dc_complex_frac d = dc_frac_from_ints(1, 3, 1, 6);  // 1/3 + 1/6i
    dc_complex_frac frac_sum = dc_frac_add(c, d);       // 5/6 + 11/12i

    // Floating-point complex (with transcendental functions)
    dc_complex_double e = dc_double_from_doubles(1.0, 0.0);
    dc_complex_double f = dc_double_exp(dc_double_from_doubles(0.0, M_PI));  // e^(iπ) = -1

    // Type conversions
    dc_complex_frac exact = dc_int_to_frac(a);      // Lossless: int → frac
    dc_complex_double approx = dc_frac_to_double(c); // Convert: frac → double

    // Division: integer division returns exact rational result
    dc_complex_frac quotient = dc_int_div(a, b);    // Exact rational division

    // String output
    char* str = dc_int_to_string(sum);
    printf("Sum: %s\n", str);  // "4+2i"
    free(str);

    // Cleanup (reference counting)
    dc_int_release(&a);
    dc_int_release(&b);
    dc_int_release(&sum);
    dc_frac_release(&c);
    dc_frac_release(&d);
    dc_frac_release(&frac_sum);
    dc_double_release(&e);
    dc_double_release(&f);
    dc_frac_release(&exact);
    dc_double_release(&approx);
    dc_frac_release(&quotient);

    return 0;
}
```

## Complex Types

### Integer Complex (`dc_complex_int`)
- **Purpose**: Gaussian integers (complex numbers with integer real and imaginary parts)
- **Backend**: Uses dynamic_int.h for arbitrary precision
- **Operations**: Exact arithmetic, division returns rational result
- **Example**: `3 + 4i`, `-7 + 2i`

### Rational Complex (`dc_complex_frac`)
- **Purpose**: Complex numbers with rational real and imaginary parts
- **Backend**: Uses dynamic_fraction.h for exact fraction arithmetic
- **Operations**: Exact rational arithmetic, automatic reduction to lowest terms
- **Example**: `1/2 + 3/4i`, `-2/3 + 5/7i`

### Floating-Point Complex (`dc_complex_double`)
- **Purpose**: IEEE 754 floating-point complex numbers
- **Backend**: Uses C99 complex.h for transcendental functions
- **Operations**: Standard complex arithmetic plus exp, log, sin, cos, etc.
- **Example**: `1.5 + 2.7i`, `3.14159 - 1.41421i`

## API Design

### Separate Function Families
Each complex type has its own complete API:

```c
// Integer complex functions
dc_complex_int dc_int_from_ints(int64_t real, int64_t imag);
dc_complex_int dc_int_add(dc_complex_int a, dc_complex_int b);
dc_complex_frac dc_int_div(dc_complex_int a, dc_complex_int b);  // Returns fraction!

// Rational complex functions
dc_complex_frac dc_frac_from_ints(int64_t r_num, int64_t r_den, int64_t i_num, int64_t i_den);
dc_complex_frac dc_frac_mul(dc_complex_frac a, dc_complex_frac b);
bool dc_frac_is_gaussian_int(dc_complex_frac c);  // Check if it's really an integer

// Floating-point complex functions
dc_complex_double dc_double_from_polar(double magnitude, double angle);
dc_complex_double dc_double_exp(dc_complex_double c);  // All C99 complex.h functions
double dc_double_abs(dc_complex_double c);  // Magnitude
```

### Type Conversions
```c
// Upward conversions (lossless)
dc_complex_frac dc_int_to_frac(dc_complex_int c);
dc_complex_double dc_int_to_double(dc_complex_int c);
dc_complex_double dc_frac_to_double(dc_complex_frac c);

// Downward conversions (with rounding)
dc_complex_int dc_frac_to_int(dc_complex_frac c);      // Rounds to nearest Gaussian integer
dc_complex_int dc_double_to_int(dc_complex_double c);   // Rounds both components
dc_complex_frac dc_double_to_frac(dc_complex_double c, int64_t max_denominator);
```

## Configuration

```c
// Memory management
#define DC_MALLOC custom_malloc
#define DC_FREE custom_free
#define DC_ASSERT custom_assert

// Thread safety (requires C11)
#define DC_ATOMIC_REFCOUNT 1

// Static linking
#define DC_STATIC

#define DC_IMPLEMENTATION
#include "dynamic_complex.h"
```

## Dependencies

### Required Dependencies
You must have these header files available in your include path:

- **dynamic_int.h**: Required for integer complex arithmetic
  - Download from: https://github.com/edadma/dynamic_int.h
- **dynamic_fraction.h**: Required for rational complex arithmetic
  - Download from: https://github.com/edadma/dynamic_fraction.h
- **C99 complex.h**: Used for floating-point transcendental functions (standard library)

### Development Dependencies (optional)
- **Unity**: Testing framework (included in devDeps/ for development)

### Dependency Setup

You have several options for managing dependencies:

1. **Manual Download**: Download the header files and place them in your include path
2. **Git Submodules**: Add dependencies as git submodules
3. **Package Manager**: Use a C package manager like CPM or Conan
4. **System Install**: Install headers system-wide
5. **CMake Include Directories**: Add dependency paths to your CMakeLists.txt

Example with git submodules:
```bash
git submodule add https://github.com/edadma/dynamic_int.h.git deps/dynamic_int
git submodule add https://github.com/edadma/dynamic_fraction.h.git deps/dynamic_fraction
```

Example CMakeLists.txt setup:
```cmake
include_directories(deps/dynamic_int.h)
include_directories(deps/dynamic_fraction.h)
```

## Building

```bash
# Build the test suite
mkdir build && cd build
cmake ..
make

# Run tests
./tests

# All 25 tests should pass with 100% function coverage
```

### Test Organization

The test suite is organized into logical groups:

1. **Integer Complex Tests** (6 test functions)
   - Basic functionality, arithmetic, memory management, missing functions coverage

2. **Rational Complex Tests** (7 test functions)
   - Creation, arithmetic, memory management, comparisons, string formatting

3. **Floating-Point Complex Tests** (8 test functions)
   - Creation, arithmetic, transcendentals, special values, comprehensive coverage

4. **Type Conversion Tests** (2 test functions)
   - All conversion paths between the three types

5. **Edge Cases and Formatting** (2 test functions)
   - Mathematical identities, string formatting, boundary conditions

Each test function thoroughly exercises multiple related functions to ensure complete coverage and mathematical correctness.

## Integration with Interpreters

This library is designed for language interpreters with separate numeric types:

```c
// Interpreter can map types directly:
typedef enum {
    TYPE_INT_COMPLEX,     // -> dc_complex_int
    TYPE_RATIONAL_COMPLEX, // -> dc_complex_frac
    TYPE_FLOAT_COMPLEX    // -> dc_complex_double
} complex_type_t;

// Type promotion handled by interpreter
dc_complex_frac promote_int_to_frac(dc_complex_int c) {
    return dc_int_to_frac(c);  // Exact conversion
}
```

## Mathematical Properties

- **Gaussian Integer Division**: `dc_int_div()` returns exact rational results
- **Automatic Reduction**: Rational complex numbers kept in lowest terms
- **Sign Normalization**: Denominators always positive, sign in numerator
- **IEEE 754 Compliance**: Floating-point complex follows C99 standard
- **String Format**: Mathematical notation ("3+4i", "2-3i", "i", "-i")

## Error Handling

The library uses a **fail-fast** approach:
- NULL pointer assertions (except in release functions)
- Memory allocation failure assertions
- Division by zero assertions
- All errors provide descriptive messages

## Performance Features

- **Cached Constants**: Singleton objects for 0, 1, i, -1, -i
- **Reference Counting**: Efficient memory sharing
- **Atomic Operations**: Optional thread-safe reference counting
- **C99 Integration**: Hardware-accelerated transcendental functions

## Testing

Comprehensive test suite with 25 test cases achieving **100% function coverage**:

### Integer Complex Tests (dc_int_*)
- Creation functions (`dc_int_from_ints`, `dc_int_from_di`, constants)
- Complete arithmetic operations (add, sub, mul, div, negate, conjugate)
- Memory management (retain, release, copy)
- Comparison predicates and string formatting
- Edge cases and mathematical identities

### Rational Complex Tests (dc_frac_*)
- Creation from integers and fractions (`dc_frac_from_ints`, `dc_frac_from_df`)
- Complete arithmetic suite (add, sub, mul, div, negate, conjugate, reciprocal)
- Gaussian integer detection predicate
- Memory management and equality testing
- String representation with fraction notation

### Floating-Point Complex Tests (dc_double_*)
- Creation from Cartesian and polar coordinates
- Basic and complete arithmetic operations
- **Full transcendental function suite**: exp, log, pow, sqrt, sin, cos, tan, sinh, cosh, tanh
- Special value detection (NaN, infinity)
- Memory management and string formatting
- Mathematical identity verification (e^(iπ/2) = i, sqrt(-1) = i)

### Type Conversion Tests
- Lossless upward conversions (int→frac→double)
- Lossy downward conversions with rounding
- Exact and approximate fraction conversion from doubles
- Conversion accuracy validation

### Comprehensive Coverage
- **All 65+ functions tested** across three complex types
- Memory management (reference counting, retain/release, copy)
- Mathematical correctness validation
- String formatting for all representations
- Edge cases, special values, and error conditions
- Cross-type operations and conversions

## License

Dual licensed under your choice of:
- [MIT License](LICENSE)
- [Unlicense (Public Domain)](LICENSE)

Choose the license that best fits your project's needs.

## Contributing

This library follows strict design principles:
- Fail-fast error handling
- Comprehensive testing required
- Mathematical correctness over convenience
