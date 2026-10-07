#pragma once
#include "IWavReader.hpp"

/**
 * @brief Concrete WAV reader — validates the RIFF/WAVE/PCM header and
 *        loads signed 16-bit mono samples at 48 kHz.
 * @see IWavReader
 */
class WavReader : public IWavReader {
public:
    void                    open(const std::string& path) override;
    std::array<uint8_t, 44> rawHeader() const override;
    std::vector<int16_t>    samples() const override;

private:
    std::array<uint8_t, 44> _header{};
    std::vector<int16_t>    _samples;
};
