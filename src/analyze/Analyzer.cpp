#include "Analyzer.hpp"
#include "dft/Dft.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>

void Analyzer::analyze(const std::vector<int16_t>& samples, int topN, uint32_t sampleRate) {
    size_t N = samples.size();

    std::vector<double> doubleSamples(N);
    for (size_t i = 0; i < N; ++i)
        doubleSamples[i] = static_cast<double>(samples[i]);

    Dft dft;
    std::vector<Complex> spectrum = dft.transform(doubleSamples);

    std::vector<std::pair<double, double>> freqMagnitudes;
    freqMagnitudes.reserve(N / 2);
    for (size_t k = 1; k <= N / 2; ++k) {
        double hz  = static_cast<double>(k) * sampleRate / static_cast<double>(N);
        double mag = magnitude(spectrum[k]);
        freqMagnitudes.emplace_back(hz, mag);
    }

    std::sort(freqMagnitudes.begin(), freqMagnitudes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    int count = std::min(topN, static_cast<int>(freqMagnitudes.size()));
    std::cout << "Top " << topN << " frequencies:\n";
    std::cout << std::fixed << std::setprecision(1);
    for (int i = 0; i < count; ++i)
        std::cout << freqMagnitudes[i].first << " Hz\n";
}
