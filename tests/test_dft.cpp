#include <criterion/criterion.h>
#include <cmath>
#include <memory>
#include <vector>
#include "dft/Complex.hpp"
#include "dft/Dft.hpp"
#include "dft/IDft.hpp"
#include "dft/Idft.hpp"

static constexpr double EPS = 1e-6;

Test(dft, single_sample_passthrough) {
    Dft dft;
    std::vector<double> samples = {42.0};
    auto spectrum = dft.transform(samples);
    cr_assert_eq(spectrum.size(), 1u);
    cr_assert_float_eq(spectrum[0].real, 42.0, EPS);
    cr_assert_float_eq(spectrum[0].imag,  0.0, EPS);
}

Test(dft, all_zeros_gives_zero_spectrum) {
    Dft dft;
    std::vector<double> samples(8, 0.0);
    auto spectrum = dft.transform(samples);
    for (const auto& c : spectrum) {
        cr_assert_float_eq(magnitude(c), 0.0, EPS);
    }
}

Test(dft, dc_signal_energy_at_bin_zero) {
    Dft dft;
    const size_t N = 8;
    std::vector<double> samples(N, 1.0);
    auto spectrum = dft.transform(samples);
    cr_assert_float_eq(magnitude(spectrum[0]), static_cast<double>(N), EPS);
    for (size_t k = 1; k < N; ++k)
        cr_assert_float_eq(magnitude(spectrum[k]), 0.0, EPS);
}

Test(dft, cosine_energy_at_correct_bin) {
    Dft dft;
    const size_t N = 8;
    const size_t K = 2;
    std::vector<double> samples(N);
    for (size_t n = 0; n < N; ++n)
        samples[n] = std::cos(2.0 * M_PI * K * n / static_cast<double>(N));
    auto spectrum = dft.transform(samples);
    double peak = magnitude(spectrum[K]);
    for (size_t k = 1; k < N / 2; ++k) {
        if (k != K)
            cr_assert(magnitude(spectrum[k]) < peak * 0.01);
    }
}

Test(dft, sine_energy_at_correct_bin) {
    Dft dft;
    const size_t N = 8;
    const size_t K = 3;
    std::vector<double> samples(N);
    for (size_t n = 0; n < N; ++n)
        samples[n] = std::sin(2.0 * M_PI * K * n / static_cast<double>(N));
    auto spectrum = dft.transform(samples);
    double peak = magnitude(spectrum[K]);
    for (size_t k = 1; k < N / 2; ++k) {
        if (k != K)
            cr_assert(magnitude(spectrum[k]) < peak * 0.01);
    }
}

Test(dft, output_size_matches_input) {
    Dft dft;
    for (size_t N : {1u, 4u, 8u, 16u}) {
        std::vector<double> s(N, 1.0);
        auto spec = dft.transform(s);
        cr_assert_eq(spec.size(), N);
    }
}

Test(idft, round_trip_silence) {
    Dft  dft;
    Idft idft;
    const size_t N = 8;
    std::vector<double> samples(N, 0.0);
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], 0.0, EPS);
}

Test(idft, round_trip_dc) {
    Dft  dft;
    Idft idft;
    const size_t N = 8;
    std::vector<double> samples(N, 5.0);
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], 5.0, EPS);
}

Test(idft, round_trip_sine) {
    Dft  dft;
    Idft idft;
    const size_t N = 8;
    std::vector<double> samples(N);
    for (size_t n = 0; n < N; ++n)
        samples[n] = std::sin(2.0 * M_PI * n / static_cast<double>(N));
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(idft, round_trip_arbitrary_signal) {
    Dft  dft;
    Idft idft;
    const size_t N = 16;
    std::vector<double> samples(N);
    for (size_t n = 0; n < N; ++n)
        samples[n] = static_cast<double>(n % 5) - 2.0;
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(idft, round_trip_mixed_sinusoids) {
    Dft  dft;
    Idft idft;
    const size_t N = 8;
    std::vector<double> samples(N);
    for (size_t n = 0; n < N; ++n)
        samples[n] = std::sin(2.0 * M_PI * n / N) + 0.5 * std::cos(4.0 * M_PI * n / N);
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft_idft, parseval_energy_preserved) {
    Dft dft;
    const size_t N = 8;
    std::vector<double> samples(N);
    for (size_t n = 0; n < N; ++n)
        samples[n] = std::sin(2.0 * M_PI * n / N) + 0.5;
    auto spectrum = dft.transform(samples);

    double timeDomainEnergy = 0.0;
    for (double s : samples)
        timeDomainEnergy += s * s;

    double freqDomainEnergy = 0.0;
    for (const auto& c : spectrum)
        freqDomainEnergy += magnitude(c) * magnitude(c);
    freqDomainEnergy /= static_cast<double>(N);

    cr_assert_float_eq(freqDomainEnergy, timeDomainEnergy, 1e-4);
}

Test(dft_idft, idft_output_size_matches_input) {
    Dft  dft;
    Idft idft;
    const size_t N = 16;
    std::vector<double> samples(N, 1.0);
    auto reconstructed = idft.transform(dft.transform(samples));
    cr_assert_eq(reconstructed.size(), N);
}

Test(idft, single_sample_passthrough) {
    Idft idft;
    std::vector<Complex> spectrum = {{7.0, 0.0}};
    auto samples = idft.transform(spectrum);
    cr_assert_eq(samples.size(), 1u);
    cr_assert_float_eq(samples[0], 7.0, EPS);
}

Test(dft, dc_signal_size_3) {
    Dft dft;
    std::vector<double> samples(3, 1.0);
    auto spectrum = dft.transform(samples);
    cr_assert_float_eq(magnitude(spectrum[0]), 3.0, EPS);
}

Test(dft, round_trip_size_3) {
    Dft  dft;
    Idft idft;
    std::vector<double> samples = {1.0, -2.0, 3.0};
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < 3; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft, round_trip_size_6) {
    Dft  dft;
    Idft idft;
    std::vector<double> samples = {1.0, 2.0, -1.0, 3.0, 0.5, -2.0};
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < 6; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft, round_trip_size_7) {
    // Prime N — smallestPrimeFactor exits loop without finding a factor, returns n
    Dft  dft;
    Idft idft;
    const size_t N = 7;
    std::vector<double> samples(N);
    for (size_t i = 0; i < N; ++i)
        samples[i] = std::cos(2.0 * M_PI * i / static_cast<double>(N));
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft, round_trip_size_9) {
    // 3² — smallestPrimeFactor: i=2 fails (9%2!=0), i=3 succeeds
    Dft  dft;
    Idft idft;
    const size_t N = 9;
    std::vector<double> samples(N);
    for (size_t i = 0; i < N; ++i)
        samples[i] = static_cast<double>(i) - 4.0;
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft, round_trip_size_15) {
    // 3×5 — composite non-power-of-2 with two different prime factors
    Dft  dft;
    Idft idft;
    const size_t N = 15;
    std::vector<double> samples(N);
    for (size_t i = 0; i < N; ++i)
        samples[i] = static_cast<double>(i % 7) - 3.0;
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft, round_trip_size_25) {
    // 5² — requires multiple loop iterations before finding factor 5
    Dft  dft;
    Idft idft;
    const size_t N = 25;
    std::vector<double> samples(N);
    for (size_t i = 0; i < N; ++i)
        samples[i] = std::sin(2.0 * M_PI * 3.0 * i / static_cast<double>(N));
    auto reconstructed = idft.transform(dft.transform(samples));
    for (size_t i = 0; i < N; ++i)
        cr_assert_float_eq(reconstructed[i], samples[i], EPS);
}

Test(dft, output_size_non_power_of_two) {
    Dft dft;
    for (size_t N : {3u, 6u, 7u, 9u, 15u}) {
        std::vector<double> s(N, 1.0);
        cr_assert_eq(dft.transform(s).size(), N);
    }
}

Test(dft, virtual_destructor_via_interface_ptr) {
    std::unique_ptr<IDft> d = std::make_unique<Dft>();
    (void)d;
}
