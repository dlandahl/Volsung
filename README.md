## Yggdrasil

Yggdrasil is at the moment a scripting language for sound design. It is composed of small audio processing modules such as Oscillators, Filters, Signal adders, Signal multipliers.
Someone will make a GUI at some point.
Read from a file or enter code line by line. Syntax:
```
; This is a comment
mk osc sine 200      ; make an oscillator, call it 'sine' and set the default frequency to 200hz
mk out out out.raw   ; make a file output, call it 'out' and set the output file to the CWD\out.raw
                     ; non relative paths may be used
ct sine0>out0        ; connect output 0 on the oscillator to input 0 on the fileout
done                 ; generate the audio
```

-`add`
-`mult`
-`out`
-`osc`
-`filter`
-`noise`
-`square`
-`crush`

<!--`add` is an object which adds the amplitudes of its two input signals, or alternatively adds a constant value to input `0`
- `mult`
works the same way as `add`
- `crush`
`crush` is a simple distortion object. It takes an input signal, and two integral parameters. One is a sample and hold for `n` samples, the other specifies the vertical resolution, i.e. how many different amplitudes should the signal amplitude be quantized to
- `fileout`
will write its input to a file, each float sample amplitude
- `filter`
a single pole lowpass filter. Can specify edge frequency
- `noise`
generates white noise and outputs it on output `0`
- `osc`
a sine oscillator. Takes as a parameter the frequency in cycles per second, hz. To specify amplitude, combine with `mult`. It is possible to use `add` without any inputs to generate a constant
- `square`
a square oscillator. Able to specify frequency and pulse width in the range 1 to -1 where 0 is a 50% duty cycle