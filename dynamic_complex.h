/**
 * @file dynamic_complex.h
 * @brief Reference-counted arbitrary precision complex number library
 * @version 1.0.0
 * @date September 2025
 *
 * Single header library for arbitrary precision complex numbers with reference counting.
 * Supports three distinct complex number types:
 * - dc_complex_int: Gaussian integers using dynamic_int
 * - dc_complex_frac: Rational complex numbers using dynamic_fraction
 * - dc_complex_double: Floating-point complex using C99 complex.h
 *
 * @section config Configuration
 *
 * Customize the library by defining these macros before including:
 *
 * @code
 * #define DC_MALLOC malloc         // custom allocator
 * #define DC_FREE free             // custom deallocator
 * #define DC_ASSERT assert         // custom assert macro
 * #define DC_ATOMIC_REFCOUNT 1     // enable atomic reference counting (requires C11)
 *
 * #define DC_IMPLEMENTATION
 * #include "dynamic_complex.h"
 * @endcode
 *
 * @section usage Basic Usage
 *
 * @code
 * // Integer complex
 * dc_complex_int a = dc_int_from_ints(3, 4);    // 3 + 4i
 * dc_complex_int b = dc_int_from_ints(1, -2);   // 1 - 2i
 * dc_complex_int sum = dc_int_add(a, b);        // 4 + 2i
 *
 * // Rational complex
 * dc_complex_frac c = dc_frac_from_ints(1, 2, 3, 4);  // 1/2 + 3/4i
 *
 * // Floating-point complex
 * dc_complex_double d = dc_double_from_doubles(1.5, 2.5);
 * dc_complex_double e = dc_double_exp(d);
 *
 * // Type conversions
 * dc_complex_frac exact = dc_int_to_frac(a);
 * dc_complex_double approx = dc_frac_to_double(exact);
 *
 * // Cleanup
 * dc_int_release(&a);
 * dc_int_release(&b);
 * dc_int_release(&sum);
 * dc_frac_release(&c);
 * dc_frac_release(&exact);
 * dc_double_release(&d);
 * dc_double_release(&e);
 * dc_double_release(&approx);
 * @endcode
 */

#ifndef DYNAMIC_COMPLEX_H
#define DYNAMIC_COMPLEX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

/* Configuration macros */
#ifndef DC_MALLOC
#define DC_MALLOC malloc
#endif

#ifndef DC_FREE
#define DC_FREE free
#endif

#ifndef DC_ASSERT
#include <assert.h>
#define DC_ASSERT assert
#endif

/* Atomic reference counting configuration */
#ifndef DC_ATOMIC_REFCOUNT
#define DC_ATOMIC_REFCOUNT 0
#endif

#if DC_ATOMIC_REFCOUNT && __STDC_VERSION__ < 201112L
    #error "DC_ATOMIC_REFCOUNT requires C11 or later for atomic support (compile with -std=c11 or later)"
#endif

#if DC_ATOMIC_REFCOUNT
    #include <stdatomic.h>
    #define DC_ATOMIC_SIZE_T _Atomic size_t
    #define DC_ATOMIC_FETCH_ADD(ptr, val) atomic_fetch_add(ptr, val)
    #define DC_ATOMIC_FETCH_SUB(ptr, val) atomic_fetch_sub(ptr, val)
    #define DC_ATOMIC_LOAD(ptr) atomic_load(ptr)
    #define DC_ATOMIC_STORE(ptr, val) atomic_store(ptr, val)
#else
    #define DC_ATOMIC_SIZE_T size_t
    #define DC_ATOMIC_FETCH_ADD(ptr, val) (*(ptr) += (val), *(ptr) - (val))
    #define DC_ATOMIC_FETCH_SUB(ptr, val) (*(ptr) -= (val), *(ptr) + (val))
    #define DC_ATOMIC_LOAD(ptr) (*(ptr))
    #define DC_ATOMIC_STORE(ptr, val) (*(ptr) = (val))
#endif

/* API macros */
#ifdef DC_STATIC
#define DC_DEC static
#define DC_DEF static
#else
#define DC_DEC extern
#define DC_DEF /* nothing - default linkage */
#endif

/* Include dependencies - these must be included with their implementations */
#define DI_IMPLEMENTATION
#define DI_STATIC
#include "devDeps/dynamic_int.h"

#define DF_IMPLEMENTATION
#define DF_STATIC
#include "devDeps/dynamic_fraction.h"

// ============================================================================
// TYPE DEFINITIONS
// ============================================================================

/**
 * @typedef dc_complex_int
 * @brief Opaque pointer to a Gaussian integer (complex number with integer components)
 */
typedef struct dc_complex_int_internal* dc_complex_int;

/**
 * @typedef dc_complex_frac
 * @brief Opaque pointer to a rational complex number
 */
typedef struct dc_complex_frac_internal* dc_complex_frac;

/**
 * @typedef dc_complex_double
 * @brief Opaque pointer to a floating-point complex number
 */
typedef struct dc_complex_double_internal* dc_complex_double;

/**
 * @struct dc_complex_int_internal
 * @brief Internal structure for a Gaussian integer
 */
struct dc_complex_int_internal {
    DC_ATOMIC_SIZE_T ref_count;
    di_int real;
    di_int imag;
};

/**
 * @struct dc_complex_frac_internal
 * @brief Internal structure for a rational complex number
 */
struct dc_complex_frac_internal {
    DC_ATOMIC_SIZE_T ref_count;
    df_frac real;
    df_frac imag;
};

/**
 * @struct dc_complex_double_internal
 * @brief Internal structure for a floating-point complex number
 */
struct dc_complex_double_internal {
    DC_ATOMIC_SIZE_T ref_count;
    double complex value;
};

// ============================================================================
// INTEGER COMPLEX INTERFACE
// ============================================================================

/**
 * @defgroup dc_int_functions Integer Complex Functions
 * @brief Functions for Gaussian integers (complex numbers with integer components)
 * @{
 */

/* Creation */
DC_DEC dc_complex_int dc_int_from_ints(int64_t real, int64_t imag);
DC_DEC dc_complex_int dc_int_from_di(di_int real, di_int imag);
DC_DEC dc_complex_int dc_int_zero(void);
DC_DEC dc_complex_int dc_int_one(void);
DC_DEC dc_complex_int dc_int_i(void);
DC_DEC dc_complex_int dc_int_neg_one(void);
DC_DEC dc_complex_int dc_int_neg_i(void);

/* Memory management */
DC_DEC dc_complex_int dc_int_retain(dc_complex_int c);
DC_DEC void dc_int_release(dc_complex_int* c);
DC_DEC dc_complex_int dc_int_copy(dc_complex_int c);

/* Arithmetic */
DC_DEC dc_complex_int dc_int_add(dc_complex_int a, dc_complex_int b);
DC_DEC dc_complex_int dc_int_sub(dc_complex_int a, dc_complex_int b);
DC_DEC dc_complex_int dc_int_mul(dc_complex_int a, dc_complex_int b);
DC_DEC dc_complex_frac dc_int_div(dc_complex_int a, dc_complex_int b);  // Returns fraction!
DC_DEC dc_complex_int dc_int_negate(dc_complex_int c);
DC_DEC dc_complex_int dc_int_conj(dc_complex_int c);

/* Accessors */
DC_DEC di_int dc_int_real(dc_complex_int c);
DC_DEC di_int dc_int_imag(dc_complex_int c);

/* Comparisons */
DC_DEC bool dc_int_eq(dc_complex_int a, dc_complex_int b);
DC_DEC bool dc_int_is_zero(dc_complex_int c);
DC_DEC bool dc_int_is_real(dc_complex_int c);
DC_DEC bool dc_int_is_imag(dc_complex_int c);

/* String conversion */
DC_DEC char* dc_int_to_string(dc_complex_int c);

/** @} */

// ============================================================================
// FRACTION COMPLEX INTERFACE
// ============================================================================

/**
 * @defgroup dc_frac_functions Fraction Complex Functions
 * @brief Functions for rational complex numbers
 * @{
 */

/* Creation */
DC_DEC dc_complex_frac dc_frac_from_ints(int64_t real_num, int64_t real_den,
                                          int64_t imag_num, int64_t imag_den);
DC_DEC dc_complex_frac dc_frac_from_df(df_frac real, df_frac imag);
DC_DEC dc_complex_frac dc_frac_zero(void);
DC_DEC dc_complex_frac dc_frac_one(void);
DC_DEC dc_complex_frac dc_frac_i(void);
DC_DEC dc_complex_frac dc_frac_neg_one(void);
DC_DEC dc_complex_frac dc_frac_neg_i(void);

/* Memory management */
DC_DEC dc_complex_frac dc_frac_retain(dc_complex_frac c);
DC_DEC void dc_frac_release(dc_complex_frac* c);
DC_DEC dc_complex_frac dc_frac_copy(dc_complex_frac c);

/* Arithmetic */
DC_DEC dc_complex_frac dc_frac_add(dc_complex_frac a, dc_complex_frac b);
DC_DEC dc_complex_frac dc_frac_sub(dc_complex_frac a, dc_complex_frac b);
DC_DEC dc_complex_frac dc_frac_mul(dc_complex_frac a, dc_complex_frac b);
DC_DEC dc_complex_frac dc_frac_div(dc_complex_frac a, dc_complex_frac b);
DC_DEC dc_complex_frac dc_frac_negate(dc_complex_frac c);
DC_DEC dc_complex_frac dc_frac_conj(dc_complex_frac c);
DC_DEC dc_complex_frac dc_frac_reciprocal(dc_complex_frac c);

/* Accessors */
DC_DEC df_frac dc_frac_real(dc_complex_frac c);
DC_DEC df_frac dc_frac_imag(dc_complex_frac c);

/* Comparisons */
DC_DEC bool dc_frac_eq(dc_complex_frac a, dc_complex_frac b);
DC_DEC bool dc_frac_is_zero(dc_complex_frac c);
DC_DEC bool dc_frac_is_real(dc_complex_frac c);
DC_DEC bool dc_frac_is_imag(dc_complex_frac c);
DC_DEC bool dc_frac_is_gaussian_int(dc_complex_frac c);

/* String conversion */
DC_DEC char* dc_frac_to_string(dc_complex_frac c);

/** @} */

// ============================================================================
// DOUBLE COMPLEX INTERFACE
// ============================================================================

/**
 * @defgroup dc_double_functions Double Complex Functions
 * @brief Functions for floating-point complex numbers
 * @{
 */

/* Creation */
DC_DEC dc_complex_double dc_double_from_doubles(double real, double imag);
DC_DEC dc_complex_double dc_double_from_polar(double magnitude, double angle);
DC_DEC dc_complex_double dc_double_zero(void);
DC_DEC dc_complex_double dc_double_one(void);
DC_DEC dc_complex_double dc_double_i(void);
DC_DEC dc_complex_double dc_double_neg_one(void);
DC_DEC dc_complex_double dc_double_neg_i(void);

/* Memory management */
DC_DEC dc_complex_double dc_double_retain(dc_complex_double c);
DC_DEC void dc_double_release(dc_complex_double* c);
DC_DEC dc_complex_double dc_double_copy(dc_complex_double c);

/* Arithmetic */
DC_DEC dc_complex_double dc_double_add(dc_complex_double a, dc_complex_double b);
DC_DEC dc_complex_double dc_double_sub(dc_complex_double a, dc_complex_double b);
DC_DEC dc_complex_double dc_double_mul(dc_complex_double a, dc_complex_double b);
DC_DEC dc_complex_double dc_double_div(dc_complex_double a, dc_complex_double b);
DC_DEC dc_complex_double dc_double_negate(dc_complex_double c);
DC_DEC dc_complex_double dc_double_conj(dc_complex_double c);

/* Complex-specific operations */
DC_DEC dc_complex_double dc_double_exp(dc_complex_double c);
DC_DEC dc_complex_double dc_double_log(dc_complex_double c);
DC_DEC dc_complex_double dc_double_pow(dc_complex_double a, dc_complex_double b);
DC_DEC dc_complex_double dc_double_sqrt(dc_complex_double c);
DC_DEC dc_complex_double dc_double_sin(dc_complex_double c);
DC_DEC dc_complex_double dc_double_cos(dc_complex_double c);
DC_DEC dc_complex_double dc_double_tan(dc_complex_double c);
DC_DEC dc_complex_double dc_double_sinh(dc_complex_double c);
DC_DEC dc_complex_double dc_double_cosh(dc_complex_double c);
DC_DEC dc_complex_double dc_double_tanh(dc_complex_double c);

/* Accessors */
DC_DEC double dc_double_real(dc_complex_double c);
DC_DEC double dc_double_imag(dc_complex_double c);
DC_DEC double dc_double_abs(dc_complex_double c);
DC_DEC double dc_double_arg(dc_complex_double c);

/* Comparisons */
DC_DEC bool dc_double_eq(dc_complex_double a, dc_complex_double b);
DC_DEC bool dc_double_is_zero(dc_complex_double c);
DC_DEC bool dc_double_is_real(dc_complex_double c);
DC_DEC bool dc_double_is_imag(dc_complex_double c);
DC_DEC bool dc_double_is_nan(dc_complex_double c);
DC_DEC bool dc_double_is_inf(dc_complex_double c);

/* String conversion */
DC_DEC char* dc_double_to_string(dc_complex_double c);

/** @} */

// ============================================================================
// TYPE CONVERSION INTERFACE
// ============================================================================

/**
 * @defgroup dc_conversion_functions Type Conversion Functions
 * @brief Functions for converting between complex number types
 * @{
 */

/* Upward conversions (lossless) */
DC_DEC dc_complex_frac dc_int_to_frac(dc_complex_int c);
DC_DEC dc_complex_double dc_int_to_double(dc_complex_int c);
DC_DEC dc_complex_double dc_frac_to_double(dc_complex_frac c);

/* Downward conversions (may round) */
DC_DEC dc_complex_int dc_frac_to_int(dc_complex_frac c);
DC_DEC dc_complex_int dc_double_to_int(dc_complex_double c);
DC_DEC dc_complex_frac dc_double_to_frac(dc_complex_double c, int64_t max_denominator);

/** @} */

// ============================================================================
// IMPLEMENTATION
// ============================================================================

#ifdef DC_IMPLEMENTATION

// Global singletons for common constants
static dc_complex_int dc_int_zero_singleton = NULL;
static dc_complex_int dc_int_one_singleton = NULL;
static dc_complex_int dc_int_i_singleton = NULL;
static dc_complex_int dc_int_neg_one_singleton = NULL;
static dc_complex_int dc_int_neg_i_singleton = NULL;

static dc_complex_frac dc_frac_zero_singleton = NULL;
static dc_complex_frac dc_frac_one_singleton = NULL;
static dc_complex_frac dc_frac_i_singleton = NULL;
static dc_complex_frac dc_frac_neg_one_singleton = NULL;
static dc_complex_frac dc_frac_neg_i_singleton = NULL;

static dc_complex_double dc_double_zero_singleton = NULL;
static dc_complex_double dc_double_one_singleton = NULL;
static dc_complex_double dc_double_i_singleton = NULL;
static dc_complex_double dc_double_neg_one_singleton = NULL;
static dc_complex_double dc_double_neg_i_singleton = NULL;

// ============================================================================
// INTEGER COMPLEX IMPLEMENTATION
// ============================================================================

DC_DEF dc_complex_int dc_int_from_ints(int64_t real, int64_t imag) {
    dc_complex_int result = DC_MALLOC(sizeof(struct dc_complex_int_internal));
    DC_ASSERT(result && "dc_int_from_ints: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->real = di_from_int64(real);
    result->imag = di_from_int64(imag);

    return result;
}

DC_DEF dc_complex_int dc_int_from_di(di_int real, di_int imag) {
    DC_ASSERT(real && "dc_int_from_di: real part cannot be NULL");
    DC_ASSERT(imag && "dc_int_from_di: imaginary part cannot be NULL");

    dc_complex_int result = DC_MALLOC(sizeof(struct dc_complex_int_internal));
    DC_ASSERT(result && "dc_int_from_di: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->real = di_retain(real);
    result->imag = di_retain(imag);

    return result;
}

DC_DEF dc_complex_int dc_int_zero(void) {
    if (!dc_int_zero_singleton) {
        dc_int_zero_singleton = dc_int_from_ints(0, 0);
        DC_ATOMIC_STORE(&dc_int_zero_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_int_retain(dc_int_zero_singleton);
}

DC_DEF dc_complex_int dc_int_one(void) {
    if (!dc_int_one_singleton) {
        dc_int_one_singleton = dc_int_from_ints(1, 0);
        DC_ATOMIC_STORE(&dc_int_one_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_int_retain(dc_int_one_singleton);
}

DC_DEF dc_complex_int dc_int_i(void) {
    if (!dc_int_i_singleton) {
        dc_int_i_singleton = dc_int_from_ints(0, 1);
        DC_ATOMIC_STORE(&dc_int_i_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_int_retain(dc_int_i_singleton);
}

DC_DEF dc_complex_int dc_int_neg_one(void) {
    if (!dc_int_neg_one_singleton) {
        dc_int_neg_one_singleton = dc_int_from_ints(-1, 0);
        DC_ATOMIC_STORE(&dc_int_neg_one_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_int_retain(dc_int_neg_one_singleton);
}

DC_DEF dc_complex_int dc_int_neg_i(void) {
    if (!dc_int_neg_i_singleton) {
        dc_int_neg_i_singleton = dc_int_from_ints(0, -1);
        DC_ATOMIC_STORE(&dc_int_neg_i_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_int_retain(dc_int_neg_i_singleton);
}

DC_DEF dc_complex_int dc_int_retain(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_retain: cannot retain NULL");
    DC_ATOMIC_FETCH_ADD(&c->ref_count, 1);
    return c;
}

DC_DEF void dc_int_release(dc_complex_int* c) {
    if (!c || !*c) return;

    size_t old_count = DC_ATOMIC_FETCH_SUB(&(*c)->ref_count, 1);
    if (old_count == 1) {
        // Don't free singletons
        if (*c == dc_int_zero_singleton || *c == dc_int_one_singleton ||
            *c == dc_int_i_singleton || *c == dc_int_neg_one_singleton ||
            *c == dc_int_neg_i_singleton) {
            *c = NULL;
            return;
        }

        di_release(&(*c)->real);
        di_release(&(*c)->imag);
        DC_FREE(*c);
    }
    *c = NULL;
}

DC_DEF dc_complex_int dc_int_copy(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_copy: cannot copy NULL");
    return dc_int_from_di(c->real, c->imag);
}

DC_DEF dc_complex_int dc_int_add(dc_complex_int a, dc_complex_int b) {
    DC_ASSERT(a && "dc_int_add: first operand cannot be NULL");
    DC_ASSERT(b && "dc_int_add: second operand cannot be NULL");

    di_int real = di_add(a->real, b->real);
    di_int imag = di_add(a->imag, b->imag);
    dc_complex_int result = dc_int_from_di(real, imag);
    di_release(&real);
    di_release(&imag);

    return result;
}

DC_DEF dc_complex_int dc_int_sub(dc_complex_int a, dc_complex_int b) {
    DC_ASSERT(a && "dc_int_sub: first operand cannot be NULL");
    DC_ASSERT(b && "dc_int_sub: second operand cannot be NULL");

    di_int real = di_sub(a->real, b->real);
    di_int imag = di_sub(a->imag, b->imag);
    dc_complex_int result = dc_int_from_di(real, imag);
    di_release(&real);
    di_release(&imag);

    return result;
}

DC_DEF dc_complex_int dc_int_mul(dc_complex_int a, dc_complex_int b) {
    DC_ASSERT(a && "dc_int_mul: first operand cannot be NULL");
    DC_ASSERT(b && "dc_int_mul: second operand cannot be NULL");

    // (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
    di_int ac = di_mul(a->real, b->real);
    di_int bd = di_mul(a->imag, b->imag);
    di_int ad = di_mul(a->real, b->imag);
    di_int bc = di_mul(a->imag, b->real);

    di_int real = di_sub(ac, bd);
    di_int imag = di_add(ad, bc);

    dc_complex_int result = dc_int_from_di(real, imag);

    di_release(&ac);
    di_release(&bd);
    di_release(&ad);
    di_release(&bc);
    di_release(&real);
    di_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_int_div(dc_complex_int a, dc_complex_int b) {
    DC_ASSERT(a && "dc_int_div: first operand cannot be NULL");
    DC_ASSERT(b && "dc_int_div: second operand cannot be NULL");

    // Convert to fractions and divide
    dc_complex_frac af = dc_int_to_frac(a);
    dc_complex_frac bf = dc_int_to_frac(b);
    dc_complex_frac result = dc_frac_div(af, bf);

    dc_frac_release(&af);
    dc_frac_release(&bf);

    return result;
}

DC_DEF dc_complex_int dc_int_negate(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_negate: operand cannot be NULL");

    di_int real = di_negate(c->real);
    di_int imag = di_negate(c->imag);
    dc_complex_int result = dc_int_from_di(real, imag);
    di_release(&real);
    di_release(&imag);

    return result;
}

DC_DEF dc_complex_int dc_int_conj(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_conj: operand cannot be NULL");

    di_int real = di_retain(c->real);
    di_int imag = di_negate(c->imag);
    dc_complex_int result = dc_int_from_di(real, imag);
    di_release(&real);
    di_release(&imag);

    return result;
}

DC_DEF di_int dc_int_real(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_real: operand cannot be NULL");
    return di_retain(c->real);
}

DC_DEF di_int dc_int_imag(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_imag: operand cannot be NULL");
    return di_retain(c->imag);
}

DC_DEF bool dc_int_eq(dc_complex_int a, dc_complex_int b) {
    DC_ASSERT(a && "dc_int_eq: first operand cannot be NULL");
    DC_ASSERT(b && "dc_int_eq: second operand cannot be NULL");

    return di_compare(a->real, b->real) == 0 && di_compare(a->imag, b->imag) == 0;
}

DC_DEF bool dc_int_is_zero(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_is_zero: operand cannot be NULL");
    return di_is_zero(c->real) && di_is_zero(c->imag);
}

DC_DEF bool dc_int_is_real(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_is_real: operand cannot be NULL");
    return di_is_zero(c->imag);
}

DC_DEF bool dc_int_is_imag(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_is_imag: operand cannot be NULL");
    return di_is_zero(c->real);
}

DC_DEF char* dc_int_to_string(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_to_string: operand cannot be NULL");

    char* real_str = di_to_string(c->real, 10);
    char* imag_str = di_to_string(c->imag, 10);

    size_t len = strlen(real_str) + strlen(imag_str) + 10;
    char* result = DC_MALLOC(len);
    DC_ASSERT(result && "dc_int_to_string: allocation failed");

    bool real_zero = di_is_zero(c->real);
    bool imag_zero = di_is_zero(c->imag);
    bool imag_neg = di_is_negative(c->imag);

    if (real_zero && imag_zero) {
        strcpy(result, "0");
    } else if (imag_zero) {
        strcpy(result, real_str);
    } else if (real_zero) {
        if (strcmp(imag_str, "1") == 0) {
            strcpy(result, "i");
        } else if (strcmp(imag_str, "-1") == 0) {
            strcpy(result, "-i");
        } else {
            sprintf(result, "%si", imag_str);
        }
    } else {
        if (strcmp(imag_str, "1") == 0) {
            sprintf(result, "%s+i", real_str);
        } else if (strcmp(imag_str, "-1") == 0) {
            sprintf(result, "%s-i", real_str);
        } else if (imag_neg) {
            sprintf(result, "%s%si", real_str, imag_str);
        } else {
            sprintf(result, "%s+%si", real_str, imag_str);
        }
    }

    free(real_str);
    free(imag_str);

    return result;
}

// ============================================================================
// FRACTION COMPLEX IMPLEMENTATION
// ============================================================================

DC_DEF dc_complex_frac dc_frac_from_ints(int64_t real_num, int64_t real_den,
                                          int64_t imag_num, int64_t imag_den) {
    df_frac real = df_from_ints(real_num, real_den);
    df_frac imag = df_from_ints(imag_num, imag_den);
    dc_complex_frac result = dc_frac_from_df(real, imag);
    df_release(&real);
    df_release(&imag);
    return result;
}

DC_DEF dc_complex_frac dc_frac_from_df(df_frac real, df_frac imag) {
    DC_ASSERT(real && "dc_frac_from_df: real part cannot be NULL");
    DC_ASSERT(imag && "dc_frac_from_df: imaginary part cannot be NULL");

    dc_complex_frac result = DC_MALLOC(sizeof(struct dc_complex_frac_internal));
    DC_ASSERT(result && "dc_frac_from_df: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->real = df_retain(real);
    result->imag = df_retain(imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_zero(void) {
    if (!dc_frac_zero_singleton) {
        dc_frac_zero_singleton = dc_frac_from_ints(0, 1, 0, 1);
        DC_ATOMIC_STORE(&dc_frac_zero_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_frac_retain(dc_frac_zero_singleton);
}

DC_DEF dc_complex_frac dc_frac_one(void) {
    if (!dc_frac_one_singleton) {
        dc_frac_one_singleton = dc_frac_from_ints(1, 1, 0, 1);
        DC_ATOMIC_STORE(&dc_frac_one_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_frac_retain(dc_frac_one_singleton);
}

DC_DEF dc_complex_frac dc_frac_i(void) {
    if (!dc_frac_i_singleton) {
        dc_frac_i_singleton = dc_frac_from_ints(0, 1, 1, 1);
        DC_ATOMIC_STORE(&dc_frac_i_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_frac_retain(dc_frac_i_singleton);
}

DC_DEF dc_complex_frac dc_frac_neg_one(void) {
    if (!dc_frac_neg_one_singleton) {
        dc_frac_neg_one_singleton = dc_frac_from_ints(-1, 1, 0, 1);
        DC_ATOMIC_STORE(&dc_frac_neg_one_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_frac_retain(dc_frac_neg_one_singleton);
}

DC_DEF dc_complex_frac dc_frac_neg_i(void) {
    if (!dc_frac_neg_i_singleton) {
        dc_frac_neg_i_singleton = dc_frac_from_ints(0, 1, -1, 1);
        DC_ATOMIC_STORE(&dc_frac_neg_i_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_frac_retain(dc_frac_neg_i_singleton);
}

DC_DEF dc_complex_frac dc_frac_retain(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_retain: cannot retain NULL");
    DC_ATOMIC_FETCH_ADD(&c->ref_count, 1);
    return c;
}

DC_DEF void dc_frac_release(dc_complex_frac* c) {
    if (!c || !*c) return;

    size_t old_count = DC_ATOMIC_FETCH_SUB(&(*c)->ref_count, 1);
    if (old_count == 1) {
        // Don't free singletons
        if (*c == dc_frac_zero_singleton || *c == dc_frac_one_singleton ||
            *c == dc_frac_i_singleton || *c == dc_frac_neg_one_singleton ||
            *c == dc_frac_neg_i_singleton) {
            *c = NULL;
            return;
        }

        df_release(&(*c)->real);
        df_release(&(*c)->imag);
        DC_FREE(*c);
    }
    *c = NULL;
}

DC_DEF dc_complex_frac dc_frac_copy(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_copy: cannot copy NULL");
    return dc_frac_from_df(c->real, c->imag);
}

DC_DEF dc_complex_frac dc_frac_add(dc_complex_frac a, dc_complex_frac b) {
    DC_ASSERT(a && "dc_frac_add: first operand cannot be NULL");
    DC_ASSERT(b && "dc_frac_add: second operand cannot be NULL");

    df_frac real = df_add(a->real, b->real);
    df_frac imag = df_add(a->imag, b->imag);
    dc_complex_frac result = dc_frac_from_df(real, imag);
    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_sub(dc_complex_frac a, dc_complex_frac b) {
    DC_ASSERT(a && "dc_frac_sub: first operand cannot be NULL");
    DC_ASSERT(b && "dc_frac_sub: second operand cannot be NULL");

    df_frac real = df_sub(a->real, b->real);
    df_frac imag = df_sub(a->imag, b->imag);
    dc_complex_frac result = dc_frac_from_df(real, imag);
    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_mul(dc_complex_frac a, dc_complex_frac b) {
    DC_ASSERT(a && "dc_frac_mul: first operand cannot be NULL");
    DC_ASSERT(b && "dc_frac_mul: second operand cannot be NULL");

    // (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
    df_frac ac = df_mul(a->real, b->real);
    df_frac bd = df_mul(a->imag, b->imag);
    df_frac ad = df_mul(a->real, b->imag);
    df_frac bc = df_mul(a->imag, b->real);

    df_frac real = df_sub(ac, bd);
    df_frac imag = df_add(ad, bc);

    dc_complex_frac result = dc_frac_from_df(real, imag);

    df_release(&ac);
    df_release(&bd);
    df_release(&ad);
    df_release(&bc);
    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_div(dc_complex_frac a, dc_complex_frac b) {
    DC_ASSERT(a && "dc_frac_div: first operand cannot be NULL");
    DC_ASSERT(b && "dc_frac_div: second operand cannot be NULL");
    DC_ASSERT(!dc_frac_is_zero(b) && "dc_frac_div: division by zero");

    // (a + bi) / (c + di) = ((ac + bd) / (c² + d²)) + ((bc - ad) / (c² + d²))i
    df_frac c2 = df_mul(b->real, b->real);
    df_frac d2 = df_mul(b->imag, b->imag);
    df_frac denom = df_add(c2, d2);

    df_frac ac = df_mul(a->real, b->real);
    df_frac bd = df_mul(a->imag, b->imag);
    df_frac bc = df_mul(a->imag, b->real);
    df_frac ad = df_mul(a->real, b->imag);

    df_frac real_num = df_add(ac, bd);
    df_frac imag_num = df_sub(bc, ad);

    df_frac real = df_div(real_num, denom);
    df_frac imag = df_div(imag_num, denom);

    dc_complex_frac result = dc_frac_from_df(real, imag);

    df_release(&c2);
    df_release(&d2);
    df_release(&denom);
    df_release(&ac);
    df_release(&bd);
    df_release(&bc);
    df_release(&ad);
    df_release(&real_num);
    df_release(&imag_num);
    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_negate(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_negate: operand cannot be NULL");

    df_frac real = df_negate(c->real);
    df_frac imag = df_negate(c->imag);
    dc_complex_frac result = dc_frac_from_df(real, imag);
    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_conj(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_conj: operand cannot be NULL");

    df_frac real = df_retain(c->real);
    df_frac imag = df_negate(c->imag);
    dc_complex_frac result = dc_frac_from_df(real, imag);
    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_frac dc_frac_reciprocal(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_reciprocal: operand cannot be NULL");
    DC_ASSERT(!dc_frac_is_zero(c) && "dc_frac_reciprocal: division by zero");

    df_frac one = df_one();
    df_frac zero = df_zero();
    dc_complex_frac num = dc_frac_from_df(one, zero);
    dc_complex_frac result = dc_frac_div(num, c);

    df_release(&one);
    df_release(&zero);
    dc_frac_release(&num);

    return result;
}

DC_DEF df_frac dc_frac_real(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_real: operand cannot be NULL");
    return df_retain(c->real);
}

DC_DEF df_frac dc_frac_imag(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_imag: operand cannot be NULL");
    return df_retain(c->imag);
}

DC_DEF bool dc_frac_eq(dc_complex_frac a, dc_complex_frac b) {
    DC_ASSERT(a && "dc_frac_eq: first operand cannot be NULL");
    DC_ASSERT(b && "dc_frac_eq: second operand cannot be NULL");

    return df_eq(a->real, b->real) && df_eq(a->imag, b->imag);
}

DC_DEF bool dc_frac_is_zero(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_is_zero: operand cannot be NULL");
    return df_is_zero(c->real) && df_is_zero(c->imag);
}

DC_DEF bool dc_frac_is_real(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_is_real: operand cannot be NULL");
    return df_is_zero(c->imag);
}

DC_DEF bool dc_frac_is_imag(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_is_imag: operand cannot be NULL");
    return df_is_zero(c->real);
}

DC_DEF bool dc_frac_is_gaussian_int(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_is_gaussian_int: operand cannot be NULL");
    return df_is_integer(c->real) && df_is_integer(c->imag);
}

DC_DEF char* dc_frac_to_string(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_to_string: operand cannot be NULL");

    char* real_str = df_to_string(c->real);
    char* imag_str = df_to_string(c->imag);

    size_t len = strlen(real_str) + strlen(imag_str) + 10;
    char* result = DC_MALLOC(len);
    DC_ASSERT(result && "dc_frac_to_string: allocation failed");

    bool real_zero = df_is_zero(c->real);
    bool imag_zero = df_is_zero(c->imag);
    bool imag_neg = df_is_negative(c->imag);

    if (real_zero && imag_zero) {
        strcpy(result, "0");
    } else if (imag_zero) {
        strcpy(result, real_str);
    } else if (real_zero) {
        df_frac one = df_one();
        df_frac neg_one = df_neg_one();
        if (df_cmp(c->imag, one) == 0) {
            strcpy(result, "i");
        } else if (df_cmp(c->imag, neg_one) == 0) {
            strcpy(result, "-i");
        } else {
            sprintf(result, "%si", imag_str);
        }
        df_release(&one);
        df_release(&neg_one);
    } else {
        df_frac one = df_one();
        df_frac neg_one = df_neg_one();
        if (df_cmp(c->imag, one) == 0) {
            sprintf(result, "%s+i", real_str);
        } else if (df_cmp(c->imag, neg_one) == 0) {
            sprintf(result, "%s-i", real_str);
        } else if (imag_neg) {
            sprintf(result, "%s%si", real_str, imag_str);
        } else {
            sprintf(result, "%s+%si", real_str, imag_str);
        }
        df_release(&one);
        df_release(&neg_one);
    }

    free(real_str);
    free(imag_str);

    return result;
}

// ============================================================================
// DOUBLE COMPLEX IMPLEMENTATION
// ============================================================================

DC_DEF dc_complex_double dc_double_from_doubles(double real, double imag) {
    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_from_doubles: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = real + imag * I;

    return result;
}

DC_DEF dc_complex_double dc_double_from_polar(double magnitude, double angle) {
    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_from_polar: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = magnitude * cexp(I * angle);

    return result;
}

DC_DEF dc_complex_double dc_double_zero(void) {
    if (!dc_double_zero_singleton) {
        dc_double_zero_singleton = dc_double_from_doubles(0.0, 0.0);
        DC_ATOMIC_STORE(&dc_double_zero_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_double_retain(dc_double_zero_singleton);
}

DC_DEF dc_complex_double dc_double_one(void) {
    if (!dc_double_one_singleton) {
        dc_double_one_singleton = dc_double_from_doubles(1.0, 0.0);
        DC_ATOMIC_STORE(&dc_double_one_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_double_retain(dc_double_one_singleton);
}

DC_DEF dc_complex_double dc_double_i(void) {
    if (!dc_double_i_singleton) {
        dc_double_i_singleton = dc_double_from_doubles(0.0, 1.0);
        DC_ATOMIC_STORE(&dc_double_i_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_double_retain(dc_double_i_singleton);
}

DC_DEF dc_complex_double dc_double_neg_one(void) {
    if (!dc_double_neg_one_singleton) {
        dc_double_neg_one_singleton = dc_double_from_doubles(-1.0, 0.0);
        DC_ATOMIC_STORE(&dc_double_neg_one_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_double_retain(dc_double_neg_one_singleton);
}

DC_DEF dc_complex_double dc_double_neg_i(void) {
    if (!dc_double_neg_i_singleton) {
        dc_double_neg_i_singleton = dc_double_from_doubles(0.0, -1.0);
        DC_ATOMIC_STORE(&dc_double_neg_i_singleton->ref_count, SIZE_MAX/2);
    }
    return dc_double_retain(dc_double_neg_i_singleton);
}

DC_DEF dc_complex_double dc_double_retain(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_retain: cannot retain NULL");
    DC_ATOMIC_FETCH_ADD(&c->ref_count, 1);
    return c;
}

DC_DEF void dc_double_release(dc_complex_double* c) {
    if (!c || !*c) return;

    size_t old_count = DC_ATOMIC_FETCH_SUB(&(*c)->ref_count, 1);
    if (old_count == 1) {
        // Don't free singletons
        if (*c == dc_double_zero_singleton || *c == dc_double_one_singleton ||
            *c == dc_double_i_singleton || *c == dc_double_neg_one_singleton ||
            *c == dc_double_neg_i_singleton) {
            *c = NULL;
            return;
        }

        DC_FREE(*c);
    }
    *c = NULL;
}

DC_DEF dc_complex_double dc_double_copy(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_copy: cannot copy NULL");
    return dc_double_from_doubles(creal(c->value), cimag(c->value));
}

DC_DEF dc_complex_double dc_double_add(dc_complex_double a, dc_complex_double b) {
    DC_ASSERT(a && "dc_double_add: first operand cannot be NULL");
    DC_ASSERT(b && "dc_double_add: second operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_add: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = a->value + b->value;

    return result;
}

DC_DEF dc_complex_double dc_double_sub(dc_complex_double a, dc_complex_double b) {
    DC_ASSERT(a && "dc_double_sub: first operand cannot be NULL");
    DC_ASSERT(b && "dc_double_sub: second operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_sub: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = a->value - b->value;

    return result;
}

DC_DEF dc_complex_double dc_double_mul(dc_complex_double a, dc_complex_double b) {
    DC_ASSERT(a && "dc_double_mul: first operand cannot be NULL");
    DC_ASSERT(b && "dc_double_mul: second operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_mul: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = a->value * b->value;

    return result;
}

DC_DEF dc_complex_double dc_double_div(dc_complex_double a, dc_complex_double b) {
    DC_ASSERT(a && "dc_double_div: first operand cannot be NULL");
    DC_ASSERT(b && "dc_double_div: second operand cannot be NULL");
    DC_ASSERT(!dc_double_is_zero(b) && "dc_double_div: division by zero");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_div: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = a->value / b->value;

    return result;
}

DC_DEF dc_complex_double dc_double_negate(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_negate: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_negate: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = -c->value;

    return result;
}

DC_DEF dc_complex_double dc_double_conj(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_conj: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_conj: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = conj(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_exp(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_exp: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_exp: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = cexp(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_log(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_log: operand cannot be NULL");
    DC_ASSERT(!dc_double_is_zero(c) && "dc_double_log: log of zero");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_log: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = clog(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_pow(dc_complex_double a, dc_complex_double b) {
    DC_ASSERT(a && "dc_double_pow: base cannot be NULL");
    DC_ASSERT(b && "dc_double_pow: exponent cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_pow: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = cpow(a->value, b->value);

    return result;
}

DC_DEF dc_complex_double dc_double_sqrt(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_sqrt: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_sqrt: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = csqrt(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_sin(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_sin: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_sin: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = csin(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_cos(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_cos: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_cos: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = ccos(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_tan(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_tan: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_tan: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = ctan(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_sinh(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_sinh: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_sinh: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = csinh(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_cosh(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_cosh: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_cosh: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = ccosh(c->value);

    return result;
}

DC_DEF dc_complex_double dc_double_tanh(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_tanh: operand cannot be NULL");

    dc_complex_double result = DC_MALLOC(sizeof(struct dc_complex_double_internal));
    DC_ASSERT(result && "dc_double_tanh: allocation failed");

    DC_ATOMIC_STORE(&result->ref_count, 1);
    result->value = ctanh(c->value);

    return result;
}

DC_DEF double dc_double_real(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_real: operand cannot be NULL");
    return creal(c->value);
}

DC_DEF double dc_double_imag(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_imag: operand cannot be NULL");
    return cimag(c->value);
}

DC_DEF double dc_double_abs(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_abs: operand cannot be NULL");
    return cabs(c->value);
}

DC_DEF double dc_double_arg(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_arg: operand cannot be NULL");
    return carg(c->value);
}

DC_DEF bool dc_double_eq(dc_complex_double a, dc_complex_double b) {
    DC_ASSERT(a && "dc_double_eq: first operand cannot be NULL");
    DC_ASSERT(b && "dc_double_eq: second operand cannot be NULL");

    return creal(a->value) == creal(b->value) && cimag(a->value) == cimag(b->value);
}

DC_DEF bool dc_double_is_zero(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_is_zero: operand cannot be NULL");
    return creal(c->value) == 0.0 && cimag(c->value) == 0.0;
}

DC_DEF bool dc_double_is_real(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_is_real: operand cannot be NULL");
    return cimag(c->value) == 0.0;
}

DC_DEF bool dc_double_is_imag(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_is_imag: operand cannot be NULL");
    return creal(c->value) == 0.0;
}

DC_DEF bool dc_double_is_nan(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_is_nan: operand cannot be NULL");
    return isnan(creal(c->value)) || isnan(cimag(c->value));
}

DC_DEF bool dc_double_is_inf(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_is_inf: operand cannot be NULL");
    return isinf(creal(c->value)) || isinf(cimag(c->value));
}

DC_DEF char* dc_double_to_string(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_to_string: operand cannot be NULL");

    char* result = DC_MALLOC(256);
    DC_ASSERT(result && "dc_double_to_string: allocation failed");

    double real = creal(c->value);
    double imag = cimag(c->value);

    if (real == 0.0 && imag == 0.0) {
        strcpy(result, "0");
    } else if (imag == 0.0) {
        sprintf(result, "%g", real);
    } else if (real == 0.0) {
        if (imag == 1.0) {
            strcpy(result, "i");
        } else if (imag == -1.0) {
            strcpy(result, "-i");
        } else {
            sprintf(result, "%gi", imag);
        }
    } else {
        if (imag == 1.0) {
            sprintf(result, "%g+i", real);
        } else if (imag == -1.0) {
            sprintf(result, "%g-i", real);
        } else if (imag < 0) {
            sprintf(result, "%g%gi", real, imag);
        } else {
            sprintf(result, "%g+%gi", real, imag);
        }
    }

    return result;
}

// ============================================================================
// TYPE CONVERSION IMPLEMENTATION
// ============================================================================

DC_DEF dc_complex_frac dc_int_to_frac(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_to_frac: operand cannot be NULL");

    df_frac real = df_from_di(c->real, di_one());
    df_frac imag = df_from_di(c->imag, di_one());
    dc_complex_frac result = dc_frac_from_df(real, imag);

    df_release(&real);
    df_release(&imag);

    return result;
}

DC_DEF dc_complex_double dc_int_to_double(dc_complex_int c) {
    DC_ASSERT(c && "dc_int_to_double: operand cannot be NULL");

    double real = di_to_double(c->real);
    double imag = di_to_double(c->imag);

    return dc_double_from_doubles(real, imag);
}

DC_DEF dc_complex_double dc_frac_to_double(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_to_double: operand cannot be NULL");

    double real = df_to_double(c->real);
    double imag = df_to_double(c->imag);

    return dc_double_from_doubles(real, imag);
}

DC_DEF dc_complex_int dc_frac_to_int(dc_complex_frac c) {
    DC_ASSERT(c && "dc_frac_to_int: operand cannot be NULL");

    // Convert to double and round
    double real = df_to_double(c->real);
    double imag = df_to_double(c->imag);

    int64_t real_rounded = (int64_t)round(real);
    int64_t imag_rounded = (int64_t)round(imag);

    return dc_int_from_ints(real_rounded, imag_rounded);
}

DC_DEF dc_complex_int dc_double_to_int(dc_complex_double c) {
    DC_ASSERT(c && "dc_double_to_int: operand cannot be NULL");

    double real = creal(c->value);
    double imag = cimag(c->value);

    int64_t real_rounded = (int64_t)round(real);
    int64_t imag_rounded = (int64_t)round(imag);

    return dc_int_from_ints(real_rounded, imag_rounded);
}

DC_DEF dc_complex_frac dc_double_to_frac(dc_complex_double c, int64_t max_denominator) {
    DC_ASSERT(c && "dc_double_to_frac: operand cannot be NULL");
    DC_ASSERT(max_denominator > 0 && "dc_double_to_frac: max_denominator must be positive");

    double real = creal(c->value);
    double imag = cimag(c->value);

    df_frac real_frac = df_from_double(real, max_denominator);
    df_frac imag_frac = df_from_double(imag, max_denominator);

    dc_complex_frac result = dc_frac_from_df(real_frac, imag_frac);

    df_release(&real_frac);
    df_release(&imag_frac);

    return result;
}

#endif // DC_IMPLEMENTATION

#endif // DYNAMIC_COMPLEX_H