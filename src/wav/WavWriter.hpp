#pragma once
#include "IWavWriter.hpp"

/**
 * @brief Concrete WAV writer — writes the provided header and PCM samples
 *        verbatim to disk.
 * @see IWavWriter
 */
class WavWriter : public IWavWriter {
public:
    void write(const std::string& path,
               const std::array<uint8_t, 44>& header,
               const std::vector<int16_t>& samples) override;
};
