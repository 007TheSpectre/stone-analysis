# stone_analysis {#mainpage}

**Epitech G-CNA-400** — WAV frequency analysis and ultrasonic steganography in C++17.

## What it does

`stone_analysis` reads 16-bit mono PCM WAV files (48 kHz) and supports three modes:

| Flag | Short | Description |
|------|-------|-------------|
| `--analyze`  | `-a` | Print the top N frequency bins by magnitude |
| `--cypher`   | `-c` | Hide a text message in the ultrasound range of a WAV file |
| `--decypher` | `-d` | Extract a hidden message from a WAV file |

```sh
./stone_analysis --analyze  IN_FILE N
./stone_analysis --cypher   IN_FILE OUT_FILE MESSAGE
./stone_analysis --decypher IN_FILE
```

## Build

```sh
make              # build ./stone_analysis
make tests_run    # unit tests + line & branch coverage
make docs         # generate this documentation
make re           # clean rebuild
```

## Module Overview

| Module | Description |
|--------|-------------|
| `wav/` | WAV header parsing, PCM sample read/write (16-bit signed, mono, 48 kHz) |
| `dft/` | `Complex` type, mixed-radix FFT (`Dft`), IFFT (`Idft`) |
| `analyze/` | Compute magnitudes, sort, display top N frequencies |
| `steg/` | Char ↔ ultrasound bin mapping, encode (`Cypher`), decode (`Decypher`) |

## Architecture Diagrams

See the [Architecture page](../architecture.html) for module graph, cypher flow,
decypher flow, steganography design, and CI/CD pipeline diagrams.
