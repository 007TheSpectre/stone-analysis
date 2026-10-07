#include "WavWriter.hpp"
#include <fstream>
#include <stdexcept>

void WavWriter::write(const std::string& path,
                      const std::array<uint8_t, 44>& header,
                      const std::vector<int16_t>& samples) {
    std::ofstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("cannot create file: " + path);
    file.write(reinterpret_cast<const char*>(header.data()), 44);
    file.write(reinterpret_cast<const char*>(samples.data()),
               static_cast<std::streamsize>(samples.size() * 2));
}
