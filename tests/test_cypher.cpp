#include <criterion/criterion.h>
#include <memory>
#include <stdexcept>
#include <vector>
#include "steg/CharMap.hpp"
#include "steg/Cypher.hpp"
#include "steg/Decypher.hpp"
#include "steg/ICypher.hpp"
#include "steg/IDecypher.hpp"

static std::vector<int16_t> makeNullSamples(size_t n) {
    return std::vector<int16_t>(n, 0);
}

static size_t requiredSamples(size_t msgLen) {
    return (msgLen + 1) * static_cast<size_t>(WINDOW_SIZE);
}

Test(cypher, too_short_audio_throws) {
    Cypher cypher;
    auto samples = makeNullSamples(WINDOW_SIZE);
    cr_assert_throw(cypher.encode(samples, "hi"), std::runtime_error);
}

Test(cypher, invalid_char_throws) {
    Cypher cypher;
    auto samples = makeNullSamples(requiredSamples(5));
    cr_assert_throw(cypher.encode(samples, "hello!"), std::runtime_error);
}

Test(cypher, empty_message_does_not_throw) {
    Cypher cypher;
    auto samples = makeNullSamples(requiredSamples(0));
    auto encoded = cypher.encode(samples, "");
    cr_assert_eq(encoded.size(), samples.size());
}

Test(cypher, output_size_matches_input) {
    Cypher cypher;
    auto samples = makeNullSamples(requiredSamples(3));
    auto encoded = cypher.encode(samples, "abc");
    cr_assert_eq(encoded.size(), samples.size());
}

Test(cypher, uppercase_normalized_to_same_result) {
    Cypher cypher;
    auto s1 = makeNullSamples(requiredSamples(5));
    auto s2 = makeNullSamples(requiredSamples(5));
    auto enc_upper = cypher.encode(s1, "HELLO");
    auto enc_lower = cypher.encode(s2, "hello");
    cr_assert_eq(enc_upper.size(), enc_lower.size());
    for (size_t i = 0; i < enc_upper.size(); ++i)
        cr_assert_eq(enc_upper[i], enc_lower[i]);
}

Test(decypher, silent_samples_return_empty) {
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(5));
    cr_assert_str_eq(decypher.decode(samples).c_str(), "");
}

Test(decypher, fewer_than_one_window_returns_empty) {
    Decypher decypher;
    auto samples = makeNullSamples(WINDOW_SIZE - 1);
    cr_assert_str_eq(decypher.decode(samples).c_str(), "");
}

Test(cypher_decypher, round_trip_single_char) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(1));
    auto encoded = cypher.encode(samples, "a");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "A");
}

Test(cypher_decypher, round_trip_letters) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(5));
    auto encoded = cypher.encode(samples, "hello");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "HELLO");
}

Test(cypher_decypher, round_trip_digits) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(2));
    auto encoded = cypher.encode(samples, "42");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "42");
}

Test(cypher_decypher, round_trip_space) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(8));
    auto encoded = cypher.encode(samples, "hi there");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "HI THERE");
}

Test(cypher_decypher, round_trip_mixed_alphanum) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(6));
    auto encoded = cypher.encode(samples, "abc123");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "ABC123");
}

Test(cypher_decypher, round_trip_single_digit) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(1));
    auto encoded = cypher.encode(samples, "7");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "7");
}

Test(cypher_decypher, round_trip_single_space) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(1));
    auto encoded = cypher.encode(samples, " ");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), " ");
}

Test(cypher_decypher, round_trip_uppercase_input) {
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(5));
    auto encoded = cypher.encode(samples, "WORLD");
    cr_assert_str_eq(decypher.decode(encoded).c_str(), "WORLD");
}

Test(cypher, high_amplitude_samples_stay_in_int16_range) {
    Cypher cypher;
    size_t n = requiredSamples(3);
    std::vector<int16_t> samples(n, 28000);
    auto encoded = cypher.encode(samples, "abc");
    for (int16_t s : encoded) {
        cr_assert(s >= -32768);
        cr_assert(s <= 32767);
    }
}

Test(cypher, negative_high_amplitude_stays_in_range) {
    Cypher cypher;
    size_t n = requiredSamples(3);
    std::vector<int16_t> samples(n, -28000);
    auto encoded = cypher.encode(samples, "xyz");
    for (int16_t s : encoded) {
        cr_assert(s >= -32768);
        cr_assert(s <= 32767);
    }
}

Test(cypher, encode_at_exact_capacity) {
    Cypher cypher;
    std::string msg = "abc";
    size_t required = requiredSamples(msg.size());
    auto samples = makeNullSamples(required);
    auto encoded = cypher.encode(samples, msg);
    cr_assert_eq(encoded.size(), required);
}

Test(cypher_decypher, round_trip_full_alphabet) {
    Cypher   cypher;
    Decypher decypher;
    std::string msg = "abcdefghijklmnopqrstuvwxyz";
    auto samples = makeNullSamples(requiredSamples(msg.size()));
    auto encoded = cypher.encode(samples, msg);
    std::string decoded = decypher.decode(encoded);
    cr_assert_str_eq(decoded.c_str(), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

Test(cypher_decypher, round_trip_all_digits) {
    Cypher   cypher;
    Decypher decypher;
    std::string msg = "0123456789";
    auto samples = makeNullSamples(requiredSamples(msg.size()));
    auto encoded = cypher.encode(samples, msg);
    std::string decoded = decypher.decode(encoded);
    cr_assert_str_eq(decoded.c_str(), "0123456789");
}

Test(cypher_decypher, round_trip_mixed_message) {
    Cypher   cypher;
    Decypher decypher;
    std::string msg = "hello world 42";
    auto samples = makeNullSamples(requiredSamples(msg.size()));
    auto encoded = cypher.encode(samples, msg);
    std::string decoded = decypher.decode(encoded);
    cr_assert_str_eq(decoded.c_str(), "HELLO WORLD 42");
}

Test(decypher, terminates_at_encoded_end) {
    // Encode 3 chars into a buffer large enough for 10 — decoder must stop at 3
    Cypher   cypher;
    Decypher decypher;
    auto samples = makeNullSamples(requiredSamples(10));
    auto encoded = cypher.encode(samples, "abc");
    std::string got = decypher.decode(encoded);
    cr_assert_eq(got.size(), 3u);
    cr_assert_str_eq(got.c_str(), "ABC");
}

Test(cypher_decypher, round_trip_all_spaces) {
    Cypher   cypher;
    Decypher decypher;
    std::string msg = "   ";
    auto samples = makeNullSamples(requiredSamples(msg.size()));
    auto encoded = cypher.encode(samples, msg);
    std::string decoded = decypher.decode(encoded);
    cr_assert_str_eq(decoded.c_str(), "   ");
}

Test(cypher, virtual_destructor_via_interface_ptr) {
    std::unique_ptr<ICypher> c = std::make_unique<Cypher>();
    (void)c;
}

Test(decypher, virtual_destructor_via_interface_ptr) {
    std::unique_ptr<IDecypher> d = std::make_unique<Decypher>();
    (void)d;
}
