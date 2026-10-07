#pragma once
#include <cstdint>
#include <vector>

/**
 * @brief Interface for frequency spectrum analysis.
 *
 * Prints the top N frequency bins sorted by magnitude to standard output.
 */
struct IAnalyzer {
    /**
     * @brief Runs a DFT on @p samples and prints the @p topN strongest bins.
     * @param samples    16-bit mono PCM input.
     * @param topN       Number of frequency bins to report (clamped to N/2).
     * @param sampleRate Sampling frequency in Hz (used to convert bin → Hz).
     */
    virtual void analyze(const std::vector<int16_t>& samples,
                         int topN, uint32_t sampleRate) = 0;
    virtual ~IAnalyzer() = default;
};
