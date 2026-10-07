#pragma once
#include <vector>
#include "Complex.hpp"

/**
 * @brief Inverse fast Fourier transform — reconstructs the time-domain signal.
 *
 * Uses the same ::dftAny engine as Dft but with the conjugate sign convention
 * and a final division by N:
 *
 * @f[
 *   x(n) = \frac{1}{N} \sum_{k=0}^{N-1} X(k)\, e^{+2\pi i k n / N}
 * @f]
 */
class Idft {
public:
    /**
     * @brief Computes the inverse FFT of @p spectrum.
     * @param spectrum Complex frequency-domain input of length N.
     * @return Real-valued time-domain output of length N.
     */
    std::vector<double> transform(const std::vector<Complex>& spectrum);
};
