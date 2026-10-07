#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Interface for reading a 16-bit mono PCM WAV file.
 *
 * After a successful call to open(), rawHeader() returns the original 44-byte
 * RIFF/WAVE header verbatim and samples() returns the signed PCM data.
 * Throws std::runtime_error for any format violation.
 */
struct IWavReader {
    /**
     * @brief Opens and validates the WAV file at @p path.
     * @param path Filesystem path to the input WAV file.
     * @throws std::runtime_error if the file cannot be opened or does not
     *         conform to 16-bit mono PCM at 48 kHz.
     */
    virtual void open(const std::string& path) = 0;

    /**
     * @brief Returns the raw 44-byte RIFF/WAVE header as read from disk.
     * @return Byte array suitable for verbatim reuse by IWavWriter::write().
     */
    virtual std::array<uint8_t, 44> rawHeader() const = 0;

    /**
     * @brief Returns all PCM samples in read order.
     * @return Signed 16-bit samples, one per channel frame.
     */
    virtual std::vector<int16_t> samples() const = 0;

    virtual ~IWavReader() = default;
};
