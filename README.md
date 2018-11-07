# Yggdrasil
Yggdrasil is at the moment a scripting language for sound design. You put together small audio processing modules such as oscillators, filters, signal adders, and signal multipliers. Someone will make a GUI at some point, and someone will make it work in realtime. You can read code from a file or enter line by line.
### Syntax
`; comment` <br />
`mk [type] [identifier] [params]` <br />
`ct [identifier][n]>[identifier][n]` <br />
### Example
```
; Simple patch for amplitude modulation
mk osc osc 300      ; make an oscillator, call it 'osc' and set the default frequency to 300hz
mk osc mod 50       ; make another oscillator at 50hz
mk mult mult _      ; make a multiplier object with no default parameter

mk out out out.raw  ; make a file output, call it 'out' and set the output file to the CWD\out.raw
                    ; non relative paths may be used
ct osc0>mult0       ; connect [output 0] on the oscillator to [input 0] on the mult
ct mod0>mult1       ; other oscillator to [input 1]
ct mult0>out0       ; write the output from the mult to the file

done                ; generate the audio
```
In many cases, object parameters are overridden by input signals if there are connections to the corresponding input. `[input 0]` is often the signal to be processed by the object. i.e. `add` will add signals, unless there is no input signal on `[input 1]`, in which case the `[default value]` parameter will be used.
### Object list
IO is zero-indexed but having one input means one input. Therefore In: 0 | Out: 1 means no inputs and one output, which is `[output 0]`. <br /> <br />
~oscillator <br />
`mk osc [name] [frequency]` In: 1 | Out: 1 <br />
Generates a sine wave at `[frequency]` or `[input 0]` hz, oscillating in the range -1, 1 <br />

~square <br />
`mk square [name] [frequency] [pulse width]` In: 2 | Out: 1 <br />
Generates a square wave. `[pulse width]` or `[input 1]` ranges from -1 to 1, where 0 is 50% duty cycle <br />

~fileout <br />
`mk out [name] [file path]` In: 1 | Out: 0 <br />
Writes `[input 0]` to a file at `[file path]`. 32bit, Mono, 44.1khz, Little-endian. <br />

~noise <br />
`mk noise [name]` In: 0 | Out: 1 <br />
Writes white noise to `[output 0]` <br />

~filter <br />
`mk filter [name] [cutoff frequency]` In: 2 | Out: 1 <br />
Lowpass filter, `[input 1]` or `[cutoff frequency]` is the edge frequency <br />

~crush <br />
`mk crush [name] [resolution] [s&h]` In: 3 | Out: 1 <br />
 `[input 0]` is sampled and held for `[s&h]` or `[input 2]` samples. It is only allowed to jump to `[resolution]` or `[input 1]` different amplitudes <br />

~add <br />
`mk add [name] [default value]` In: 2 | Out: 1 <br />
`[output 0]` is the sum of `[input 0]` and either `[input 1]` or `[default value]` <br />

~mult <br />
`mk mult [name] [default value]` In: 2 | Out: 1 <br />
`[output 0]` is the product of `[input 0]` and either `[input 1]` or `[default value]` <br />
### Advanced Example
```
;
; this is an implementation of
;       Karplus-Strong
; physical modelling algorithm
;
; synthesizing some plucked strings
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
mk out out out.raw
mk filter soft 300

ct ddl0>soft0
ct soft0>out0

done
```
