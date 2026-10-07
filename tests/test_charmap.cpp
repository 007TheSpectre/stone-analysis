#include <criterion/criterion.h>
#include <cmath>
#include <stdexcept>
#include <vector>
#include "steg/CharMap.hpp"

static constexpr double EPS = 1e-9;

Test(charmap, space_to_bin) {
    cr_assert_eq(charToBin(' '), ULTRASOUND_BASE_BIN + 1);
}

Test(charmap, digit_zero_to_bin) {
    cr_assert_eq(charToBin('0'), ULTRASOUND_BASE_BIN + 2);
}

Test(charmap, digit_nine_to_bin) {
    cr_assert_eq(charToBin('9'), ULTRASOUND_BASE_BIN + 11);
}

Test(charmap, digit_range_sequential) {
    for (int i = 0; i <= 9; ++i)
        cr_assert_eq(charToBin(static_cast<char>('0' + i)), ULTRASOUND_BASE_BIN + 2 + i);
}

Test(charmap, letter_a_to_bin) {
    cr_assert_eq(charToBin('a'), ULTRASOUND_BASE_BIN + 12);
}

Test(charmap, letter_z_to_bin) {
    cr_assert_eq(charToBin('z'), ULTRASOUND_BASE_BIN + 37);
}

Test(charmap, letter_range_sequential) {
    for (int i = 0; i < 26; ++i)
        cr_assert_eq(charToBin(static_cast<char>('a' + i)), ULTRASOUND_BASE_BIN + 12 + i);
}

Test(charmap, invalid_punctuation_returns_neg1) {
    cr_assert_eq(charToBin('!'), -1);
    cr_assert_eq(charToBin('@'), -1);
    cr_assert_eq(charToBin('#'), -1);
}

Test(charmap, invalid_uppercase_returns_neg1) {
    cr_assert_eq(charToBin('A'), -1);
    cr_assert_eq(charToBin('Z'), -1);
}

Test(charmap, bin_to_char_space) {
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 1), ' ');
}

Test(charmap, bin_to_char_digits) {
    for (int i = 0; i <= 9; ++i)
        cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 2 + i), static_cast<char>('0' + i));
}

Test(charmap, bin_to_char_letters) {
    for (int i = 0; i < 26; ++i)
        cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 12 + i), static_cast<char>('a' + i));
}

Test(charmap, bin_to_char_base_bin_returns_null) {
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN), '\0');
}

Test(charmap, bin_to_char_out_of_range_returns_null) {
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 38), '\0');
    cr_assert_eq(binToChar(0), '\0');
    cr_assert_eq(binToChar(-1), '\0');
}

Test(charmap, round_trip_all_valid_chars) {
    const char* valid = " 0123456789abcdefghijklmnopqrstuvwxyz";
    for (const char* p = valid; *p; ++p)
        cr_assert_eq(binToChar(charToBin(*p)), *p);
}

Test(charmap, normalize_already_lowercase) {
    cr_assert_str_eq(normalizeMessage("hello").c_str(), "hello");
}

Test(charmap, normalize_uppercase_to_lower) {
    cr_assert_str_eq(normalizeMessage("HELLO").c_str(), "hello");
}

Test(charmap, normalize_mixed_case) {
    cr_assert_str_eq(normalizeMessage("Hello World").c_str(), "hello world");
}

Test(charmap, normalize_digits_preserved) {
    cr_assert_str_eq(normalizeMessage("abc 123").c_str(), "abc 123");
}

Test(charmap, normalize_empty_string) {
    cr_assert_str_eq(normalizeMessage("").c_str(), "");
}

Test(charmap, normalize_invalid_throws) {
    cr_assert_throw(normalizeMessage("hello!"), std::runtime_error);
}

Test(charmap, normalize_newline_throws) {
    cr_assert_throw(normalizeMessage("hi\nthere"), std::runtime_error);
}

Test(charmap, plant_character_sets_magnitude) {
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int bin = ULTRASOUND_BASE_BIN + 5;
    double before = magnitude(spectrum[bin]);
    plantCharacter(spectrum, N, bin, ENCODE_MAGNITUDE, 0.0);
    cr_assert(magnitude(spectrum[bin]) > before);
}

Test(charmap, plant_character_magnitude_equals_encode_magnitude) {
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int bin = ULTRASOUND_BASE_BIN + 10;
    plantCharacter(spectrum, N, bin, ENCODE_MAGNITUDE, 0.0);
    cr_assert_float_eq(magnitude(spectrum[bin]), ENCODE_MAGNITUDE, 1.0);
}

Test(charmap, plant_character_conjugate_symmetry) {
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int bin = ULTRASOUND_BASE_BIN + 5;
    plantCharacter(spectrum, N, bin, ENCODE_MAGNITUDE, 0.0);
    Complex c    = spectrum[bin];
    Complex conj = spectrum[N - static_cast<size_t>(bin)];
    cr_assert_float_eq(conj.real,  c.real, EPS);
    cr_assert_float_eq(conj.imag, -c.imag, EPS);
}

Test(charmap, plant_character_leaves_other_bins_unchanged) {
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int bin = ULTRASOUND_BASE_BIN + 5;
    plantCharacter(spectrum, N, bin, ENCODE_MAGNITUDE, 0.0);
    for (size_t k = 0; k < N; ++k) {
        if (k != static_cast<size_t>(bin) && k != N - static_cast<size_t>(bin))
            cr_assert_float_eq(magnitude(spectrum[k]), 0.0, EPS);
    }
}

Test(charmap, plant_character_nonzero_phase) {
    // phi=π/4 → both real and imaginary components are non-zero
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int bin = ULTRASOUND_BASE_BIN + 5;
    plantCharacter(spectrum, N, bin, ENCODE_MAGNITUDE, M_PI / 4.0);
    cr_assert(std::abs(spectrum[bin].real) > 1.0);
    cr_assert(std::abs(spectrum[bin].imag) > 1.0);
    cr_assert_float_eq(magnitude(spectrum[bin]), ENCODE_MAGNITUDE, 1.0);
}

Test(charmap, plant_character_clears_existing_band_content) {
    // Carrier energy elsewhere in the ultrasound band must not survive a plant,
    // otherwise it could outvote the encoded symbol at decode time.
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int carrierBin = ULTRASOUND_BASE_BIN + 20;
    int symbolBin  = ULTRASOUND_BASE_BIN + 3;
    spectrum[carrierBin] = {50000.0, 0.0};
    spectrum[N - static_cast<size_t>(carrierBin)] = {50000.0, 0.0};
    plantCharacter(spectrum, N, symbolBin, ENCODE_MAGNITUDE, 0.0);
    cr_assert_float_eq(magnitude(spectrum[carrierBin]), 0.0, EPS);
    cr_assert_float_eq(magnitude(spectrum[symbolBin]), ENCODE_MAGNITUDE, 1.0);
}

Test(charmap, plant_character_conjugate_symmetry_nonzero_phase) {
    const size_t N = 2400;
    std::vector<Complex> spectrum(N, {0.0, 0.0});
    int bin = ULTRASOUND_BASE_BIN + 8;
    plantCharacter(spectrum, N, bin, ENCODE_MAGNITUDE, M_PI / 3.0);
    Complex c  = spectrum[bin];
    Complex cs = spectrum[N - static_cast<size_t>(bin)];
    cr_assert_float_eq(cs.real,  c.real, EPS);
    cr_assert_float_eq(cs.imag, -c.imag, EPS);
}

Test(charmap, bin_to_char_boundary_values) {
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 2),  '0');
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 11), '9');
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 12), 'a');
    cr_assert_eq(binToChar(ULTRASOUND_BASE_BIN + 37), 'z');
}
