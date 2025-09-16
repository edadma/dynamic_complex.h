# dynamic_complex.h

[![Version](https://img.shields.io/badge/version-v1.0.0-blue.svg)](https://github.com/edadma/dynamic_complex.h/releases)
[![Language](https://img.shields.io/badge/language-C11-blue.svg)](https://en.cppreference.com/w/c/11)
[![License](https://img.shields.io/badge/license-MIT%20OR%20Unlicense-green.svg)](#license)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS%20%7C%20MCU-lightgrey.svg)](#platform-support)
[![Tests](https://img.shields.io/badge/tests-14%20passing-brightgreen.svg)](#testing)

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
- **Comprehensive Testing**: 14 test cases covering all functionality

## Quick Start

```c
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

- **dynamic_int.h**: Required for integer complex arithmetic
- **dynamic_fraction.h**: Required for rational complex arithmetic
- **C99 complex.h**: Used for floating-point transcendental functions
- **Unity**: Testing framework (included in devDeps/)

## Building

```bash
# Build the test suite
mkdir build && cd build
cmake ..
make

# Run tests
./tests

# All 14 tests should pass
```

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

Comprehensive test suite with 14 test cases:
- Integer complex creation and arithmetic
- Rational complex with Gaussian integer detection
- Floating-point complex with transcendental functions
- Type conversions between all three types
- Memory management and reference counting
- String formatting and edge cases

## License

Dual licensed under your choice of:
- [MIT License](LICENSE)
- [Unlicense (Public Domain)](LICENSE)

Choose the license that best fits your project's needs.

## Contributing

This library follows strict design principles:
- Fail-fast error handling
- No AI attribution in commits
- Comprehensive testing required
- Mathematical correctness over convenience