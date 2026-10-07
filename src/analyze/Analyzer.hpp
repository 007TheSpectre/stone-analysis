#pragma once
#include "IAnalyzer.hpp"

/**
 * @brief Concrete frequency analyzer using the mixed-radix FFT.
 *
 * Converts each int16 sample to double, computes the full DFT, sorts bins
 * by magnitude in descending order, and prints `freq_hz : magnitude` lines.
 *
 * @see IAnalyzer
 */
class Analyzer : public IAnalyzer {
public:
    void analyze(const std::vector<int16_t>& samples,
                 int topN, uint32_t sampleRate) override;
};
