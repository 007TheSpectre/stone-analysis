#pragma once
#include <cmath>

/// @brief Cartesian representation of a complex number.
struct Complex {
    double real; ///< Real part.
    double imag; ///< Imaginary part.
};

/// @brief Component-wise addition.
inline Complex operator+(Complex a, Complex b) { return {a.real + b.real, a.imag + b.imag}; }

/// @brief Component-wise subtraction.
inline Complex operator-(Complex a, Complex b) { return {a.real - b.real, a.imag - b.imag}; }

/// @brief Complex multiplication: (a+bi)(c+di) = (ac-bd) + (ad+bc)i.
inline Complex operator*(Complex a, Complex b) {
    return {a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real};
}

/// @brief Scalar multiplication.
inline Complex operator*(double s, Complex c) { return {s * c.real, s * c.imag}; }

/// @brief In-place addition.
inline Complex operator+=(Complex& a, Complex b) { a = a + b; return a; }

/// @brief Returns |c| = sqrt(re² + im²).
inline double  magnitude(Complex c) { return std::sqrt(c.real * c.real + c.imag * c.imag); }

/// @brief Returns the complex conjugate (re, -im).
inline Complex conjugate(Complex c) { return {c.real, -c.imag}; }

/// @brief Returns e^(i·angle) = cos(angle) + i·sin(angle).
inline Complex complexExp(double angle) { return {std::cos(angle), std::sin(angle)}; }
