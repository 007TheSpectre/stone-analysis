#include "Fft.hpp"
#include <cmath>

static constexpr double TWO_PI = 2.0 * M_PI;

void fftPow2(std::vector<Complex>& a, bool invert) {
    size_t n = a.size();
    if (n <= 1) return;

    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
            Complex tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }

    std::vector<Complex> roots(n / 2);
    double sign = invert ? 1.0 : -1.0;
    for (size_t k = 0; k < n / 2; ++k)
        roots[k] = complexExp(sign * TWO_PI * static_cast<double>(k) / static_cast<double>(n));

    for (size_t len = 2; len <= n; len <<= 1) {
        size_t half = len / 2;
        size_t stride = n / len;
        for (size_t base = 0; base < n; base += len) {
            for (size_t k = 0; k < half; ++k) {
                Complex twiddle = roots[k * stride];
                Complex even = a[base + k];
                Complex odd  = a[base + k + half] * twiddle;
                a[base + k]        = even + odd;
                a[base + k + half] = even - odd;
            }
        }
    }
}

static size_t nextPowerOfTwo(size_t n) {
    size_t power = 1;
    while (power < n)
        power <<= 1;
    return power;
}

static std::vector<Complex> bluestein(const std::vector<Complex>& in, bool invert) {
    size_t N = in.size();
    double sign = invert ? 1.0 : -1.0;
    size_t M = nextPowerOfTwo(2 * N - 1);

    std::vector<Complex> chirp(N);
    std::vector<Complex> a(M, {0.0, 0.0});
    std::vector<Complex> filter(M, {0.0, 0.0});

    for (size_t n = 0; n < N; ++n) {
        unsigned long long squared =
            static_cast<unsigned long long>(n) * n % (2ULL * N);
        double angle = sign * M_PI * static_cast<double>(squared) / static_cast<double>(N);
        chirp[n] = complexExp(angle);
        a[n] = in[n] * chirp[n];
    }

    filter[0] = conjugate(chirp[0]);
    for (size_t n = 1; n < N; ++n) {
        Complex value = conjugate(chirp[n]);
        filter[n] = value;
        filter[M - n] = value;
    }

    fftPow2(a, false);
    fftPow2(filter, false);
    for (size_t i = 0; i < M; ++i)
        a[i] = a[i] * filter[i];
    fftPow2(a, true);

    double invM = 1.0 / static_cast<double>(M);
    std::vector<Complex> out(N);
    for (size_t k = 0; k < N; ++k)
        out[k] = chirp[k] * (invM * a[k]);
    return out;
}

std::vector<Complex> dftAny(const std::vector<Complex>& in, bool invert) {
    size_t N = in.size();
    if (N <= 1)
        return in;
    if ((N & (N - 1)) == 0) {
        std::vector<Complex> a = in;
        fftPow2(a, invert);
        return a;
    }
    return bluestein(in, invert);
}
