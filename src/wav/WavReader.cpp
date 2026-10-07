#include "WavReader.hpp"
#include <cstring>
#include <fstream>
#include <stdexcept>

static uint16_t readU16LE(const uint8_t* p) {
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
}

static uint32_t readU32LE(const uint8_t* p) {
    return static_cast<uint32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

void WavReader::open(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("cannot open file: " + path);

    file.read(reinterpret_cast<char*>(_header.data()), 44);
    if (file.gcount() != 44)
        throw std::runtime_error("file too short to be a valid WAV: " + path);

    if (std::memcmp(_header.data(), "RIFF", 4) != 0)
        throw std::runtime_error("not a RIFF file: " + path);
    if (std::memcmp(_header.data() + 8, "WAVE", 4) != 0)
        throw std::runtime_error("not a WAVE file: " + path);
    if (std::memcmp(_header.data() + 12, "fmt ", 4) != 0)
        throw std::runtime_error("missing fmt chunk: " + path);

    uint16_t audioFormat  = readU16LE(_header.data() + 20);
    uint16_t numChannels  = readU16LE(_header.data() + 22);
    uint32_t sampleRate   = readU32LE(_header.data() + 24);
    uint16_t bitsPerSample = readU16LE(_header.data() + 34);

    if (audioFormat != 1)
        throw std::runtime_error("not PCM audio: " + path);
    if (numChannels != 1)
        throw std::runtime_error("not mono audio: " + path);
    if (sampleRate != 48000)
        throw std::runtime_error("sample rate must be 48000: " + path);
    if (bitsPerSample != 16)
        throw std::runtime_error("bit depth must be 16: " + path);

    uint32_t dataSize = readU32LE(_header.data() + 40);
    size_t numSamples = dataSize / 2;

    _samples.resize(numSamples);
    file.read(reinterpret_cast<char*>(_samples.data()), dataSize);
    if (static_cast<uint32_t>(file.gcount()) != dataSize)
        throw std::runtime_error("data chunk shorter than expected: " + path);
}

std::array<uint8_t, 44> WavReader::rawHeader() const { return _header; }
std::vector<int16_t>    WavReader::samples()   const { return _samples; }
