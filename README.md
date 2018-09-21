### Yggdrasil
Yggdrasil is at the moment a scripting language for sound design. You put together small audio processing modules such as oscillators, filters, signal adders, signal multipliers. Someone will make a GUI at some point. Read from a file or enter code line by line.
##### Syntax:
`; comment` <br />
`mk [type] [identifier] [params]` <br />
`ct [identifier][output_index]>[identifier][input_index]` <br />
##### Example:
```
; This is a comment
mk osc sine 200      ; make an oscillator, call it 'sine' and set the default frequency to 200hz
mk out out out.raw   ; make a file output, call it 'out' and set the output file to the CWD\out.raw
                     ; non relative paths may be used
ct sine0>out0        ; connect output 0 on the oscillator to input 0 on the fileout
done                 ; generate the audio
```
##### Documentation:
~oscillator <br />
`mk osc [name] [frequency]` In: 1 Out: 1 <br />
Generates a sine wave at \[frequency] or \[input 0] hz <br /> <br />

~square <br />
`mk square [name] [frequency] [pulse width]` In: 2 Out: 1 <br />
Generates a square wave. \[pulse width]/\[input 1] ranges from -1 to 1, where 0 is 50% duty cycle <br /> <br />

~fileout <br />
`mk out [name] [file path]` In: 1 Out: 0 <br />
Writes \[input 0] to a file at \[file path] <br /> <br />

~noise <br />
`mk noise [name]` In: 0 Out: 1 <br />
Writes white noise to \[output 0] hz <br /> <br />
