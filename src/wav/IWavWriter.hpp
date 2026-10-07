#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Interface for writing a 16-bit mono PCM WAV file.
 *
 * The caller supplies the exact 44-byte header (typically obtained from
 * IWavReader::rawHeader()) so the output file is byte-for-byte compatible
 * with the original aside from the sample data.
 */
struct IWavWriter {
    /**
     * @brief Writes @p header followed by @p samples to @p path.
     * @param path      Destination file path.
     * @param header    Exact 44-byte RIFF/WAVE header to write verbatim.
     * @param samples   PCM sample data to append after the header.
     * @throws std::runtime_error if the file cannot be created.
     */
    virtual void write(const std::string& path,
                       const std::array<uint8_t, 44>& header,
                       const std::vector<int16_t>& samples) = 0;
    virtual ~IWavWriter() = default;
};
