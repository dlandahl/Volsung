
```
;
;       Karplus-Strong
; physical modelling synthesis
;

; === Amp Envelope ===
; generate an envelope
; it should be above zero to avoid phase inversion
mk square env 3 -0.9995
mk add shift 1
mk mult div 0.5
mk filter smooth 50

ct env0>shift0
ct shift0>div0
ct div0>smooth0

; === Oscillator ===
; the volume will be controlled by the envelope
mk noise osc
mk mult amp 0
ct osc0>amp0
ct smooth0>amp1

; === Filtered Delay ===
; delay line with feedback loop
mk delay ddl 100.227272727
mk mult fb 0.95
mk add sum 0
mk filter lop 880

ct amp0>sum0
ct sum0>ddl0
ct ddl0>lop0
ct lop0>fb0
ct fb0>sum1

; write the output of the delay to a file
mk file out out.raw
mk filter soft 300

ct ddl0>soft0
ct soft0>out0

done
```
