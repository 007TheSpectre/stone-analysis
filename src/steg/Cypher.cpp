#include "Cypher.hpp"
#include "CharMap.hpp"
#include "dft/Dft.hpp"
#include "dft/Idft.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

std::vector<int16_t> Cypher::encode(const std::vector<int16_t>& samples,
                                    const std::string& message) {
    std::string msg = normalizeMessage(message);

    size_t required = (msg.size() + 1) * static_cast<size_t>(WINDOW_SIZE);
    if (required > samples.size())
        throw std::runtime_error("audio file too short to encode the message");

    std::vector<int16_t> output = samples;

    Dft  dft;
    Idft idft;

    for (size_t w = 0; w <= msg.size(); ++w) {
        size_t start = w * static_cast<size_t>(WINDOW_SIZE);

        std::vector<double> window(WINDOW_SIZE);
        for (int i = 0; i < WINDOW_SIZE; ++i)
            window[i] = static_cast<double>(output[start + i]);

        std::vector<Complex> spectrum = dft.transform(window);

        int bin = (w == msg.size()) ? ULTRASOUND_BASE_BIN
                                    : charToBin(msg[w]);
        plantCharacter(spectrum, static_cast<size_t>(WINDOW_SIZE), bin,
                       ENCODE_MAGNITUDE, 0.0);

        std::vector<double> reconstructed = idft.transform(spectrum);

        for (int i = 0; i < WINDOW_SIZE; ++i) {
            int32_t v = static_cast<int32_t>(std::round(reconstructed[i]));
            v = std::max(-32768, std::min(32767, v));
            output[start + i] = static_cast<int16_t>(v);
        }
    }
    return output;
}
