#include <criterion/criterion.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "analyze/Analyzer.hpp"
#include "analyze/IAnalyzer.hpp"

static std::vector<int16_t> makeSine(size_t N, double freq, uint32_t sr) {
    std::vector<int16_t> s(N);
    for (size_t n = 0; n < N; ++n)
        s[n] = static_cast<int16_t>(16000.0 * std::sin(2.0 * M_PI * freq * n / sr));
    return s;
}

Test(analyzer, single_sine_does_not_throw) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(2400, 440.0, 48000), 3, 48000);
    std::cout.rdbuf(old);
}

Test(analyzer, top_one_does_not_throw) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(2400, 1000.0, 48000), 1, 48000);
    std::cout.rdbuf(old);
}

Test(analyzer, top_n_larger_than_bins_does_not_throw) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(8, 440.0, 48000), 100, 48000);
    std::cout.rdbuf(old);
}

Test(analyzer, output_contains_frequency_line) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(2400, 440.0, 48000), 1, 48000);
    std::cout.rdbuf(old);
    cr_assert(sink.str().find("Hz") != std::string::npos);
}

Test(analyzer, silence_does_not_throw) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    std::vector<int16_t> samples(2400, 0);
    Analyzer{}.analyze(samples, 5, 48000);
    std::cout.rdbuf(old);
}

Test(analyzer, top_n_zero_produces_no_hz_lines) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(8, 440.0, 48000), 0, 48000);
    std::cout.rdbuf(old);
    cr_assert(sink.str().find("Hz") == std::string::npos);
}

Test(analyzer, two_different_sine_waves) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    const size_t N = 48000;
    std::vector<int16_t> s(N);
    for (size_t n = 0; n < N; ++n)
        s[n] = static_cast<int16_t>(
            8000.0 * std::sin(2.0 * M_PI * 440.0 * n / 48000.0) +
            8000.0 * std::sin(2.0 * M_PI * 880.0 * n / 48000.0));
    Analyzer{}.analyze(s, 2, 48000);
    std::cout.rdbuf(old);
    cr_assert(sink.str().find("Hz") != std::string::npos);
}

Test(analyzer, top_1_exact_frequency) {
    // N=2400 at 48kHz → 20 Hz/bin; 440 Hz → exact bin 22 → "440.0 Hz"
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(2400, 440.0, 48000), 1, 48000);
    std::cout.rdbuf(old);
    cr_assert(sink.str().find("440.0 Hz") != std::string::npos);
}

Test(analyzer, single_sample_no_hz_lines) {
    // N=1 → N/2=0 → no frequency bins → output has header but no Hz lines
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(std::vector<int16_t>{1000}, 3, 48000);
    std::cout.rdbuf(old);
    std::string out = sink.str();
    cr_assert(out.find("Top 3") != std::string::npos);
    cr_assert(out.find("Hz") == std::string::npos);
}

Test(analyzer, output_has_top_n_header) {
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(2400, 440.0, 48000), 5, 48000);
    std::cout.rdbuf(old);
    cr_assert(sink.str().find("Top 5 frequencies:") != std::string::npos);
}

Test(analyzer, exact_hz_line_count) {
    // topN=3 on a 2400-sample signal → exactly 3 "Hz" occurrences in output
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    Analyzer{}.analyze(makeSine(2400, 440.0, 48000), 3, 48000);
    std::cout.rdbuf(old);
    std::string s = sink.str();
    int count = 0;
    size_t pos = 0;
    while ((pos = s.find("Hz", pos)) != std::string::npos) { ++count; ++pos; }
    cr_assert_eq(count, 3);
}

Test(analyzer, virtual_destructor_via_interface_ptr) {
    std::unique_ptr<IAnalyzer> a = std::make_unique<Analyzer>();
    (void)a;
}

Test(analyzer, dominant_frequency_appears_first) {
    // 880 Hz at 2× the amplitude of 440 Hz → 880.0 Hz must come before 440.0 Hz
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    const size_t N = 2400;
    std::vector<int16_t> s(N);
    for (size_t n = 0; n < N; ++n)
        s[n] = static_cast<int16_t>(
            4000.0 * std::sin(2.0 * M_PI * 440.0 * n / 48000.0) +
            8000.0 * std::sin(2.0 * M_PI * 880.0 * n / 48000.0));
    Analyzer{}.analyze(s, 2, 48000);
    std::cout.rdbuf(old);
    std::string output = sink.str();
    size_t pos880 = output.find("880.0 Hz");
    size_t pos440 = output.find("440.0 Hz");
    cr_assert(pos880 != std::string::npos);
    cr_assert(pos440 != std::string::npos);
    cr_assert(pos880 < pos440);
}
