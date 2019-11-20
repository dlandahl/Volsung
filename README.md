
### The language that does bleep bloop

Syntax:
```
; Generates a 400hz sine wave
my_source: osc~ 400
my_output: file~ "~/Documents/output.raw"
my_source -> my_output
```

Sequences, index sequences with sequences, step through sequences.
Also an example for building abstractions for dealing with the western musical system.
The language is useful for generative music / composition / DSP / audio effects / sound design / synthesis.
```
; Generate the frequencies of the pentatonic scale
scale: { 0, 2, 4, 7, 9 }
root: 440
freqs: (2^(1/12))^scale * root


eg: eg~
vca: mult~
clock: clock~ sf / 4

; Trigger envelope into VCA
clock
-> 0|eg
-> filter~ 40
-> 1|vca


; Randomise decay time
snh: snh~
clock -> 1|snh
noise~
-> *0.5 -> +0.5
-> * sf/2 + sf/10
-> snh -> 1|eg


; Generate sine signal and output
index: snh~
clock -> 1|index

noise~ -> index
-> *0.5 -> +0.5
-> *4.9
-> seq~ freqs
-> osc~
-> vca
-> file~ "Random.raw"


```
