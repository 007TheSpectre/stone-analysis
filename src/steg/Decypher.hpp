#pragma once
#include "IDecypher.hpp"

/**
 * @brief Concrete steganographic decoder.
 *
 * Iterates over 2400-sample windows, computes the FFT of each, finds the
 * peak bin in the ultrasound range, and stops when the terminator bin or
 * silence is encountered.
 *
 * @see IDecypher
 */
class Decypher : public IDecypher {
public:
    std::string decode(const std::vector<int16_t>& samples) override;
};
