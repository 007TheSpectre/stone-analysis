#pragma once
#include "ICypher.hpp"

/**
 * @brief Concrete steganographic encoder.
 *
 * For each character in the message (plus a terminator), takes one 2400-sample
 * window, runs the FFT, injects a sinusoid at the character's ultrasound bin
 * via addSinusoid(), runs the IFFT, and clamps the result to int16.
 *
 * @see ICypher
 */
class Cypher : public ICypher {
public:
    std::vector<int16_t> encode(const std::vector<int16_t>& samples,
                                const std::string& message) override;
};
