#define UNITY_INCLUDE_DOUBLE
#include "devDeps/unity/unity.h"

#define DC_IMPLEMENTATION
#include "dynamic_complex.h"

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

// ============================================================================
// INTEGER COMPLEX TESTS
// ============================================================================

void test_dc_int_creation(void) {
    dc_complex_int a = dc_int_from_ints(3, 4);
    dc_complex_int b = dc_int_zero();
    dc_complex_int c = dc_int_one();
    dc_complex_int d = dc_int_i();

    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_NOT_NULL(d);

    TEST_ASSERT_TRUE(dc_int_is_zero(b));
    TEST_ASSERT_TRUE(dc_int_is_real(c));
    TEST_ASSERT_TRUE(dc_int_is_imag(d));

    dc_int_release(&a);
    dc_int_release(&b);
    dc_int_release(&c);
    dc_int_release(&d);
}

void test_dc_int_arithmetic(void) {
    dc_complex_int a = dc_int_from_ints(3, 4);   // 3 + 4i
    dc_complex_int b = dc_int_from_ints(1, -2);  // 1 - 2i

    // Addition: (3 + 4i) + (1 - 2i) = 4 + 2i
    dc_complex_int sum = dc_int_add(a, b);
    di_int sum_real = dc_int_real(sum);
    di_int sum_imag = dc_int_imag(sum);

    int32_t real_val, imag_val;
    TEST_ASSERT_TRUE(di_to_int32(sum_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(sum_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(4, real_val);
    TEST_ASSERT_EQUAL_INT32(2, imag_val);

    // Subtraction: (3 + 4i) - (1 - 2i) = 2 + 6i
    dc_complex_int diff = dc_int_sub(a, b);
    di_int diff_real = dc_int_real(diff);
    di_int diff_imag = dc_int_imag(diff);

    TEST_ASSERT_TRUE(di_to_int32(diff_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(diff_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(2, real_val);
    TEST_ASSERT_EQUAL_INT32(6, imag_val);

    // Multiplication: (3 + 4i) * (1 - 2i) = 11 - 2i
    dc_complex_int prod = dc_int_mul(a, b);
    di_int prod_real = dc_int_real(prod);
    di_int prod_imag = dc_int_imag(prod);

    TEST_ASSERT_TRUE(di_to_int32(prod_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(prod_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(11, real_val);
    TEST_ASSERT_EQUAL_INT32(-2, imag_val);

    // Conjugate: conj(3 + 4i) = 3 - 4i
    dc_complex_int conj = dc_int_conj(a);
    di_int conj_real = dc_int_real(conj);
    di_int conj_imag = dc_int_imag(conj);

    TEST_ASSERT_TRUE(di_to_int32(conj_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(conj_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(3, real_val);
    TEST_ASSERT_EQUAL_INT32(-4, imag_val);

    // Cleanup
    dc_int_release(&a);
    dc_int_release(&b);
    dc_int_release(&sum);
    dc_int_release(&diff);
    dc_int_release(&prod);
    dc_int_release(&conj);
    di_release(&sum_real);
    di_release(&sum_imag);
    di_release(&diff_real);
    di_release(&diff_imag);
    di_release(&prod_real);
    di_release(&prod_imag);
    di_release(&conj_real);
    di_release(&conj_imag);
}

void test_dc_int_division(void) {
    dc_complex_int a = dc_int_from_ints(3, 4);   // 3 + 4i
    dc_complex_int b = dc_int_from_ints(1, -2);  // 1 - 2i

    // Division returns rational complex: (3 + 4i) / (1 - 2i) = -1 + 2i
    dc_complex_frac quotient = dc_int_div(a, b);

    // Check if result is a Gaussian integer
    TEST_ASSERT_TRUE(dc_frac_is_gaussian_int(quotient));

    // Convert back to integer
    dc_complex_int quotient_int = dc_frac_to_int(quotient);
    di_int quot_real = dc_int_real(quotient_int);
    di_int quot_imag = dc_int_imag(quotient_int);

    int32_t real_val, imag_val;
    TEST_ASSERT_TRUE(di_to_int32(quot_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(quot_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(-1, real_val);
    TEST_ASSERT_EQUAL_INT32(2, imag_val);

    // Cleanup
    dc_int_release(&a);
    dc_int_release(&b);
    dc_frac_release(&quotient);
    dc_int_release(&quotient_int);
    di_release(&quot_real);
    di_release(&quot_imag);
}

void test_dc_int_string_conversion(void) {
    dc_complex_int a = dc_int_from_ints(3, 4);
    dc_complex_int b = dc_int_from_ints(0, 1);
    dc_complex_int c = dc_int_from_ints(5, 0);
    dc_complex_int d = dc_int_zero();

    char* str_a = dc_int_to_string(a);
    char* str_b = dc_int_to_string(b);
    char* str_c = dc_int_to_string(c);
    char* str_d = dc_int_to_string(d);

    TEST_ASSERT_EQUAL_STRING("3+4i", str_a);
    TEST_ASSERT_EQUAL_STRING("i", str_b);
    TEST_ASSERT_EQUAL_STRING("5", str_c);
    TEST_ASSERT_EQUAL_STRING("0", str_d);

    // Cleanup
    dc_int_release(&a);
    dc_int_release(&b);
    dc_int_release(&c);
    dc_int_release(&d);
    free(str_a);
    free(str_b);
    free(str_c);
    free(str_d);
}

void test_dc_int_memory_management(void) {
    dc_complex_int a = dc_int_from_ints(1, 2);
    dc_complex_int b = dc_int_retain(a);

    TEST_ASSERT_EQUAL_PTR(a, b);

    dc_int_release(&a);
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);

    dc_int_release(&b);
    TEST_ASSERT_NULL(b);
}

void test_dc_int_missing_functions(void) {
    // Test dc_int_from_di
    di_int real = di_from_int64(5);
    di_int imag = di_from_int64(-3);
    dc_complex_int a = dc_int_from_di(real, imag);

    di_int a_real = dc_int_real(a);
    di_int a_imag = dc_int_imag(a);
    int32_t real_val, imag_val;
    TEST_ASSERT_TRUE(di_to_int32(a_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(a_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(5, real_val);
    TEST_ASSERT_EQUAL_INT32(-3, imag_val);

    // Test dc_int_neg_one and dc_int_neg_i
    dc_complex_int neg_one = dc_int_neg_one();
    dc_complex_int neg_i = dc_int_neg_i();

    di_int neg_one_real = dc_int_real(neg_one);
    di_int neg_one_imag = dc_int_imag(neg_one);
    di_int neg_i_real = dc_int_real(neg_i);
    di_int neg_i_imag = dc_int_imag(neg_i);

    TEST_ASSERT_TRUE(di_to_int32(neg_one_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(neg_one_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(-1, real_val);
    TEST_ASSERT_EQUAL_INT32(0, imag_val);

    TEST_ASSERT_TRUE(di_to_int32(neg_i_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(neg_i_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(0, real_val);
    TEST_ASSERT_EQUAL_INT32(-1, imag_val);

    // Test dc_int_copy
    dc_complex_int b = dc_int_copy(a);
    TEST_ASSERT_TRUE(dc_int_eq(a, b));
    TEST_ASSERT_FALSE(a == b);  // Different objects

    // Test dc_int_negate
    dc_complex_int neg_a = dc_int_negate(a);
    di_int neg_a_real = dc_int_real(neg_a);
    di_int neg_a_imag = dc_int_imag(neg_a);
    TEST_ASSERT_TRUE(di_to_int32(neg_a_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(neg_a_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(-5, real_val);
    TEST_ASSERT_EQUAL_INT32(3, imag_val);

    // Cleanup
    di_release(&real);
    di_release(&imag);
    dc_int_release(&a);
    dc_int_release(&neg_one);
    dc_int_release(&neg_i);
    dc_int_release(&b);
    dc_int_release(&neg_a);
    di_release(&a_real);
    di_release(&a_imag);
    di_release(&neg_one_real);
    di_release(&neg_one_imag);
    di_release(&neg_i_real);
    di_release(&neg_i_imag);
    di_release(&neg_a_real);
    di_release(&neg_a_imag);
}

// ============================================================================
// FRACTION COMPLEX TESTS
// ============================================================================

void test_dc_frac_creation(void) {
    dc_complex_frac a = dc_frac_from_ints(1, 2, 3, 4);  // 1/2 + 3/4i
    dc_complex_frac b = dc_frac_zero();
    dc_complex_frac c = dc_frac_one();
    dc_complex_frac d = dc_frac_i();

    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_NOT_NULL(d);

    TEST_ASSERT_TRUE(dc_frac_is_zero(b));
    TEST_ASSERT_TRUE(dc_frac_is_real(c));
    TEST_ASSERT_TRUE(dc_frac_is_imag(d));

    dc_frac_release(&a);
    dc_frac_release(&b);
    dc_frac_release(&c);
    dc_frac_release(&d);
}

void test_dc_frac_arithmetic(void) {
    dc_complex_frac a = dc_frac_from_ints(1, 2, 1, 3);  // 1/2 + 1/3i
    dc_complex_frac b = dc_frac_from_ints(1, 4, 1, 6);  // 1/4 + 1/6i

    // Addition: (1/2 + 1/3i) + (1/4 + 1/6i) = 3/4 + 1/2i
    dc_complex_frac sum = dc_frac_add(a, b);
    df_frac sum_real = dc_frac_real(sum);
    df_frac sum_imag = dc_frac_imag(sum);

    TEST_ASSERT_EQUAL_DOUBLE(0.75, df_to_double(sum_real));
    TEST_ASSERT_EQUAL_DOUBLE(0.5, df_to_double(sum_imag));

    // Test conjugate
    dc_complex_frac conj = dc_frac_conj(a);
    df_frac conj_imag = dc_frac_imag(conj);
    TEST_ASSERT_EQUAL_DOUBLE(-1.0/3.0, df_to_double(conj_imag));

    // Cleanup
    dc_frac_release(&a);
    dc_frac_release(&b);
    dc_frac_release(&sum);
    dc_frac_release(&conj);
    df_release(&sum_real);
    df_release(&sum_imag);
    df_release(&conj_imag);
}

void test_dc_frac_gaussian_int_predicate(void) {
    dc_complex_frac a = dc_frac_from_ints(3, 1, 4, 1);  // 3 + 4i (Gaussian int)
    dc_complex_frac b = dc_frac_from_ints(1, 2, 3, 4);  // 1/2 + 3/4i (not Gaussian int)

    TEST_ASSERT_TRUE(dc_frac_is_gaussian_int(a));
    TEST_ASSERT_FALSE(dc_frac_is_gaussian_int(b));

    dc_frac_release(&a);
    dc_frac_release(&b);
}

void test_dc_frac_missing_creation(void) {
    // Test dc_frac_from_df
    df_frac real = df_from_ints(2, 3);
    df_frac imag = df_from_ints(-1, 4);
    dc_complex_frac a = dc_frac_from_df(real, imag);

    df_frac a_real = dc_frac_real(a);
    df_frac a_imag = dc_frac_imag(a);

    TEST_ASSERT_EQUAL_DOUBLE(2.0/3.0, df_to_double(a_real));
    TEST_ASSERT_EQUAL_DOUBLE(-1.0/4.0, df_to_double(a_imag));

    // Test dc_frac_neg_one and dc_frac_neg_i
    dc_complex_frac neg_one = dc_frac_neg_one();
    dc_complex_frac neg_i = dc_frac_neg_i();

    df_frac neg_one_real = dc_frac_real(neg_one);
    df_frac neg_one_imag = dc_frac_imag(neg_one);
    df_frac neg_i_real = dc_frac_real(neg_i);
    df_frac neg_i_imag = dc_frac_imag(neg_i);

    TEST_ASSERT_EQUAL_DOUBLE(-1.0, df_to_double(neg_one_real));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, df_to_double(neg_one_imag));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, df_to_double(neg_i_real));
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, df_to_double(neg_i_imag));

    // Cleanup
    df_release(&real);
    df_release(&imag);
    dc_frac_release(&a);
    dc_frac_release(&neg_one);
    dc_frac_release(&neg_i);
    df_release(&a_real);
    df_release(&a_imag);
    df_release(&neg_one_real);
    df_release(&neg_one_imag);
    df_release(&neg_i_real);
    df_release(&neg_i_imag);
}

void test_dc_frac_memory_management(void) {
    // Test dc_frac_retain and dc_frac_copy
    dc_complex_frac a = dc_frac_from_ints(3, 4, 1, 2);
    dc_complex_frac b = dc_frac_retain(a);
    dc_complex_frac c = dc_frac_copy(a);

    TEST_ASSERT_EQUAL_PTR(a, b);  // Same object
    TEST_ASSERT_FALSE(a == c);  // Different objects
    TEST_ASSERT_TRUE(dc_frac_eq(a, c));  // Same value

    dc_frac_release(&a);
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);

    dc_frac_release(&b);
    dc_frac_release(&c);
}

void test_dc_frac_complete_arithmetic(void) {
    dc_complex_frac a = dc_frac_from_ints(3, 4, 1, 2);  // 3/4 + 1/2i
    dc_complex_frac b = dc_frac_from_ints(1, 3, 2, 5);  // 1/3 + 2/5i

    // Test subtraction: (3/4 + 1/2i) - (1/3 + 2/5i) = 5/12 + 1/10i
    dc_complex_frac diff = dc_frac_sub(a, b);
    df_frac diff_real = dc_frac_real(diff);
    df_frac diff_imag = dc_frac_imag(diff);
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 5.0/12.0, df_to_double(diff_real));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0/10.0, df_to_double(diff_imag));

    // Test multiplication: (3/4 + 1/2i) * (1/3 + 2/5i)
    // = (3/4 * 1/3 - 1/2 * 2/5) + (3/4 * 2/5 + 1/2 * 1/3)i
    // = (3/12 - 2/10) + (6/20 + 1/6)i
    // = (1/4 - 1/5) + (3/10 + 1/6)i
    // = (5/20 - 4/20) + (9/30 + 5/30)i
    // = 1/20 + 14/30i = 1/20 + 7/15i
    dc_complex_frac prod = dc_frac_mul(a, b);
    df_frac prod_real = dc_frac_real(prod);
    df_frac prod_imag = dc_frac_imag(prod);
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0/20.0, df_to_double(prod_real));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 7.0/15.0, df_to_double(prod_imag));

    // Test division
    dc_complex_frac quot = dc_frac_div(a, b);
    TEST_ASSERT_NOT_NULL(quot);

    // Test negate
    dc_complex_frac neg_a = dc_frac_negate(a);
    df_frac neg_a_real = dc_frac_real(neg_a);
    df_frac neg_a_imag = dc_frac_imag(neg_a);
    TEST_ASSERT_EQUAL_DOUBLE(-3.0/4.0, df_to_double(neg_a_real));
    TEST_ASSERT_EQUAL_DOUBLE(-1.0/2.0, df_to_double(neg_a_imag));

    // Test reciprocal
    dc_complex_frac recip = dc_frac_reciprocal(a);
    TEST_ASSERT_NOT_NULL(recip);

    // Cleanup
    dc_frac_release(&a);
    dc_frac_release(&b);
    dc_frac_release(&diff);
    dc_frac_release(&prod);
    dc_frac_release(&quot);
    dc_frac_release(&neg_a);
    dc_frac_release(&recip);
    df_release(&diff_real);
    df_release(&diff_imag);
    df_release(&prod_real);
    df_release(&prod_imag);
    df_release(&neg_a_real);
    df_release(&neg_a_imag);
}

void test_dc_frac_comparisons_and_string(void) {
    dc_complex_frac a = dc_frac_from_ints(3, 4, 1, 2);  // 3/4 + 1/2i
    dc_complex_frac b = dc_frac_from_ints(3, 4, 1, 2);  // Same value
    dc_complex_frac c = dc_frac_from_ints(1, 2, 0, 1);  // 1/2 + 0i (real)

    // Test dc_frac_eq
    TEST_ASSERT_TRUE(dc_frac_eq(a, b));
    TEST_ASSERT_FALSE(dc_frac_eq(a, c));

    // Test string conversion
    char* str_a = dc_frac_to_string(a);
    char* str_c = dc_frac_to_string(c);
    dc_complex_frac zero = dc_frac_zero();
    char* str_zero = dc_frac_to_string(zero);
    dc_complex_frac i = dc_frac_i();
    char* str_i = dc_frac_to_string(i);

    TEST_ASSERT_NOT_NULL(str_a);
    TEST_ASSERT_NOT_NULL(str_c);
    TEST_ASSERT_EQUAL_STRING("0", str_zero);
    TEST_ASSERT_EQUAL_STRING("i", str_i);

    // Cleanup
    dc_frac_release(&a);
    dc_frac_release(&b);
    dc_frac_release(&c);
    dc_frac_release(&zero);
    dc_frac_release(&i);
    free(str_a);
    free(str_c);
    free(str_zero);
    free(str_i);
}

// ============================================================================
// DOUBLE COMPLEX TESTS
// ============================================================================

void test_dc_double_creation(void) {
    dc_complex_double a = dc_double_from_doubles(3.5, 4.5);
    dc_complex_double b = dc_double_from_polar(1.0, M_PI/4);
    dc_complex_double c = dc_double_zero();
    dc_complex_double d = dc_double_one();
    dc_complex_double e = dc_double_i();

    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_NOT_NULL(d);
    TEST_ASSERT_NOT_NULL(e);

    TEST_ASSERT_EQUAL_DOUBLE(3.5, dc_double_real(a));
    TEST_ASSERT_EQUAL_DOUBLE(4.5, dc_double_imag(a));

    TEST_ASSERT_DOUBLE_WITHIN(1e-10, sqrt(2)/2, dc_double_real(b));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, sqrt(2)/2, dc_double_imag(b));

    TEST_ASSERT_TRUE(dc_double_is_zero(c));
    TEST_ASSERT_TRUE(dc_double_is_real(d));
    TEST_ASSERT_TRUE(dc_double_is_imag(e));

    dc_double_release(&a);
    dc_double_release(&b);
    dc_double_release(&c);
    dc_double_release(&d);
    dc_double_release(&e);
}

void test_dc_double_arithmetic(void) {
    dc_complex_double a = dc_double_from_doubles(3.0, 4.0);  // 3 + 4i
    dc_complex_double b = dc_double_from_doubles(1.0, -2.0); // 1 - 2i

    // Addition
    dc_complex_double sum = dc_double_add(a, b);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, dc_double_real(sum));
    TEST_ASSERT_EQUAL_DOUBLE(2.0, dc_double_imag(sum));

    // Multiplication
    dc_complex_double prod = dc_double_mul(a, b);
    TEST_ASSERT_EQUAL_DOUBLE(11.0, dc_double_real(prod));
    TEST_ASSERT_EQUAL_DOUBLE(-2.0, dc_double_imag(prod));

    // Magnitude
    TEST_ASSERT_EQUAL_DOUBLE(5.0, dc_double_abs(a));

    // Phase
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, atan2(4.0, 3.0), dc_double_arg(a));

    dc_double_release(&a);
    dc_double_release(&b);
    dc_double_release(&sum);
    dc_double_release(&prod);
}

void test_dc_double_transcendental(void) {
    dc_complex_double a = dc_double_from_doubles(1.0, 0.0);  // 1 + 0i
    dc_complex_double b = dc_double_i();                     // 0 + 1i

    // e^(i*pi/2) = i
    dc_complex_double exp_result = dc_double_exp(dc_double_from_doubles(0.0, M_PI/2));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(exp_result));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0, dc_double_imag(exp_result));

    // sqrt(-1) = i
    dc_complex_double neg_one = dc_double_from_doubles(-1.0, 0.0);
    dc_complex_double sqrt_result = dc_double_sqrt(neg_one);
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(sqrt_result));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0, dc_double_imag(sqrt_result));

    dc_double_release(&a);
    dc_double_release(&b);
    dc_double_release(&exp_result);
    dc_double_release(&neg_one);
    dc_double_release(&sqrt_result);
}

void test_dc_double_missing_creation(void) {
    // Test dc_double_neg_one and dc_double_neg_i
    dc_complex_double neg_one = dc_double_neg_one();
    dc_complex_double neg_i = dc_double_neg_i();

    TEST_ASSERT_EQUAL_DOUBLE(-1.0, dc_double_real(neg_one));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, dc_double_imag(neg_one));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, dc_double_real(neg_i));
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, dc_double_imag(neg_i));

    dc_double_release(&neg_one);
    dc_double_release(&neg_i);
}

void test_dc_double_memory_management(void) {
    // Test dc_double_retain and dc_double_copy
    dc_complex_double a = dc_double_from_doubles(3.14, 2.71);
    dc_complex_double b = dc_double_retain(a);
    dc_complex_double c = dc_double_copy(a);

    TEST_ASSERT_EQUAL_PTR(a, b);  // Same object
    TEST_ASSERT_FALSE(a == c);  // Different objects
    TEST_ASSERT_TRUE(dc_double_eq(a, c));  // Same value

    dc_double_release(&a);
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);

    dc_double_release(&b);
    dc_double_release(&c);
}

void test_dc_double_complete_arithmetic(void) {
    dc_complex_double a = dc_double_from_doubles(3.0, 4.0);  // 3 + 4i
    dc_complex_double b = dc_double_from_doubles(1.0, -2.0); // 1 - 2i

    // Test subtraction: (3 + 4i) - (1 - 2i) = 2 + 6i
    dc_complex_double diff = dc_double_sub(a, b);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, dc_double_real(diff));
    TEST_ASSERT_EQUAL_DOUBLE(6.0, dc_double_imag(diff));

    // Test division: (3 + 4i) / (1 - 2i) = -1 + 2i
    dc_complex_double quot = dc_double_div(a, b);
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, -1.0, dc_double_real(quot));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 2.0, dc_double_imag(quot));

    // Test negate
    dc_complex_double neg_a = dc_double_negate(a);
    TEST_ASSERT_EQUAL_DOUBLE(-3.0, dc_double_real(neg_a));
    TEST_ASSERT_EQUAL_DOUBLE(-4.0, dc_double_imag(neg_a));

    // Test conjugate
    dc_complex_double conj_a = dc_double_conj(a);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, dc_double_real(conj_a));
    TEST_ASSERT_EQUAL_DOUBLE(-4.0, dc_double_imag(conj_a));

    // Cleanup
    dc_double_release(&a);
    dc_double_release(&b);
    dc_double_release(&diff);
    dc_double_release(&quot);
    dc_double_release(&neg_a);
    dc_double_release(&conj_a);
}

void test_dc_double_all_transcendental(void) {
    dc_complex_double z = dc_double_from_doubles(1.0, 1.0);  // 1 + i

    // Test log: log(e) = 1
    dc_complex_double e = dc_double_from_doubles(M_E, 0.0);
    dc_complex_double log_e = dc_double_log(e);
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0, dc_double_real(log_e));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_imag(log_e));

    // Test pow: (1+i)^2 = 2i
    dc_complex_double two = dc_double_from_doubles(2.0, 0.0);
    dc_complex_double z_squared = dc_double_pow(z, two);
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(z_squared));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 2.0, dc_double_imag(z_squared));

    // Test trigonometric functions
    dc_complex_double zero = dc_double_from_doubles(0.0, 0.0);
    dc_complex_double pi_half = dc_double_from_doubles(M_PI/2, 0.0);

    dc_complex_double sin_zero = dc_double_sin(zero);
    dc_complex_double cos_zero = dc_double_cos(zero);
    dc_complex_double tan_zero = dc_double_tan(zero);

    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(sin_zero));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0, dc_double_real(cos_zero));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(tan_zero));

    // Test hyperbolic functions
    dc_complex_double sinh_zero = dc_double_sinh(zero);
    dc_complex_double cosh_zero = dc_double_cosh(zero);
    dc_complex_double tanh_zero = dc_double_tanh(zero);

    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(sinh_zero));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 1.0, dc_double_real(cosh_zero));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.0, dc_double_real(tanh_zero));

    // Cleanup
    dc_double_release(&z);
    dc_double_release(&e);
    dc_double_release(&log_e);
    dc_double_release(&two);
    dc_double_release(&z_squared);
    dc_double_release(&zero);
    dc_double_release(&pi_half);
    dc_double_release(&sin_zero);
    dc_double_release(&cos_zero);
    dc_double_release(&tan_zero);
    dc_double_release(&sinh_zero);
    dc_double_release(&cosh_zero);
    dc_double_release(&tanh_zero);
}

void test_dc_double_comparisons_and_special(void) {
    dc_complex_double a = dc_double_from_doubles(3.14, 2.71);
    dc_complex_double b = dc_double_from_doubles(3.14, 2.71);
    dc_complex_double c = dc_double_from_doubles(1.0, 0.0);

    // Test dc_double_eq
    TEST_ASSERT_TRUE(dc_double_eq(a, b));
    TEST_ASSERT_FALSE(dc_double_eq(a, c));

    // Test NaN and infinity detection
    dc_complex_double nan_complex = dc_double_from_doubles(NAN, 0.0);
    dc_complex_double inf_complex = dc_double_from_doubles(INFINITY, 0.0);
    dc_complex_double normal = dc_double_from_doubles(1.0, 2.0);

    TEST_ASSERT_TRUE(dc_double_is_nan(nan_complex));
    TEST_ASSERT_FALSE(dc_double_is_nan(normal));

    TEST_ASSERT_TRUE(dc_double_is_inf(inf_complex));
    TEST_ASSERT_FALSE(dc_double_is_inf(normal));

    // Test string conversion
    char* str_a = dc_double_to_string(a);
    char* str_normal = dc_double_to_string(normal);
    dc_complex_double zero = dc_double_zero();
    char* str_zero = dc_double_to_string(zero);
    dc_complex_double i = dc_double_i();
    char* str_i = dc_double_to_string(i);

    TEST_ASSERT_NOT_NULL(str_a);
    TEST_ASSERT_NOT_NULL(str_normal);
    TEST_ASSERT_EQUAL_STRING("0", str_zero);
    TEST_ASSERT_EQUAL_STRING("i", str_i);

    // Cleanup
    dc_double_release(&a);
    dc_double_release(&b);
    dc_double_release(&c);
    dc_double_release(&nan_complex);
    dc_double_release(&inf_complex);
    dc_double_release(&normal);
    dc_double_release(&zero);
    dc_double_release(&i);
    free(str_a);
    free(str_normal);
    free(str_zero);
    free(str_i);
}

// ============================================================================
// TYPE CONVERSION TESTS
// ============================================================================

void test_type_conversions(void) {
    // Integer to fraction (lossless)
    dc_complex_int a = dc_int_from_ints(3, 4);
    dc_complex_frac af = dc_int_to_frac(a);

    TEST_ASSERT_TRUE(dc_frac_is_gaussian_int(af));

    df_frac af_real = dc_frac_real(af);
    df_frac af_imag = dc_frac_imag(af);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, df_to_double(af_real));
    TEST_ASSERT_EQUAL_DOUBLE(4.0, df_to_double(af_imag));

    // Integer to double (lossless for small integers)
    dc_complex_double ad = dc_int_to_double(a);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, dc_double_real(ad));
    TEST_ASSERT_EQUAL_DOUBLE(4.0, dc_double_imag(ad));

    // Fraction to double
    dc_complex_frac f = dc_frac_from_ints(1, 2, 3, 4);
    dc_complex_double fd = dc_frac_to_double(f);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, dc_double_real(fd));
    TEST_ASSERT_EQUAL_DOUBLE(0.75, dc_double_imag(fd));

    // Double to integer (with rounding)
    dc_complex_double d = dc_double_from_doubles(3.7, 4.3);
    dc_complex_int di = dc_double_to_int(d);
    di_int di_real = dc_int_real(di);
    di_int di_imag = dc_int_imag(di);

    int32_t real_val, imag_val;
    TEST_ASSERT_TRUE(di_to_int32(di_real, &real_val));
    TEST_ASSERT_TRUE(di_to_int32(di_imag, &imag_val));
    TEST_ASSERT_EQUAL_INT32(4, real_val);  // rounded from 3.7
    TEST_ASSERT_EQUAL_INT32(4, imag_val);  // rounded from 4.3

    // Cleanup
    dc_int_release(&a);
    dc_frac_release(&af);
    dc_double_release(&ad);
    dc_frac_release(&f);
    dc_double_release(&fd);
    dc_double_release(&d);
    dc_int_release(&di);
    df_release(&af_real);
    df_release(&af_imag);
    di_release(&di_real);
    di_release(&di_imag);
}

void test_remaining_type_conversions(void) {
    // Test dc_double_to_frac
    dc_complex_double d = dc_double_from_doubles(0.75, 0.5);  // 3/4 + 1/2i
    dc_complex_frac df = dc_double_to_frac(d, 100);  // max denominator 100

    df_frac df_real = dc_frac_real(df);
    df_frac df_imag = dc_frac_imag(df);

    // Should approximate to exact fractions
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.75, df_to_double(df_real));
    TEST_ASSERT_DOUBLE_WITHIN(1e-10, 0.5, df_to_double(df_imag));

    // Test with irrational number - should be approximated
    dc_complex_double pi_e = dc_double_from_doubles(M_PI, M_E);
    dc_complex_frac pi_e_frac = dc_double_to_frac(pi_e, 1000);

    df_frac pi_frac_real = dc_frac_real(pi_e_frac);
    df_frac e_frac_imag = dc_frac_imag(pi_e_frac);

    // Should be close approximations
    TEST_ASSERT_DOUBLE_WITHIN(0.01, M_PI, df_to_double(pi_frac_real));
    TEST_ASSERT_DOUBLE_WITHIN(0.01, M_E, df_to_double(e_frac_imag));

    // Cleanup
    dc_double_release(&d);
    dc_frac_release(&df);
    dc_double_release(&pi_e);
    dc_frac_release(&pi_e_frac);
    df_release(&df_real);
    df_release(&df_imag);
    df_release(&pi_frac_real);
    df_release(&e_frac_imag);
}

// ============================================================================
// EDGE CASES AND ERROR CONDITIONS
// ============================================================================

void test_edge_cases(void) {
    // Test zero handling
    dc_complex_int zero = dc_int_zero();
    dc_complex_int one = dc_int_one();

    dc_complex_int result = dc_int_add(zero, one);
    TEST_ASSERT_TRUE(dc_int_eq(result, one));

    dc_complex_int mult_by_zero = dc_int_mul(one, zero);
    TEST_ASSERT_TRUE(dc_int_is_zero(mult_by_zero));

    // Test i^2 = -1
    dc_complex_int i = dc_int_i();
    dc_complex_int i_squared = dc_int_mul(i, i);
    dc_complex_int neg_one = dc_int_neg_one();
    TEST_ASSERT_TRUE(dc_int_eq(i_squared, neg_one));

    // Cleanup
    dc_int_release(&zero);
    dc_int_release(&one);
    dc_int_release(&result);
    dc_int_release(&mult_by_zero);
    dc_int_release(&i);
    dc_int_release(&i_squared);
    dc_int_release(&neg_one);
}

void test_string_formatting(void) {
    // Test various string formats
    dc_complex_int a = dc_int_from_ints(0, 0);   // 0
    dc_complex_int b = dc_int_from_ints(5, 0);   // 5
    dc_complex_int c = dc_int_from_ints(0, 3);   // 3i
    dc_complex_int d = dc_int_from_ints(0, 1);   // i
    dc_complex_int e = dc_int_from_ints(0, -1);  // -i
    dc_complex_int f = dc_int_from_ints(2, 3);   // 2+3i
    dc_complex_int g = dc_int_from_ints(2, -3);  // 2-3i

    char* str_a = dc_int_to_string(a);
    char* str_b = dc_int_to_string(b);
    char* str_c = dc_int_to_string(c);
    char* str_d = dc_int_to_string(d);
    char* str_e = dc_int_to_string(e);
    char* str_f = dc_int_to_string(f);
    char* str_g = dc_int_to_string(g);

    TEST_ASSERT_EQUAL_STRING("0", str_a);
    TEST_ASSERT_EQUAL_STRING("5", str_b);
    TEST_ASSERT_EQUAL_STRING("3i", str_c);
    TEST_ASSERT_EQUAL_STRING("i", str_d);
    TEST_ASSERT_EQUAL_STRING("-i", str_e);
    TEST_ASSERT_EQUAL_STRING("2+3i", str_f);
    TEST_ASSERT_EQUAL_STRING("2-3i", str_g);

    // Cleanup
    dc_int_release(&a);
    dc_int_release(&b);
    dc_int_release(&c);
    dc_int_release(&d);
    dc_int_release(&e);
    dc_int_release(&f);
    dc_int_release(&g);
    free(str_a);
    free(str_b);
    free(str_c);
    free(str_d);
    free(str_e);
    free(str_f);
    free(str_g);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void) {
    UNITY_BEGIN();

    // Integer complex tests
    RUN_TEST(test_dc_int_creation);
    RUN_TEST(test_dc_int_arithmetic);
    RUN_TEST(test_dc_int_division);
    RUN_TEST(test_dc_int_string_conversion);
    RUN_TEST(test_dc_int_memory_management);
    RUN_TEST(test_dc_int_missing_functions);

    // Fraction complex tests
    RUN_TEST(test_dc_frac_creation);
    RUN_TEST(test_dc_frac_arithmetic);
    RUN_TEST(test_dc_frac_gaussian_int_predicate);
    RUN_TEST(test_dc_frac_missing_creation);
    RUN_TEST(test_dc_frac_memory_management);
    RUN_TEST(test_dc_frac_complete_arithmetic);
    RUN_TEST(test_dc_frac_comparisons_and_string);

    // Double complex tests
    RUN_TEST(test_dc_double_creation);
    RUN_TEST(test_dc_double_arithmetic);
    RUN_TEST(test_dc_double_transcendental);
    RUN_TEST(test_dc_double_missing_creation);
    RUN_TEST(test_dc_double_memory_management);
    RUN_TEST(test_dc_double_complete_arithmetic);
    RUN_TEST(test_dc_double_all_transcendental);
    RUN_TEST(test_dc_double_comparisons_and_special);

    // Type conversion tests
    RUN_TEST(test_type_conversions);
    RUN_TEST(test_remaining_type_conversions);

    // Edge cases
    RUN_TEST(test_edge_cases);
    RUN_TEST(test_string_formatting);

    return UNITY_END();
}
