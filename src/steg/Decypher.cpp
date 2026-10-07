#include "Decypher.hpp"
#include "CharMap.hpp"
#include "dft/Dft.hpp"
#include <cctype>

std::string Decypher::decode(const std::vector<int16_t>& samples) {
    std::string result;
    Dft dft;

    size_t totalSamples = samples.size();
    size_t w = 0;

    while ((w + 1) * static_cast<size_t>(WINDOW_SIZE) <= totalSamples) {
        size_t start = w * static_cast<size_t>(WINDOW_SIZE);

        std::vector<double> window(WINDOW_SIZE);
        for (int i = 0; i < WINDOW_SIZE; ++i)
            window[i] = static_cast<double>(samples[start + i]);

        std::vector<Complex> spectrum = dft.transform(window);

        int    peakBin = ULTRASOUND_BASE_BIN;
        double peakMag = 0.0;
        for (int bin = ULTRASOUND_BASE_BIN; bin <= ULTRASOUND_TOP_BIN; ++bin) {
            double mag = magnitude(spectrum[bin]);
            if (mag > peakMag) {
                peakMag = mag;
                peakBin = bin;
            }
        }

        if (peakMag < DETECT_THRESHOLD)
            break;
        if (peakBin == ULTRASOUND_BASE_BIN)
            break;

        result += binToChar(peakBin);
        ++w;
    }

    for (char& c : result)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return result;
}
