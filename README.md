> **Epitech project — `G-CNA-400` (`stoneanalysis`)**
>
> Built with .
> Solo project - I wrote everything here.
>
> This is my own copy of the assignment repository, published here as a
> portfolio piece. The original repository is private.

---

# stone_analysis

**Documentation:** [https://epitechpge2-2025.github.io/G-CNA-400-LIL-4-1-stoneanalysis-7/](https://epitechpge2-2025.github.io/G-CNA-400-LIL-4-1-stoneanalysis-7/)

A CLI tool that reads 16-bit mono WAV PCM files (48 kHz) and performs three operations:

- **Analyze** — finds the top N frequencies by magnitude using DFT
- **Cypher** — hides a text message inside a WAV file via ultrasound steganography
- **Decypher** — extracts a hidden message from a WAV file

All signal processing (DFT, IDFT, steganography) is implemented from scratch. No audio, FFT, or steganography libraries are used.

---

## Build

```sh
make        # build ./stone_analysis
make re     # fclean + all
make clean  # remove object files
make fclean # remove objects + binary
```

Requirements: C++17 compiler (`c++`), GNU Make.

---

## Usage

```sh
./stone_analysis [--analyze | -a]   IN_FILE N
./stone_analysis [--cypher  | -c]   IN_FILE OUT_FILE MESSAGE
./stone_analysis [--decypher | -d]  IN_FILE
```

### --analyze

Prints the top N frequencies (by magnitude) in decreasing order.

```sh
$ ./stone_analysis --analyze input.wav 3
Top 3 frequencies:
494.0 Hz
330.0 Hz
415.0 Hz
```

### --cypher

Hides a message inside the audio file. The output file has the same size and identical header as the input.

```sh
$ ./stone_analysis --cypher input.wav output.wav "Miaou"
```

Supported characters: `a–z`, `A–Z` (stored case-insensitively), `0–9`, and `SPACE`.

### --decypher

Extracts the hidden message and prints it in uppercase.

```sh
$ ./stone_analysis --decypher output.wav
MIAOU
```

---

## How it works

### Frequency analysis

The full sample buffer is passed through an O(n²) Discrete Fourier Transform. The magnitude of each frequency bin up to the Nyquist limit (24 kHz) is computed, sorted, and the top N are printed.

```
X(k) = Σ_{n=0}^{N-1} x(n) · e^{-2iπkn/N}
```

Frequency resolution: `freq_hz = k · (sample_rate / N)`

### Steganography

The audio is split into 50 ms windows (2400 samples at 48 kHz). Each window encodes one character by injecting a sinusoid in the ultrasound range (> 20 kHz), which is inaudible to humans.

| Parameter | Value |
|-----------|-------|
| Window size | 2400 samples (50 ms) |
| Base bin | 1000 (= 20 000 Hz) |
| Bins used | 1000–1037 |
| Encoding magnitude | 10 000 |
| Detection threshold | 5 000 |

Character → bin mapping:
- Sentinel (end-of-message): bin 1000
- `' '` (space): bin 1001
- `'0'`–`'9'`: bins 1002–1011
- `'a'`–`'z'`: bins 1012–1037

Adding a sinusoid at bin `k` in the frequency domain:

```
X[k]     += M · e^(iφ)
X[N − k]  = conj(X[k])     // conjugate symmetry for real signal
```

After modification, IDFT reconstructs the time-domain samples, which are clamped to `[-32768, 32767]` before writing.

---

## Architecture

```
src/
├── main.cpp              CLI: argument parsing and mode dispatch
├── wav/
│   ├── WavReader         Parse and validate WAV header; read samples
│   └── WavWriter         Write verbatim header + samples
├── dft/
│   ├── Complex.hpp       Header-only complex number type
│   ├── Dft               O(n²) forward DFT
│   └── Idft              O(n²) inverse DFT
├── analyze/
│   └── Analyzer          Magnitude sort and top-N display
└── steg/
    ├── CharMap           char ↔ frequency bin mapping; addSinusoid
    ├── Cypher            Encode message into WAV samples
    └── Decypher          Decode message from WAV samples
```

---

## Constraints

- WAV format: 16-bit signed PCM, mono, 48 kHz only.
- Output WAV header is byte-for-byte identical to the input header.
- The output file is the same size as the input file; the message must fit within the existing audio.
- Exit code `0` on success, `84` on any error. All error messages go to stderr.
