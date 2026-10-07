#pragma once
#include <vector>
#include "Complex.hpp"

/**
 * @brief In-place iterative radix-2 FFT (Cooley-Tukey).
 *
 * @param a      Buffer whose size **must** be a power of two; transformed in place.
 * @param invert When true, uses the +2πi sign convention. No 1/N normalisation
 *               is applied in either direction.
 */
void fftPow2(std::vector<Complex>& a, bool invert);

/**
 * @brief Discrete Fourier transform of an arbitrary length in O(N log N).
 *
 * Powers of two are transformed directly with ::fftPow2; every other length
 * (including primes) goes through Bluestein's algorithm, which expresses the
 * DFT as a convolution evaluated with power-of-two FFTs.
 *
 * @param in     Complex input of length N.
 * @param invert When true, computes the inverse-sign transform. No 1/N
 *               normalisation is applied — callers divide by N themselves.
 * @return Complex transform of length N.
 */
std::vector<Complex> dftAny(const std::vector<Complex>& in, bool invert);
