#include <criterion/criterion.h>
#include <cmath>
#include "dft/Complex.hpp"

static constexpr double EPS = 1e-9;

Test(complex, add) {
    Complex r = Complex{1.0, 2.0} + Complex{3.0, 4.0};
    cr_assert_float_eq(r.real, 4.0, EPS);
    cr_assert_float_eq(r.imag, 6.0, EPS);
}

Test(complex, sub) {
    Complex r = Complex{3.0, 4.0} - Complex{1.0, 2.0};
    cr_assert_float_eq(r.real, 2.0, EPS);
    cr_assert_float_eq(r.imag, 2.0, EPS);
}

Test(complex, mul_complex) {
    Complex r = Complex{1.0, 2.0} * Complex{3.0, 4.0};
    cr_assert_float_eq(r.real, -5.0, EPS);
    cr_assert_float_eq(r.imag, 10.0, EPS);
}

Test(complex, mul_identity) {
    Complex r = Complex{3.0, 4.0} * Complex{1.0, 0.0};
    cr_assert_float_eq(r.real, 3.0, EPS);
    cr_assert_float_eq(r.imag, 4.0, EPS);
}

Test(complex, scalar_mul) {
    Complex r = 2.0 * Complex{3.0, 4.0};
    cr_assert_float_eq(r.real, 6.0, EPS);
    cr_assert_float_eq(r.imag, 8.0, EPS);
}

Test(complex, scalar_mul_zero) {
    Complex r = 0.0 * Complex{3.0, 4.0};
    cr_assert_float_eq(r.real, 0.0, EPS);
    cr_assert_float_eq(r.imag, 0.0, EPS);
}

Test(complex, plus_equal) {
    Complex a = {1.0, 2.0};
    a += Complex{3.0, 4.0};
    cr_assert_float_eq(a.real, 4.0, EPS);
    cr_assert_float_eq(a.imag, 6.0, EPS);
}

Test(complex, magnitude_zero) {
    cr_assert_float_eq(magnitude(Complex{0.0, 0.0}), 0.0, EPS);
}

Test(complex, magnitude_3_4) {
    cr_assert_float_eq(magnitude(Complex{3.0, 4.0}), 5.0, EPS);
}

Test(complex, magnitude_pure_real) {
    cr_assert_float_eq(magnitude(Complex{-7.0, 0.0}), 7.0, EPS);
}

Test(complex, magnitude_pure_imag) {
    cr_assert_float_eq(magnitude(Complex{0.0, -6.0}), 6.0, EPS);
}

Test(complex, conjugate_positive_imag) {
    Complex r = conjugate(Complex{3.0, 4.0});
    cr_assert_float_eq(r.real,  3.0, EPS);
    cr_assert_float_eq(r.imag, -4.0, EPS);
}

Test(complex, conjugate_negative_imag) {
    Complex r = conjugate(Complex{3.0, -4.0});
    cr_assert_float_eq(r.real, 3.0, EPS);
    cr_assert_float_eq(r.imag, 4.0, EPS);
}

Test(complex, conjugate_real_only) {
    Complex r = conjugate(Complex{5.0, 0.0});
    cr_assert_float_eq(r.real, 5.0, EPS);
    cr_assert_float_eq(r.imag, 0.0, EPS);
}

Test(complex, exp_zero) {
    Complex r = complexExp(0.0);
    cr_assert_float_eq(r.real, 1.0, EPS);
    cr_assert_float_eq(r.imag, 0.0, EPS);
}

Test(complex, exp_half_pi) {
    Complex r = complexExp(M_PI / 2.0);
    cr_assert_float_eq(r.real, 0.0, 1e-9);
    cr_assert_float_eq(r.imag, 1.0, 1e-9);
}

Test(complex, exp_pi) {
    Complex r = complexExp(M_PI);
    cr_assert_float_eq(r.real, -1.0, 1e-9);
    cr_assert_float_eq(r.imag,  0.0, 1e-9);
}

Test(complex, exp_two_pi) {
    Complex r = complexExp(2.0 * M_PI);
    cr_assert_float_eq(r.real, 1.0, 1e-9);
    cr_assert_float_eq(r.imag, 0.0, 1e-9);
}

Test(complex, mul_conjugate_gives_magnitude_squared) {
    Complex c = {3.0, 4.0};
    Complex r = c * conjugate(c);
    cr_assert_float_eq(r.real, 25.0, EPS);
    cr_assert_float_eq(r.imag,  0.0, EPS);
}
