#include <criterion/criterion.h>
#include <array>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>
#include "wav/IWavReader.hpp"
#include "wav/IWavWriter.hpp"
#include "wav/WavReader.hpp"
#include "wav/WavWriter.hpp"

static void writeU16LE(uint8_t* p, uint16_t v) {
    p[0] = v & 0xFF;
    p[1] = (v >> 8) & 0xFF;
}

static void writeU32LE(uint8_t* p, uint32_t v) {
    p[0] = v & 0xFF;
    p[1] = (v >> 8) & 0xFF;
    p[2] = (v >> 16) & 0xFF;
    p[3] = (v >> 24) & 0xFF;
}

static std::array<uint8_t, 44> buildHeader(uint32_t numSamples,
                                            uint16_t channels     = 1,
                                            uint32_t sampleRate   = 48000,
                                            uint16_t bitsPerSample = 16) {
    std::array<uint8_t, 44> h{};
    uint32_t dataSize = numSamples * channels * (bitsPerSample / 8);
    std::memcpy(h.data(),      "RIFF", 4);
    writeU32LE(h.data() +  4,  36 + dataSize);
    std::memcpy(h.data() +  8, "WAVE", 4);
    std::memcpy(h.data() + 12, "fmt ", 4);
    writeU32LE(h.data() + 16,  16);
    writeU16LE(h.data() + 20,  1);
    writeU16LE(h.data() + 22,  channels);
    writeU32LE(h.data() + 24,  sampleRate);
    writeU32LE(h.data() + 28,  sampleRate * channels * bitsPerSample / 8);
    writeU16LE(h.data() + 32,  static_cast<uint16_t>(channels * bitsPerSample / 8));
    writeU16LE(h.data() + 34,  bitsPerSample);
    std::memcpy(h.data() + 36, "data", 4);
    writeU32LE(h.data() + 40,  dataSize);
    return h;
}

static void writeTempWav(const char* path,
                          const std::array<uint8_t, 44>& header,
                          const std::vector<int16_t>& samples) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(header.data()), 44);
    f.write(reinterpret_cast<const char*>(samples.data()),
            static_cast<std::streamsize>(samples.size() * 2));
}

Test(wav_reader, nonexistent_file_throws) {
    WavReader reader;
    cr_assert_throw(reader.open("/no/such/file.wav"), std::runtime_error);
}

Test(wav_reader, bad_riff_magic_throws) {
    const char* path = "/tmp/sa_test_bad_riff.wav";
    auto header = buildHeader(10);
    std::memcpy(header.data(), "XXXX", 4);
    writeTempWav(path, header, std::vector<int16_t>(10, 0));
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, bad_wave_magic_throws) {
    const char* path = "/tmp/sa_test_bad_wave.wav";
    auto header = buildHeader(10);
    std::memcpy(header.data() + 8, "XXXX", 4);
    writeTempWav(path, header, std::vector<int16_t>(10, 0));
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, wrong_sample_rate_throws) {
    const char* path = "/tmp/sa_test_bad_rate.wav";
    writeTempWav(path, buildHeader(10, 1, 44100, 16), std::vector<int16_t>(10, 0));
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, stereo_throws) {
    const char* path = "/tmp/sa_test_stereo.wav";
    writeTempWav(path, buildHeader(10, 2, 48000, 16), std::vector<int16_t>(20, 0));
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, eight_bit_throws) {
    const char* path = "/tmp/sa_test_8bit.wav";
    auto header = buildHeader(10, 1, 48000, 8);
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(header.data()), 44);
    std::vector<uint8_t> data(10, 0);
    f.write(reinterpret_cast<const char*>(data.data()), 10);
    f.close();
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, reads_valid_wav_sample_count) {
    const char* path = "/tmp/sa_test_valid.wav";
    std::vector<int16_t> samples = {100, 200, -100, -200, 0};
    writeTempWav(path, buildHeader(5), samples);
    WavReader reader;
    reader.open(path);
    cr_assert_eq(reader.samples().size(), 5u);
    std::remove(path);
}

Test(wav_reader, reads_valid_wav_sample_values) {
    const char* path = "/tmp/sa_test_vals.wav";
    std::vector<int16_t> samples = {1000, -2000, 32767, -32768, 0};
    writeTempWav(path, buildHeader(5), samples);
    WavReader reader;
    reader.open(path);
    auto got = reader.samples();
    for (size_t i = 0; i < samples.size(); ++i)
        cr_assert_eq(got[i], samples[i]);
    std::remove(path);
}

Test(wav_reader, raw_header_matches_written) {
    const char* path = "/tmp/sa_test_hdr.wav";
    std::vector<int16_t> samples(10, 0);
    auto header = buildHeader(10);
    writeTempWav(path, header, samples);
    WavReader reader;
    reader.open(path);
    auto got = reader.rawHeader();
    cr_assert_eq(std::memcmp(got.data(), header.data(), 44), 0);
    std::remove(path);
}

Test(wav_writer, round_trip_sample_values) {
    const char* path = "/tmp/sa_test_writer_rt.wav";
    std::vector<int16_t> samples = {1000, 2000, -3000, 0, 500};
    WavWriter writer;
    writer.write(path, buildHeader(5), samples);
    WavReader reader;
    reader.open(path);
    auto got = reader.samples();
    cr_assert_eq(got.size(), samples.size());
    for (size_t i = 0; i < samples.size(); ++i)
        cr_assert_eq(got[i], samples[i]);
    std::remove(path);
}

Test(wav_writer, header_bytes_preserved) {
    const char* path = "/tmp/sa_test_writer_hdr.wav";
    std::vector<int16_t> samples(10, 0);
    auto header = buildHeader(10);
    WavWriter writer;
    writer.write(path, header, samples);
    WavReader reader;
    reader.open(path);
    auto got = reader.rawHeader();
    cr_assert_eq(std::memcmp(got.data(), header.data(), 44), 0);
    std::remove(path);
}

Test(wav_writer, write_to_bad_path_throws) {
    WavWriter writer;
    auto header = buildHeader(0);
    cr_assert_throw(
        writer.write("/no/such/dir/out.wav", header, {}),
        std::runtime_error
    );
}

Test(wav_reader, header_too_short_throws) {
    const char* path = "/tmp/sa_test_short_hdr.wav";
    uint8_t partial[20] = {'R','I','F','F',20,0,0,0,'W','A','V','E',
                           'f','m','t',' ',16,0,0,0};
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(partial), 20);
    f.close();
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, missing_fmt_chunk_throws) {
    const char* path = "/tmp/sa_test_nofmt.wav";
    auto header = buildHeader(10);
    std::memcpy(header.data() + 12, "    ", 4);
    writeTempWav(path, header, std::vector<int16_t>(10, 0));
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, non_pcm_format_throws) {
    const char* path = "/tmp/sa_test_nonpcm.wav";
    auto header = buildHeader(10);
    writeU16LE(header.data() + 20, 3);
    writeTempWav(path, header, std::vector<int16_t>(10, 0));
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, truncated_data_chunk_throws) {
    const char* path = "/tmp/sa_test_trunc.wav";
    auto header = buildHeader(100);
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(header.data()), 44);
    std::vector<int16_t> partial(10, 0);
    f.write(reinterpret_cast<const char*>(partial.data()),
            static_cast<std::streamsize>(partial.size() * 2));
    f.close();
    WavReader reader;
    cr_assert_throw(reader.open(path), std::runtime_error);
    std::remove(path);
}

Test(wav_reader, zero_sample_wav) {
    const char* path = "/tmp/sa_test_zero_samples.wav";
    writeTempWav(path, buildHeader(0), {});
    WavReader reader;
    reader.open(path);
    cr_assert_eq(reader.samples().size(), 0u);
    std::remove(path);
}

Test(wav_writer, preserves_int16_extremes) {
    const char* path = "/tmp/sa_test_minmax.wav";
    std::vector<int16_t> samples = {32767, -32768, 0, 1, -1};
    WavWriter writer;
    writer.write(path, buildHeader(5), samples);
    WavReader reader;
    reader.open(path);
    auto got = reader.samples();
    cr_assert_eq(got.size(), 5u);
    cr_assert_eq(got[0], 32767);
    cr_assert_eq(got[1], static_cast<int16_t>(-32768));
    cr_assert_eq(got[2], static_cast<int16_t>(0));
    std::remove(path);
}

Test(wav_reader, virtual_destructor_via_interface_ptr) {
    std::unique_ptr<IWavReader> r = std::make_unique<WavReader>();
    (void)r;
}

Test(wav_writer, virtual_destructor_via_interface_ptr) {
    std::unique_ptr<IWavWriter> w = std::make_unique<WavWriter>();
    (void)w;
}

Test(wav_reader, large_sample_count_round_trip) {
    const char* path = "/tmp/sa_test_large.wav";
    const size_t N = 9600;
    std::vector<int16_t> samples(N, 1000);
    writeTempWav(path, buildHeader(N), samples);
    WavReader reader;
    reader.open(path);
    cr_assert_eq(reader.samples().size(), N);
    cr_assert_eq(reader.samples()[0], 1000);
    cr_assert_eq(reader.samples()[N - 1], 1000);
    std::remove(path);
}
