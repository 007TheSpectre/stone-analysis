#include "Dft.hpp"
#include "Fft.hpp"

std::vector<Complex> Dft::transform(const std::vector<double>& samples) {
    std::vector<Complex> x(samples.size());
    for (size_t i = 0; i < samples.size(); ++i)
        x[i] = {samples[i], 0.0};
    return dftAny(x, false);
}
