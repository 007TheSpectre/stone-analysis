#pragma once
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Interface for extracting a text message hidden by ICypher::encode().
 */
struct IDecypher {
    /**
     * @brief Scans @p samples window by window and reconstructs the message.
     * @param samples  16-bit PCM buffer as produced by ICypher::encode().
     * @return Decoded message in uppercase, or an empty string if no
     *         hidden message is found.
     */
    virtual std::string decode(const std::vector<int16_t>& samples) = 0;
    virtual ~IDecypher() = default;
};
