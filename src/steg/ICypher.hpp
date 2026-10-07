#pragma once
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Interface for hiding a text message inside a PCM sample buffer.
 *
 * The encoding works entirely in the ultrasound range (>20 kHz) so the
 * audible content of the file is perceptually unchanged.
 */
struct ICypher {
    /**
     * @brief Encodes @p message into a copy of @p samples.
     * @param samples  Original 16-bit PCM buffer; length must be at least
     *                 `(message.size() + 1) * WINDOW_SIZE`.
     * @param message  Text to hide (letters, digits, spaces; case-insensitive).
     * @return New sample buffer of the same length with the message embedded.
     * @throws std::runtime_error if @p samples is too short or @p message
     *         contains unsupported characters.
     */
    virtual std::vector<int16_t> encode(const std::vector<int16_t>& samples,
                                        const std::string& message) = 0;
    virtual ~ICypher() = default;
};
