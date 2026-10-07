#include "CharMap.hpp"
#include <cctype>
#include <stdexcept>

int charToBin(char c) {
    if (c == ' ')  return ULTRASOUND_BASE_BIN + 1;
    if (c >= '0' && c <= '9') return ULTRASOUND_BASE_BIN + 2 + (c - '0');
    if (c >= 'a' && c <= 'z') return ULTRASOUND_BASE_BIN + 12 + (c - 'a');
    return -1;
}

char binToChar(int bin) {
    int idx = bin - ULTRASOUND_BASE_BIN;
    if (idx == 1) return ' ';
    if (idx >= 2  && idx <= 11) return static_cast<char>('0' + (idx - 2));
    if (idx >= 12 && idx <= 37) return static_cast<char>('a' + (idx - 12));
    return '\0';
}

std::string normalizeMessage(const std::string& msg) {
    std::string result;
    result.reserve(msg.size());
    for (char c : msg) {
        char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (lower != ' ' && (lower < '0' || lower > '9') &&
            (lower < 'a' || lower > 'z'))
            throw std::runtime_error(std::string("invalid character in message: '") + c + "'");
        result += lower;
    }
    return result;
}

void plantCharacter(std::vector<Complex>& spectrum, size_t N, int bin,
                    double M, double phi) {
    for (int b = ULTRASOUND_BASE_BIN; b <= ULTRASOUND_TOP_BIN; ++b) {
        spectrum[static_cast<size_t>(b)] = {0.0, 0.0};
        spectrum[N - static_cast<size_t>(b)] = {0.0, 0.0};
    }
    spectrum[bin] = M * complexExp(phi);
    spectrum[N - static_cast<size_t>(bin)] = conjugate(spectrum[bin]);
}
