## Yggdrasil

Yggdrasil is at the moment a scripting language for sound design. It is composed of small audio processing modules such as Oscillators, Filters, Signal adders, Signal multipliers. Here is a complete list of the audio objects available and what they do

- add
`add` is an object which adds the amplitudes of its two input signals, or alternatively adds a constant value to input `0`
- mult
works the same way as `add`
- crush
`crush` is a simple distortion object. It takes an input signal, and two integral parameters. One is a sample and hold for `n` samples, the other specifies the vertical resolution, i.e. how many different amplitudes should the signal amplitude be quantized to
- fileout
will write its input to a file, each float sample amplitude
- filter
a single pole lowpass filter. Can specify edge frequency
- noise
generates white noise and outputs it on output `0`
- osc
a sine oscillator. Takes as a parameter the frequency in cycles per second, hz. To specify amplitude, combine with `mult`. It is possible to use `add` without any inputs to generate a constant
- square
a square oscillator. Able to specify frequency and pulse width in the range 1 to -1 where 0 is a 50% duty cycle