#pragma once
#include <vector>
#include "Complex.hpp"

/**
 * @brief Interface for a discrete Fourier transform.
 *
 * Implementors must map a sequence of real-valued samples to the complex
 * frequency domain using the sign convention defined by the concrete class.
 */
struct IDft {
    /**
     * @brief Computes the DFT of the given sample sequence.
     * @param samples Real-valued time-domain signal of length N.
     * @return Complex spectrum of length N; bin k represents frequency k·(fs/N).
     */
    virtual std::vector<Complex> transform(const std::vector<double>& samples) = 0;
    virtual ~IDft() = default;
};
