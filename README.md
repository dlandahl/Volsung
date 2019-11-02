
```
;
;       Karplus-Strong
; physical modelling synthesis
;

; === Amp Envelope ===
; generate an envelope
; it should be above zero to avoid phase inversion
square env 3 -0.9995
add shift 1
mult div 0.5
filter smooth 50

env0>shift0
shift0>div0
div0>smooth0

== Oscillator ===
he volume will be controlled by the envelope
noise osc
mult amp 0
osc0>amp0
smooth0>amp1

== Filtered Delay ===
elay line with feedback loop
delay ddl 100.227272727
mult fb 0.95
add sum 0
filter lop 880

amp0>sum0
sum0>ddl0
ddl0>lop0
lop0>fb0
fb0>sum1

rite the output of the delay to a file
file out out.raw
filter soft 300

ddl0>soft0
soft0>out0

done
```

