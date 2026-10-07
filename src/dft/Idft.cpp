#include "Idft.hpp"
#include "Fft.hpp"

std::vector<double> Idft::transform(const std::vector<Complex>& spectrum) {
    size_t N = spectrum.size();
    std::vector<Complex> result = dftAny(spectrum, true);
    std::vector<double> samples(N);
    for (size_t i = 0; i < N; ++i)
        samples[i] = result[i].real / static_cast<double>(N);
    return samples;
}
