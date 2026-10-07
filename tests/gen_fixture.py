#!/usr/bin/env python3
import sys
import struct
import math

NUM_SAMPLES = 48000
SAMPLE_RATE = 48000

samples = [int(10000 * math.sin(2 * math.pi * 440 * i / SAMPLE_RATE)) for i in range(NUM_SAMPLES)]
data = struct.pack(f'<{NUM_SAMPLES}h', *samples)
data_size = len(data)

header = struct.pack('<4sI4s4sIHHIIHH4sI',
    b'RIFF', 36 + data_size,
    b'WAVE',
    b'fmt ', 16,
    1, 1,
    SAMPLE_RATE, SAMPLE_RATE * 2,
    2, 16,
    b'data', data_size,
)

with open(sys.argv[1], 'wb') as f:
    f.write(header)
    f.write(data)
