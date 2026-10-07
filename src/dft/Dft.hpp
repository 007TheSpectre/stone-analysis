#pragma once
#include "IDft.hpp"

/**
 * @brief Fast Fourier transform — forward transform (exponent sign: −2πi).
 *
 * Delegates to ::dftAny: a power-of-two radix-2 FFT, or Bluestein's algorithm
 * for any other length (including primes), so every input is O(N log N).
 *
 * @f[
 *   X(k) = \sum_{n=0}^{N-1} x(n)\, e^{-2\pi i k n / N}
 * @f]
 */
class Dft : public IDft {
public:
    /**
     * @brief Computes the forward FFT of @p samples.
     * @param samples Real-valued time-domain input of length N.
     * @return Complex spectrum of length N.
     */
    std::vector<Complex> transform(const std::vector<double>& samples) override;
};
