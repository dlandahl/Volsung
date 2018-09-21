# Yggdrasil
Yggdrasil is at the moment a scripting language for sound design. You put together small audio processing modules such as oscillators, filters, signal adders, signal multipliers. Someone will make a GUI at some point. Read from a file or enter code line by line.
### Syntax
`; comment` <br />
`mk [type] [identifier] [params]` <br />
`ct [identifier][output_index]>[identifier][input_index]` <br />
### Example
```
; This patch will generate a sine wave at 200hz
mk osc sine 200      ; make an oscillator, call it 'sine' and set the default frequency to 200hz
mk out out out.raw   ; make a file output, call it 'out' and set the output file to the CWD\out.raw
                     ; non relative paths may be used
ct sine0>out0        ; connect [output 0] on the oscillator to [input 0] on the fileout
done                 ; generate the audio
```
In many cases, object parameters are overridden by input signals if there are connections to the corresponding input. `[input 0]` is often the signal to be processed by the object. i.e. `add` will add signals, unless there is no input signal on `[input 1]`, in which case the `[default value]` parameter will be used.
### Object list
IO is zero-indexed but having one input means one input. Therefore In: 0 | Out: 1 means no inputs and one output, which is `[output 0]`. <br /> <br />
~oscillator <br />
`mk osc [name] [frequency]`						In: 1 | Out: 1 <br />
Generates a sine wave at `[frequency]` or `[input 0]` hz <br />

~square <br />
`mk square [name] [frequency] [pulse width]`	In: 2 | Out: 1 <br />
Generates a square wave. `[pulse width]` or `[input 1]` ranges from -1 to 1, where 0 is 50% duty cycle <br />

~fileout <br />
`mk out [name] [file path]`						In: 1 | Out: 0 <br />
Writes `[input 0]` to a file at `[file path]` <br />

~noise <br />
`mk noise [name]`								In: 0 | Out: 1 <br />
Writes white noise to `[output 0]` <br />

~filter <br />
`mk filter [name] [cutoff frequency]`			In: 2 | Out: 1 <br />
Lowpass filter, `[input 1]` or `[cutoff frequency]` is the edge frequency <br />

~crush <br />
`mk crush [name] [resolution] [s&h]`			In: 3 | Out: 1 <br />
 `[input 0]` is sampled and held for `[s&h]` or `[input 2]` samples. It is only allowed to jump to `[resolution]` or `[input 1]` different amplitudes <br />

~add <br />
`mk add [name] [default value]`					In: 2 | Out: 1 <br />
`[output 0]` is the sum of `[input 0]` and either `[input 1]` or `[default value]` <br />

~mult <br />
`mk mult [name] [default value]`				In: 2 | Out: 1 <br />
`[output 0]` is the product of `[input 0]` and either `[input 1]` or `[default value]` <br />
