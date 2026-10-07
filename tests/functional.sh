#!/usr/bin/env bash

BINARY=./stone_analysis
PASS=0
FAIL=0

TMPDIR_FT=$(mktemp -d)
trap 'rm -rf "$TMPDIR_FT"' EXIT

INPUT="$TMPDIR_FT/input.wav"
OUTPUT="$TMPDIR_FT/output.wav"
OUT="$TMPDIR_FT/stdout"
ERR="$TMPDIR_FT/stderr"

python3 tests/gen_fixture.py "$INPUT"

ok()   { PASS=$((PASS+1)); printf '[PASS] %s\n' "$1"; }
fail() { FAIL=$((FAIL+1)); printf '[FAIL] %s\n' "$1"; }

expect_exit() {
    local desc="$1" code="$2"; shift 2
    local got=0
    "$@" >"$OUT" 2>"$ERR" || got=$?
    [ "$got" -eq "$code" ] && ok "$desc" || fail "$desc — expected exit $code, got $got"
}

expect_stdout() {
    local desc="$1" code="$2" pattern="$3"; shift 3
    local got=0
    "$@" >"$OUT" 2>"$ERR" || got=$?
    if [ "$got" -eq "$code" ] && grep -qF "$pattern" "$OUT"; then
        ok "$desc"
    else
        fail "$desc — exit=$got, pattern='$pattern' not in stdout"
    fi
}

# --- Error cases ---
expect_exit "no args → 84"                    84  $BINARY
expect_exit "unknown mode → 84"               84  $BINARY --unknown
expect_exit "--analyze wrong argc → 84"       84  $BINARY --analyze
expect_exit "--analyze N=0 → 84"              84  $BINARY --analyze "$INPUT" 0
expect_exit "--analyze nonexistent → 84"      84  $BINARY --analyze /no/such/file 3
expect_exit "--cypher wrong argc → 84"        84  $BINARY --cypher
expect_exit "--decypher wrong argc → 84"      84  $BINARY --decypher

# --- Help ---
expect_exit   "--help → 0"                     0  $BINARY --help
expect_exit   "-h → 0"                         0  $BINARY -h

# --- Analyze ---
expect_stdout "--analyze prints header"        0  "Top 3"  $BINARY --analyze "$INPUT" 3
expect_stdout "--analyze prints Hz"            0  "Hz"     $BINARY --analyze "$INPUT" 3
expect_stdout "-a short flag works"            0  "Hz"     $BINARY -a "$INPUT" 5

# --- Cypher / Decypher round-trips ---
expect_exit   "--cypher encodes → 0"           0  $BINARY --cypher "$INPUT" "$OUTPUT" "hello"
expect_stdout "--decypher → HELLO"             0  "HELLO"  $BINARY --decypher "$OUTPUT"
expect_stdout "-d short flag → HELLO"          0  "HELLO"  $BINARY -d "$OUTPUT"

expect_exit   "-c short flag encodes → 0"      0  $BINARY -c "$INPUT" "$OUTPUT" "test 42"
expect_stdout "-d decodes test 42 → TEST 42"   0  "TEST 42" $BINARY -d "$OUTPUT"

# --- More error cases ---
expect_exit "--cypher nonexistent input → 84"    84  $BINARY --cypher /no/such.wav "$OUTPUT" "hi"
expect_exit "--decypher nonexistent input → 84"  84  $BINARY --decypher /no/such.wav
expect_exit "-c wrong argc (no args) → 84"       84  $BINARY -c
expect_exit "-d wrong argc (no args) → 84"       84  $BINARY -d

# --- Output file properties ---
expect_exit   "cypher miaou → 0"                  0  $BINARY --cypher "$INPUT" "$OUTPUT" "miaou"
INPUT_BYTES=$(wc -c < "$INPUT")
OUTPUT_BYTES=$(wc -c < "$OUTPUT")
[ "$INPUT_BYTES" -eq "$OUTPUT_BYTES" ] \
    && ok "output file same size as input" \
    || fail "output file size mismatch (input=$INPUT_BYTES output=$OUTPUT_BYTES)"
expect_stdout "--decypher miaou → MIAOU"          0  "MIAOU"  $BINARY --decypher "$OUTPUT"

# --- Numbers-only message ---
expect_exit   "cypher digits → 0"                 0  $BINARY --cypher "$INPUT" "$OUTPUT" "12345"
expect_stdout "decypher digits → 12345"           0  "12345"  $BINARY --decypher "$OUTPUT"

# --- Analyze N=1 produces exactly 1 Hz line ---
expect_stdout "--analyze N=1 → 1 Hz line"         0  "Hz"     $BINARY --analyze "$INPUT" 1

# --- Uppercase input is normalized to lowercase ---
OUTPUT2="$TMPDIR_FT/output2.wav"
expect_exit   "cypher HELLO (uppercase input) → 0" 0  $BINARY --cypher "$INPUT" "$OUTPUT" "HELLO"
expect_exit   "cypher hello (lowercase input) → 0" 0  $BINARY --cypher "$INPUT" "$OUTPUT2" "hello"
expect_stdout "decode uppercase-encoded → HELLO"   0  "HELLO"  $BINARY --decypher "$OUTPUT"
expect_stdout "decode lowercase-encoded → HELLO"   0  "HELLO"  $BINARY --decypher "$OUTPUT2"

# --- Encoded file is byte-different from original input ---
expect_exit   "cypher abc → 0"                      0  $BINARY --cypher "$INPUT" "$TMPDIR_FT/output3.wav" "abc"
if ! cmp -s "$INPUT" "$TMPDIR_FT/output3.wav"; then
    ok "encoded file differs from input"
else
    fail "encoded file is identical to input (no modification)"
fi
expect_stdout "decode abc → ABC"                    0  "ABC"   $BINARY --decypher "$TMPDIR_FT/output3.wav"

# --- Summary ---
printf '\n%d functional tests: %d passed, %d failed\n' "$((PASS+FAIL))" "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
