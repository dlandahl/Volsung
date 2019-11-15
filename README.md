
### The language that does bleep bloop

Syntax:
```
; Generates a 400hz sine wave
my_source: osc~ 400
my_output: file~ "~/Documents/output.raw"
my_source(0) -> my_output(0)
```

Sequences, index sequences with sequences, step through sequences.
Also an example for building abstractions for dealing with the western musical system.
The language is useful for generative music / composition / DSP / audio effects / sound design / synthesis.
```
; Semitone offsets from the root note describe a scale
scale: { 0, 2, 3, 5, 7, 8, 10, 12 }
root: 440 ; Root frequency of the scale

; Use the formula for just intonation to generate frequency values
freqs: (2^(1/12))^scale * root

; Output and a clock generator with 5 pulses per second
clock: clock~ sf / 5
disk: file~ "melody.raw"

melody: { 1, 4, 3, 3, 7, 4, 2 } ; Melody from scale degrees

clock(0)
-> step~ freqs[melody - 1]
-> filter~ 10 ; Filter creates portamento
-> osc~
-> *0.6 -> disk(0)
```
