/**
 * @file dynamic_fraction.h
 * @brief Reference-counted arbitrary precision rational number library
 * @version 1.0.0
 * @date September 2025
 *
 * Single header library for arbitrary precision rational numbers (fractions)
 * with reference counting. Built on top of dynamic_int.h for arbitrary
 * precision integer arithmetic.
 *
 * @section license License
 *
 * Dual licensed under MIT OR Unlicense. See LICENSE file for details.
 * Choose the license that best fits your project's needs.
 *
 * @section config Configuration
 *
 * Customize the library by defining these macros before including:
 *
 * @code
 * #define DF_MALLOC malloc         // custom allocator
 * #define DF_FREE free             // custom deallocator
 * #define DF_ASSERT assert         // custom assert macro
 *
 * #define DF_IMPLEMENTATION
 * #include "dynamic_fraction.h"
 * @endcode
 *
 * @section usage Basic Usage
 *
 * @code
 * df_frac a = df_from_ints(3, 4);     // 3/4
 * df_frac b = df_from_ints(2, 3);     // 2/3
 * df_frac sum = df_add(a, b);         // 17/12
 *
 * double result = df_to_double(sum);
 * printf("Sum: %f\n", result);
 *
 * df_release(&a);
 * df_release(&b);
 * df_release(&sum);
 * @endcode
 */

#ifndef DYNAMIC_FRACTION_H
#define DYNAMIC_FRACTION_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <string.h>

// Include dynamic_int.h for arbitrary precision integers
#include "devDeps/dynamic_int.h"

/* Configuration macros */
#ifndef DF_MALLOC
#define DF_MALLOC malloc
#endif

#ifndef DF_FREE
#define DF_FREE free
#endif

#ifndef DF_ASSERT
#define DF_ASSERT assert
#endif

// API macros
#ifdef DF_STATIC
#define DF_DEF static
#define DF_IMPL static
#else
#define DF_DEF extern
#define DF_IMPL /* nothing - default linkage */
#endif

// ============================================================================
// INTERFACE
// ============================================================================

/**
 * @struct df_frac_internal
 * @brief Internal structure for a rational number
 *
 * Represents a fraction as numerator/denominator with reference counting.
 * The fraction is always kept in reduced form (lowest terms).
 */
struct df_frac_internal {
    di_int numerator;    /**< Numerator (can be negative) */
    di_int denominator;  /**< Denominator (always positive) */
    size_t ref_count;    /**< Reference count for memory management */
};

/**
 * @typedef df_frac
 * @brief Opaque pointer to a rational number
 */
typedef struct df_frac_internal* df_frac;

/**
 * @defgroup lifecycle Lifecycle Management
 * @brief Functions for creating and destroying fractions
 * @{
 */

/**
 * @brief Create a fraction from numerator and denominator
 * @param numerator Numerator value
 * @param denominator Denominator value (must not be 0)
 * @return New df_frac or NULL on failure
 *
 * The fraction is automatically reduced to lowest terms.
 */
DF_DEF df_frac df_from_ints(int64_t numerator, int64_t denominator);

/**
 * @brief Create a fraction from a di_int numerator and denominator
 * @param numerator Numerator as di_int
 * @param denominator Denominator as di_int (must not be 0)
 * @return New df_frac or NULL on failure
 */
DF_DEF df_frac df_from_di(di_int numerator, di_int denominator);

/**
 * @brief Create a fraction from an integer
 * @param value Integer value
 * @return New df_frac representing value/1
 */
DF_DEF df_frac df_from_int(int64_t value);

/**
 * @brief Create a fraction from a double
 * @param value Double value
 * @param max_denominator Maximum denominator for approximation
 * @return New df_frac approximating the double value
 */
DF_DEF df_frac df_from_double(double value, int64_t max_denominator);

/**
 * @brief Create a copy of a fraction
 * @param f Fraction to copy (may be NULL)
 * @return New df_frac with same value
 */
DF_DEF df_frac df_copy(df_frac f);

/**
 * @brief Increase reference count
 * @param f Fraction to retain (may be NULL)
 * @return The same fraction pointer
 */
DF_DEF df_frac df_retain(df_frac f);

/**
 * @brief Decrease reference count and free if zero
 * @param f Pointer to fraction (will be set to NULL)
 */
DF_DEF void df_release(df_frac* f);

/** @} */ // end of lifecycle

/**
 * @defgroup arithmetic Arithmetic Operations
 * @brief Basic arithmetic operations on fractions
 * @{
 */

/**
 * @brief Add two fractions
 * @param a First fraction
 * @param b Second fraction
 * @return New df_frac with a + b
 */
DF_DEF df_frac df_add(df_frac a, df_frac b);

/**
 * @brief Subtract two fractions
 * @param a First fraction
 * @param b Second fraction
 * @return New df_frac with a - b
 */
DF_DEF df_frac df_sub(df_frac a, df_frac b);

/**
 * @brief Multiply two fractions
 * @param a First fraction
 * @param b Second fraction
 * @return New df_frac with a * b
 */
DF_DEF df_frac df_mul(df_frac a, df_frac b);

/**
 * @brief Divide two fractions
 * @param a First fraction
 * @param b Second fraction (must not be zero)
 * @return New df_frac with a / b
 */
DF_DEF df_frac df_div(df_frac a, df_frac b);

/**
 * @brief Negate a fraction
 * @param f Fraction to negate
 * @return New df_frac with -f
 */
DF_DEF df_frac df_negate(df_frac f);

/**
 * @brief Get absolute value
 * @param f Fraction
 * @return New df_frac with |f|
 */
DF_DEF df_frac df_abs(df_frac f);

/**
 * @brief Get reciprocal (1/f)
 * @param f Fraction (must not be zero)
 * @return New df_frac with 1/f
 */
DF_DEF df_frac df_reciprocal(df_frac f);

/** @} */ // end of arithmetic

/**
 * @defgroup comparison Comparison Functions
 * @brief Functions for comparing fractions
 * @{
 */

/**
 * @brief Compare two fractions
 * @param a First fraction
 * @param b Second fraction
 * @return -1 if a < b, 0 if a == b, 1 if a > b
 */
DF_DEF int df_cmp(df_frac a, df_frac b);

/**
 * @brief Test equality
 * @param a First fraction
 * @param b Second fraction
 * @return true if a == b
 */
DF_DEF bool df_eq(df_frac a, df_frac b);

/**
 * @brief Test inequality
 * @param a First fraction
 * @param b Second fraction
 * @return true if a != b
 */
DF_DEF bool df_ne(df_frac a, df_frac b);

/**
 * @brief Test less than
 * @param a First fraction
 * @param b Second fraction
 * @return true if a < b
 */
DF_DEF bool df_lt(df_frac a, df_frac b);

/**
 * @brief Test less than or equal
 * @param a First fraction
 * @param b Second fraction
 * @return true if a <= b
 */
DF_DEF bool df_le(df_frac a, df_frac b);

/**
 * @brief Test greater than
 * @param a First fraction
 * @param b Second fraction
 * @return true if a > b
 */
DF_DEF bool df_gt(df_frac a, df_frac b);

/**
 * @brief Test greater than or equal
 * @param a First fraction
 * @param b Second fraction
 * @return true if a >= b
 */
DF_DEF bool df_ge(df_frac a, df_frac b);

/** @} */ // end of comparison

/**
 * @defgroup predicates Predicate Functions
 * @brief Functions for testing fraction properties
 * @{
 */

/**
 * @brief Test if fraction is zero
 * @param f Fraction to test
 * @return true if f == 0
 */
DF_DEF bool df_is_zero(df_frac f);

/**
 * @brief Test if fraction is one
 * @param f Fraction to test
 * @return true if f == 1
 */
DF_DEF bool df_is_one(df_frac f);

/**
 * @brief Test if fraction is negative
 * @param f Fraction to test
 * @return true if f < 0
 */
DF_DEF bool df_is_negative(df_frac f);

/**
 * @brief Test if fraction is positive
 * @param f Fraction to test
 * @return true if f > 0
 */
DF_DEF bool df_is_positive(df_frac f);

/**
 * @brief Test if fraction is an integer
 * @param f Fraction to test
 * @return true if denominator == 1
 */
DF_DEF bool df_is_integer(df_frac f);

/** @} */ // end of predicates

/**
 * @defgroup conversion Conversion Functions
 * @brief Functions for converting to/from other types
 * @{
 */

/**
 * @brief Convert to double
 * @param f Fraction to convert
 * @return Double approximation of the fraction
 */
DF_DEF double df_to_double(df_frac f);

/**
 * @brief Convert to int64_t if possible
 * @param f Fraction to convert
 * @param result Pointer to store result
 * @return true if conversion successful (f is integer and fits in int64_t)
 */
DF_DEF bool df_to_int64(df_frac f, int64_t* result);

/**
 * @brief Convert to string
 * @param f Fraction to convert
 * @return Allocated string in format "num/den" or "num" if integer
 * @note Caller must free the returned string
 */
DF_DEF char* df_to_string(df_frac f);

/**
 * @brief Parse fraction from string
 * @param str String to parse (format: "num/den" or "num")
 * @return New df_frac or NULL on parse error
 */
DF_DEF df_frac df_from_string(const char* str);

/**
 * @brief Get numerator as di_int
 * @param f Fraction
 * @return Copy of numerator (caller must release)
 */
DF_DEF di_int df_numerator(df_frac f);

/**
 * @brief Get denominator as di_int
 * @param f Fraction
 * @return Copy of denominator (caller must release)
 */
DF_DEF di_int df_denominator(df_frac f);

/** @} */ // end of conversion

/**
 * @defgroup special Special Values
 * @brief Functions for creating special fraction values
 * @{
 */

/**
 * @brief Create zero fraction (0/1)
 * @return New df_frac representing 0
 */
DF_DEF df_frac df_zero(void);

/**
 * @brief Create one fraction (1/1)
 * @return New df_frac representing 1
 */
DF_DEF df_frac df_one(void);

/**
 * @brief Create negative one fraction (-1/1)
 * @return New df_frac representing -1
 */
DF_DEF df_frac df_neg_one(void);

/** @} */ // end of special

// ============================================================================
// EXTENDED OPERATIONS
// ============================================================================

/**
 * @defgroup extended Extended Mathematical Operations
 * @brief Additional mathematical functions for language interpreters
 * @{
 */

/**
 * @brief Raise fraction to integer power
 * @param base Base fraction
 * @param exponent Integer exponent
 * @return New df_frac with base^exponent
 * @since 1.1.0
 */
DF_DEF df_frac df_pow(df_frac base, int64_t exponent);

/**
 * @brief Floor function - greatest integer ≤ f
 * @param f Input fraction
 * @return New df_frac representing floor(f)
 * @since 1.1.0
 */
DF_DEF df_frac df_floor(df_frac f);

/**
 * @brief Ceiling function - smallest integer ≥ f
 * @param f Input fraction
 * @return New df_frac representing ceil(f)
 * @since 1.1.0
 */
DF_DEF df_frac df_ceil(df_frac f);

/**
 * @brief Truncate towards zero
 * @param f Input fraction
 * @return New df_frac with fractional part removed
 * @since 1.1.0
 */
DF_DEF df_frac df_trunc(df_frac f);

/**
 * @brief Round to nearest integer
 * @param f Input fraction
 * @return New df_frac rounded to nearest integer
 * @since 1.1.0
 */
DF_DEF df_frac df_round(df_frac f);

/**
 * @brief Get sign of fraction
 * @param f Input fraction
 * @return -1 if negative, 0 if zero, 1 if positive
 * @since 1.1.0
 */
DF_DEF int df_sign(df_frac f);

/**
 * @brief Minimum of two fractions
 * @param a First fraction
 * @param b Second fraction
 * @return New df_frac with smaller value
 * @since 1.1.0
 */
DF_DEF df_frac df_min(df_frac a, df_frac b);

/**
 * @brief Maximum of two fractions
 * @param a First fraction
 * @param b Second fraction
 * @return New df_frac with larger value
 * @since 1.1.0
 */
DF_DEF df_frac df_max(df_frac a, df_frac b);


/**
 * @brief Hash function for fractions
 * @param f Input fraction
 * @return Hash value suitable for hash tables
 * @since 1.1.0
 */
DF_DEF uint64_t df_hash(df_frac f);

/**
 * @brief Check if fraction fits in int32_t
 * @param f Input fraction
 * @return true if can be converted to int32_t without loss
 * @since 1.1.0
 */
DF_DEF bool df_fits_int32(df_frac f);

/**
 * @brief Check if fraction fits in int64_t
 * @param f Input fraction
 * @return true if can be converted to int64_t without loss
 * @since 1.1.0
 */
DF_DEF bool df_fits_int64(df_frac f);

/**
 * @brief Check if fraction fits in double without precision loss
 * @param f Input fraction
 * @return true if can be converted to double exactly
 * @since 1.1.0
 */
DF_DEF bool df_fits_double(df_frac f);

/**
 * @brief Get integer (whole) part of fraction
 * @param f Input fraction
 * @return New di_int with whole part
 * @since 1.1.0
 */
DF_DEF di_int df_whole_part(df_frac f);

/**
 * @brief Get fractional part of fraction
 * @param f Input fraction
 * @return New df_frac with only fractional part (0 ≤ result < 1)
 * @since 1.1.0
 */
DF_DEF df_frac df_fractional_part(df_frac f);

/** @} */ // end of extended

// ============================================================================
// IMPLEMENTATION
// ============================================================================

#ifdef DF_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

// Helper: Allocate a new fraction structure
static df_frac df_alloc(void) {
    df_frac f = (df_frac)DF_MALLOC(sizeof(struct df_frac_internal));
    DF_ASSERT(f && "df_alloc: memory allocation failed");
    f->numerator = NULL;
    f->denominator = NULL;
    f->ref_count = 1;
    return f;
}

// Helper: Reduce fraction to lowest terms
static void df_reduce(df_frac f) {
    DF_ASSERT(f && "df_reduce: fraction cannot be NULL");
    DF_ASSERT(f->numerator && "df_reduce: numerator cannot be NULL");
    DF_ASSERT(f->denominator && "df_reduce: denominator cannot be NULL");

    // Get GCD of numerator and denominator
    di_int gcd = di_gcd(f->numerator, f->denominator);
    if (!gcd || di_is_one(gcd)) {
        di_release(&gcd);
        return;
    }

    // Divide both by GCD
    di_int new_num = di_div(f->numerator, gcd);
    di_int new_den = di_div(f->denominator, gcd);

    di_release(&f->numerator);
    di_release(&f->denominator);
    f->numerator = new_num;
    f->denominator = new_den;

    di_release(&gcd);
}

// Helper: Ensure denominator is positive (move sign to numerator)
static void df_normalize_sign(df_frac f) {
    DF_ASSERT(f && "df_normalize_sign: fraction cannot be NULL");
    DF_ASSERT(f->denominator && "df_normalize_sign: denominator cannot be NULL");

    if (di_is_negative(f->denominator)) {
        di_int neg_num = di_negate(f->numerator);
        di_int neg_den = di_negate(f->denominator);

        di_release(&f->numerator);
        di_release(&f->denominator);
        f->numerator = neg_num;
        f->denominator = neg_den;
    }
}

// Create fraction from int64 values
DF_IMPL df_frac df_from_ints(int64_t numerator, int64_t denominator) {
    DF_ASSERT(denominator != 0 && "df_from_ints: denominator cannot be zero");

    df_frac f = df_alloc();
    DF_ASSERT(f && "df_from_ints: allocation failed");

    f->numerator = di_from_int64(numerator);
    f->denominator = di_from_int64(denominator);

    if (!f->numerator || !f->denominator) {
        di_release(&f->numerator);
        di_release(&f->denominator);
        DF_FREE(f);
        return NULL;
    }

    df_normalize_sign(f);
    df_reduce(f);
    return f;
}

// Create fraction from di_int values
DF_IMPL df_frac df_from_di(di_int numerator, di_int denominator) {
    DF_ASSERT(numerator && "df_from_di: numerator cannot be NULL");
    DF_ASSERT(denominator && !di_is_zero(denominator) && "df_from_di: denominator cannot be zero");

    df_frac f = df_alloc();
    DF_ASSERT(f && "df_from_ints: allocation failed");

    f->numerator = di_retain(numerator);
    f->denominator = di_retain(denominator);

    df_normalize_sign(f);
    df_reduce(f);
    return f;
}

// Create fraction from integer
DF_IMPL df_frac df_from_int(int64_t value) {
    return df_from_ints(value, 1);
}

// Create fraction from double using continued fractions
DF_IMPL df_frac df_from_double(double value, int64_t max_denominator) {
    if (isnan(value) || isinf(value)) return NULL;
    if (max_denominator <= 0) max_denominator = INT64_MAX;

    // Handle negative values
    bool negative = value < 0;
    if (negative) value = -value;

    // Continued fraction algorithm
    int64_t h0 = 0, h1 = 1;
    int64_t k0 = 1, k1 = 0;
    double x = value;

    while (k1 <= max_denominator) {
        int64_t a = (int64_t)floor(x);
        int64_t h2 = a * h1 + h0;
        int64_t k2 = a * k1 + k0;

        if (k2 > max_denominator) break;

        h0 = h1; h1 = h2;
        k0 = k1; k1 = k2;

        if (fabs(value - (double)h1 / (double)k1) < 1e-15) break;

        x = 1.0 / (x - a);
        if (x > 1e15) break; // Avoid overflow
    }

    return df_from_ints(negative ? -h1 : h1, k1);
}

// Copy a fraction
DF_IMPL df_frac df_copy(df_frac f) {
    DF_ASSERT(f && "df_copy: fraction cannot be NULL");
    di_int num = di_retain(f->numerator);
    di_int den = di_retain(f->denominator);
    df_frac result = df_from_di(num, den);
    di_release(&num);
    di_release(&den);
    return result;
}

// Retain (increase reference count)
DF_IMPL df_frac df_retain(df_frac f) {
    DF_ASSERT(f && "df_retain: fraction cannot be NULL");
    f->ref_count++;
    return f;
}

// Release (decrease reference count)
DF_IMPL void df_release(df_frac* f) {
    if (!f || !*f) return;

    (*f)->ref_count--;
    if ((*f)->ref_count == 0) {
        di_release(&(*f)->numerator);
        di_release(&(*f)->denominator);
        DF_FREE(*f);
    }
    *f = NULL;
}

// Addition: a/b + c/d = (ad + bc) / bd
DF_IMPL df_frac df_add(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_add: first operand cannot be NULL");
    DF_ASSERT(b && "df_add: second operand cannot be NULL");

    // Calculate ad and bc
    di_int ad = di_mul(a->numerator, b->denominator);
    di_int bc = di_mul(b->numerator, a->denominator);
    di_int num = di_add(ad, bc);

    // Calculate bd
    di_int den = di_mul(a->denominator, b->denominator);

    df_frac result = df_from_di(num, den);

    di_release(&ad);
    di_release(&bc);
    di_release(&num);
    di_release(&den);

    return result;
}

// Subtraction: a/b - c/d = (ad - bc) / bd
DF_IMPL df_frac df_sub(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_sub: first operand cannot be NULL");
    DF_ASSERT(b && "df_sub: second operand cannot be NULL");

    // Calculate ad and bc
    di_int ad = di_mul(a->numerator, b->denominator);
    di_int bc = di_mul(b->numerator, a->denominator);
    di_int num = di_sub(ad, bc);

    // Calculate bd
    di_int den = di_mul(a->denominator, b->denominator);

    df_frac result = df_from_di(num, den);

    di_release(&ad);
    di_release(&bc);
    di_release(&num);
    di_release(&den);

    return result;
}

// Multiplication: (a/b) * (c/d) = ac / bd
DF_IMPL df_frac df_mul(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_mul: first operand cannot be NULL");
    DF_ASSERT(b && "df_mul: second operand cannot be NULL");

    di_int num = di_mul(a->numerator, b->numerator);
    di_int den = di_mul(a->denominator, b->denominator);

    df_frac result = df_from_di(num, den);

    di_release(&num);
    di_release(&den);

    return result;
}

// Division: (a/b) / (c/d) = ad / bc
DF_IMPL df_frac df_div(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_div: dividend cannot be NULL");
    DF_ASSERT(b && "df_div: divisor cannot be NULL");
    DF_ASSERT(!df_is_zero(b) && "df_div: division by zero");

    di_int num = di_mul(a->numerator, b->denominator);
    di_int den = di_mul(a->denominator, b->numerator);

    df_frac result = df_from_di(num, den);

    di_release(&num);
    di_release(&den);

    return result;
}

// Negate
DF_IMPL df_frac df_negate(df_frac f) {
    DF_ASSERT(f && "df_negate: operand cannot be NULL");

    di_int neg_num = di_negate(f->numerator);
    di_int den = di_retain(f->denominator);
    df_frac result = df_from_di(neg_num, den);
    di_release(&neg_num);
    di_release(&den);

    return result;
}

// Absolute value
DF_IMPL df_frac df_abs(df_frac f) {
    DF_ASSERT(f && "df_abs: operand cannot be NULL");

    di_int abs_num = di_abs(f->numerator);
    di_int den = di_retain(f->denominator);
    df_frac result = df_from_di(abs_num, den);
    di_release(&abs_num);
    di_release(&den);

    return result;
}

// Reciprocal
DF_IMPL df_frac df_reciprocal(df_frac f) {
    DF_ASSERT(f && "df_reciprocal: operand cannot be NULL");
    DF_ASSERT(!df_is_zero(f) && "df_reciprocal: reciprocal of zero");

    di_int num = di_retain(f->denominator);
    di_int den = di_retain(f->numerator);
    df_frac result = df_from_di(num, den);
    di_release(&num);
    di_release(&den);
    return result;
}

// Compare two fractions
DF_IMPL int df_cmp(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_cmp: first operand cannot be NULL");
    DF_ASSERT(b && "df_cmp: second operand cannot be NULL");

    // Compare a/b with c/d by comparing ad with bc
    di_int ad = di_mul(a->numerator, b->denominator);
    di_int bc = di_mul(b->numerator, a->denominator);

    int result = di_compare(ad, bc);

    di_release(&ad);
    di_release(&bc);

    return result;
}

// Equality test
DF_IMPL bool df_eq(df_frac a, df_frac b) {
    return df_cmp(a, b) == 0;
}

// Inequality test
DF_IMPL bool df_ne(df_frac a, df_frac b) {
    return df_cmp(a, b) != 0;
}

// Less than
DF_IMPL bool df_lt(df_frac a, df_frac b) {
    return df_cmp(a, b) < 0;
}

// Less than or equal
DF_IMPL bool df_le(df_frac a, df_frac b) {
    return df_cmp(a, b) <= 0;
}

// Greater than
DF_IMPL bool df_gt(df_frac a, df_frac b) {
    return df_cmp(a, b) > 0;
}

// Greater than or equal
DF_IMPL bool df_ge(df_frac a, df_frac b) {
    return df_cmp(a, b) >= 0;
}

// Test if zero
DF_IMPL bool df_is_zero(df_frac f) {
    DF_ASSERT(f && "df_is_zero: operand cannot be NULL");
    return di_is_zero(f->numerator);
}

// Test if one
DF_IMPL bool df_is_one(df_frac f) {
    DF_ASSERT(f && "df_is_one: operand cannot be NULL");
    return di_eq(f->numerator, f->denominator);
}

// Test if negative
DF_IMPL bool df_is_negative(df_frac f) {
    DF_ASSERT(f && "df_is_negative: operand cannot be NULL");
    return di_is_negative(f->numerator);
}

// Test if positive
DF_IMPL bool df_is_positive(df_frac f) {
    DF_ASSERT(f && "df_is_positive: operand cannot be NULL");
    return !di_is_negative(f->numerator) && !di_is_zero(f->numerator);
}

// Test if integer
DF_IMPL bool df_is_integer(df_frac f) {
    DF_ASSERT(f && "df_is_integer: operand cannot be NULL");
    return di_is_one(f->denominator);
}

// Convert to double
DF_IMPL double df_to_double(df_frac f) {
    DF_ASSERT(f && "df_to_double: operand cannot be NULL");

    double num = di_to_double(f->numerator);
    double den = di_to_double(f->denominator);

    return num / den;
}

// Convert to int64 if possible
DF_IMPL bool df_to_int64(df_frac f, int64_t* result) {
    DF_ASSERT(f && "df_to_int64: fraction cannot be NULL");
    DF_ASSERT(result && "df_to_int64: result pointer cannot be NULL");
    if (!df_is_integer(f)) return false;

    return di_to_int64(f->numerator, result);
}

// Convert to string
DF_IMPL char* df_to_string(df_frac f) {
    DF_ASSERT(f && "df_to_string: operand cannot be NULL");

    char* num_str = di_to_string(f->numerator, 10);
    DF_ASSERT(num_str && "df_to_string: numerator string conversion failed");

    // If integer, just return the numerator string
    if (df_is_integer(f)) {
        return num_str;
    }

    char* den_str = di_to_string(f->denominator, 10);
    if (!den_str) {
        free(num_str);
        return NULL;
    }

    // Allocate space for "num/den"
    size_t len = strlen(num_str) + strlen(den_str) + 2;
    char* result = (char*)DF_MALLOC(len);
    DF_ASSERT(result && "df_to_string: result allocation failed");

    snprintf(result, len, "%s/%s", num_str, den_str);

    free(num_str);
    free(den_str);

    return result;
}

// Parse from string
DF_IMPL df_frac df_from_string(const char* str) {
    DF_ASSERT(str && "df_from_string: string cannot be NULL");

    // Look for '/' separator
    const char* slash = strchr(str, '/');

    if (!slash) {
        // No slash, parse as integer
        di_int num = di_from_string(str, 10);
        DF_ASSERT(num && "df_from_string: numerator parsing failed");

        di_int den = di_one();
        df_frac result = df_from_di(num, den);

        di_release(&num);
        di_release(&den);

        return result;
    }

    // Parse numerator and denominator
    size_t num_len = slash - str;
    char* num_str = (char*)DF_MALLOC(num_len + 1);
    DF_ASSERT(num_str && "df_from_string: numerator string allocation failed");

    strncpy(num_str, str, num_len);
    num_str[num_len] = '\0';

    di_int num = di_from_string(num_str, 10);
    DF_FREE(num_str);

    DF_ASSERT(num && "df_from_string: numerator parsing failed");

    di_int den = di_from_string(slash + 1, 10);
    DF_ASSERT(den && "df_from_string: denominator parsing failed");
    DF_ASSERT(!di_is_zero(den) && "df_from_string: denominator cannot be zero");

    df_frac result = df_from_di(num, den);

    di_release(&num);
    di_release(&den);

    return result;
}

// Get numerator
DF_IMPL di_int df_numerator(df_frac f) {
    DF_ASSERT(f && "df_numerator: operand cannot be NULL");
    return di_retain(f->numerator);
}

// Get denominator
DF_IMPL di_int df_denominator(df_frac f) {
    DF_ASSERT(f && "df_denominator: operand cannot be NULL");
    return di_retain(f->denominator);
}

// Create zero
DF_IMPL df_frac df_zero(void) {
    return df_from_ints(0, 1);
}

// Create one
DF_IMPL df_frac df_one(void) {
    return df_from_ints(1, 1);
}

// Create negative one
DF_IMPL df_frac df_neg_one(void) {
    return df_from_ints(-1, 1);
}

// ============================================================================
// Extended Functions Implementation
// ============================================================================

// Power function
DF_IMPL df_frac df_pow(df_frac base, int64_t exponent) {
    DF_ASSERT(base && "df_pow: base cannot be NULL");

    if (exponent == 0) {
        return df_one();
    }

    if (exponent == 1) {
        return df_copy(base);
    }

    if (df_is_zero(base)) {
        DF_ASSERT(exponent > 0 && "df_pow: zero to negative power is undefined");
        return df_zero();
    }

    // Handle negative exponents
    if (exponent < 0) {
        df_frac reciprocal = df_reciprocal(base);
        df_frac result = df_pow(reciprocal, -exponent);
        df_release(&reciprocal);
        return result;
    }

    // Positive integer exponent - use exponentiation by squaring
    df_frac result = df_one();
    df_frac current_base = df_copy(base);

    while (exponent > 0) {
        if (exponent & 1) {  // If exponent is odd
            df_frac new_result = df_mul(result, current_base);
            df_release(&result);
            result = new_result;
        }
        exponent >>= 1;
        if (exponent > 0) {
            df_frac new_base = df_mul(current_base, current_base);
            df_release(&current_base);
            current_base = new_base;
        }
    }

    df_release(&current_base);
    return result;
}

// Floor function
DF_IMPL df_frac df_floor(df_frac f) {
    DF_ASSERT(f && "df_floor: operand cannot be NULL");

    if (df_is_integer(f)) {
        return df_copy(f);
    }

    // di_div already performs floor division (rounds toward negative infinity)
    di_int quotient = di_div(f->numerator, f->denominator);

    di_int one = di_one();
    df_frac result = df_from_di(quotient, one);
    di_release(&quotient);
    di_release(&one);
    return result;
}

// Ceiling function
DF_IMPL df_frac df_ceil(df_frac f) {
    DF_ASSERT(f && "df_ceil: operand cannot be NULL");

    if (df_is_integer(f)) {
        return df_copy(f);
    }

    // Get integer part (floor)
    di_int quotient = di_div(f->numerator, f->denominator);

    // For non-integers, ceiling is always floor + 1
    di_int one = di_one();
    di_int adjusted = di_add(quotient, one);
    di_release(&quotient);

    df_frac result = df_from_di(adjusted, one);
    di_release(&adjusted);
    di_release(&one);
    return result;
}

// Truncate function
DF_IMPL df_frac df_trunc(df_frac f) {
    DF_ASSERT(f && "df_trunc: operand cannot be NULL");

    if (df_is_integer(f)) {
        return df_copy(f);
    }

    // Truncate toward zero - same as whole_part
    di_int whole_int = df_whole_part(f);
    di_int one = di_one();
    df_frac result = df_from_di(whole_int, one);
    di_release(&whole_int);
    di_release(&one);
    return result;
}

// Round function
DF_IMPL df_frac df_round(df_frac f) {
    DF_ASSERT(f && "df_round: operand cannot be NULL");

    if (df_is_integer(f)) {
        return df_copy(f);
    }

    // Round to nearest integer, ties to even (banker's rounding)
    df_frac half = df_from_ints(1, 2);
    df_frac abs_frac_part = df_abs(df_fractional_part(f));

    // Check if fractional part is exactly 0.5
    if (df_eq(abs_frac_part, half)) {
        // Tie case - round to even
        di_int whole_part = df_whole_part(f);
        di_int one_den = di_one();
        df_frac whole_frac = df_from_di(whole_part, one_den);
        di_release(&whole_part);
        di_release(&one_den);

        // Check if whole part is even
        di_int whole = df_whole_part(f);
        int64_t whole_val;
        di_to_int64(whole, &whole_val);

        if (whole_val % 2 == 0) {
            // Already even, return whole part
            df_release(&half);
            df_release(&abs_frac_part);
            di_release(&whole);
            return whole_frac;
        } else {
            // Odd, round to next even number
            di_int one = di_one();
            di_int adjusted_whole;

            if (df_is_negative(f)) {
                adjusted_whole = di_sub(whole, one);
            } else {
                adjusted_whole = di_add(whole, one);
            }

            di_int one_den = di_one();
            df_frac result = df_from_di(adjusted_whole, one_den);
            di_release(&one_den);

            df_release(&half);
            df_release(&abs_frac_part);
            df_release(&whole_frac);
            di_release(&whole);
            di_release(&one);
            di_release(&adjusted_whole);
            return result;
        }
    } else {
        // Normal rounding - not a tie
        df_frac signed_half;
        if (df_is_negative(f)) {
            signed_half = df_negate(half);
        } else {
            signed_half = df_copy(half);
        }

        df_frac adjusted = df_add(f, signed_half);
        df_frac result = df_trunc(adjusted);

        df_release(&half);
        df_release(&abs_frac_part);
        df_release(&signed_half);
        df_release(&adjusted);
        return result;
    }
}

// Sign function
DF_IMPL int df_sign(df_frac f) {
    DF_ASSERT(f && "df_sign: operand cannot be NULL");

    if (df_is_zero(f)) return 0;
    if (df_is_negative(f)) return -1;
    return 1;
}

// Min function
DF_IMPL df_frac df_min(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_min: first operand cannot be NULL");
    DF_ASSERT(b && "df_min: second operand cannot be NULL");

    return df_lt(a, b) ? df_copy(a) : df_copy(b);
}

// Max function
DF_IMPL df_frac df_max(df_frac a, df_frac b) {
    DF_ASSERT(a && "df_max: first operand cannot be NULL");
    DF_ASSERT(b && "df_max: second operand cannot be NULL");

    return df_gt(a, b) ? df_copy(a) : df_copy(b);
}


// Hash function
DF_IMPL uint64_t df_hash(df_frac f) {
    DF_ASSERT(f && "df_hash: operand cannot be NULL");

    // Simple hash combining numerator and denominator
    // Use a well-known hash combining formula
    uint64_t h1 = 0, h2 = 0;

    // Hash the numerator and denominator using their string representations
    char* num_str = di_to_string(f->numerator, 10);
    char* den_str = di_to_string(f->denominator, 10);

    // Simple string hash (djb2 algorithm)
    for (const char* s = num_str; *s; s++) {
        h1 = h1 * 33 + (uint8_t)*s;
    }
    for (const char* s = den_str; *s; s++) {
        h2 = h2 * 33 + (uint8_t)*s;
    }

    free(num_str);
    free(den_str);

    // Combine hashes
    return h1 ^ (h2 << 1);
}

// Type checking functions
DF_IMPL bool df_fits_int32(df_frac f) {
    DF_ASSERT(f && "df_fits_int32: operand cannot be NULL");

    if (!df_is_integer(f)) return false;

    int32_t dummy;
    return di_to_int32(f->numerator, &dummy);
}

DF_IMPL bool df_fits_int64(df_frac f) {
    DF_ASSERT(f && "df_fits_int64: operand cannot be NULL");

    if (!df_is_integer(f)) return false;

    int64_t dummy;
    return di_to_int64(f->numerator, &dummy);
}

DF_IMPL bool df_fits_double(df_frac f) {
    DF_ASSERT(f && "df_fits_double: operand cannot be NULL");

    // Convert to double and back, see if we get the same fraction
    double d = df_to_double(f);
    if (!isfinite(d)) return false;

    df_frac converted = df_from_double(d, 1000000); // Reasonable precision
    bool fits = df_eq(f, converted);
    df_release(&converted);

    return fits;
}

// Fraction parts
DF_IMPL di_int df_whole_part(df_frac f) {
    DF_ASSERT(f && "df_whole_part: operand cannot be NULL");

    // For truncation toward zero, we need to handle negative numbers differently
    // di_div floors toward negative infinity, but we want truncation toward zero
    di_int quotient = di_div(f->numerator, f->denominator);

    // If negative and not an exact division, add 1 to get truncation behavior
    if (df_is_negative(f) && !df_is_integer(f)) {
        di_int one = di_one();
        di_int adjusted = di_add(quotient, one);
        di_release(&quotient);
        di_release(&one);
        return adjusted;
    }

    return quotient;
}

DF_IMPL df_frac df_fractional_part(df_frac f) {
    DF_ASSERT(f && "df_fractional_part: operand cannot be NULL");

    if (df_is_integer(f)) {
        return df_zero();
    }

    // fractional_part = f - whole_part(f)
    // This preserves the sign: -2.333... - (-2) = -0.333...
    di_int whole_int = df_whole_part(f);
    di_int one = di_one();
    df_frac whole = df_from_di(whole_int, one);
    di_release(&whole_int);
    di_release(&one);

    df_frac result = df_sub(f, whole);
    df_release(&whole);

    return result;
}

#endif // DF_IMPLEMENTATION

#endif // DYNAMIC_FRACTION_H