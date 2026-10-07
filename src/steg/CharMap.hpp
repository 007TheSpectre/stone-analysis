#pragma once
#include <string>
#include <vector>
#include "dft/Complex.hpp"

/// @brief Window size in samples (2400 samples ≈ 50 ms at 48 kHz).
static constexpr int    WINDOW_SIZE         = 2400;

/// @brief First bin of the ultrasound range; used as the message terminator.
static constexpr int    ULTRASOUND_BASE_BIN = 1000;

/// @brief Last bin of the ultrasound message band (terminator + space + 10 digits + 26 letters).
static constexpr int    ULTRASOUND_TOP_BIN  = ULTRASOUND_BASE_BIN + 37;

/// @brief Magnitude injected into the spectrum to encode a character.
static constexpr double ENCODE_MAGNITUDE    = 10000.0;

/// @brief Minimum magnitude to consider a bin as carrying an encoded character.
static constexpr double DETECT_THRESHOLD    = 5000.0;

/**
 * @brief Maps a character to its ultrasound frequency bin.
 *
 * | Input      | Bin range       |
 * |------------|-----------------|
 * | `' '`      | 1001            |
 * | `'0'–'9'`  | 1002–1011       |
 * | `'a'–'z'`  | 1012–1037       |
 *
 * @param c Lowercase letter, digit, or space.
 * @return Frequency bin index, or -1 if @p c is not a supported character.
 */
int charToBin(char c);

/**
 * @brief Maps a frequency bin back to its character.
 * @param bin Frequency bin index in the ultrasound range.
 * @return The corresponding character, or `'\0'` for an unrecognised bin.
 */
char binToChar(int bin);

/**
 * @brief Converts @p msg to lowercase and validates every character.
 * @param msg Input string (letters, digits, spaces).
 * @return Lowercase version of @p msg.
 * @throws std::runtime_error if @p msg contains an unsupported character.
 */
std::string normalizeMessage(const std::string& msg);

/**
 * @brief Encodes one symbol by making @p bin the sole occupant of the
 *        ultrasound message band.
 *
 * Every bin in @f$[\text{ULTRASOUND\_BASE\_BIN}, \text{ULTRASOUND\_TOP\_BIN}]@f$
 * (and its conjugate) is first zeroed so that whatever the carrier already held
 * in the band cannot mask or outvote the symbol. The single @p bin is then set,
 * with conjugate symmetry preserved so the IDFT output stays real-valued.
 *
 * @f[
 *   X[\text{bin}] = M \cdot e^{i\varphi}, \quad
 *   X[N-\text{bin}] = \overline{X[\text{bin}]}
 * @f]
 *
 * @param spectrum Frequency-domain buffer of length N (modified in place).
 * @param N        Transform size (must equal @p spectrum.size()).
 * @param bin      Target frequency bin (must satisfy 0 < bin < N/2).
 * @param M        Sinusoid magnitude.
 * @param phi      Initial phase in radians.
 */
void plantCharacter(std::vector<Complex>& spectrum, size_t N, int bin,
                    double M, double phi);
