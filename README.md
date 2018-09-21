# Yggdrasil

Yggdrasil is at the moment a scripting language for sound design. You put together small audio processing modules such as oscillators, filters, signal adders, signal multipliers. Someone will make a GUI at some point. Read from a file or enter code line by line.

### Syntax:
`; comment` <br />
`mk [type] [identifier] [params]` <br />
`ct [identifier][output_index]>[identifier][input_index]` <br />
<br />
### Example:
```
; This is a comment
mk osc sine 200      ; make an oscillator, call it 'sine' and set the default frequency to 200hz
mk out out out.raw   ; make a file output, call it 'out' and set the output file to the CWD\out.raw
                     ; non relative paths may be used
ct sine0>out0        ; connect output 0 on the oscillator to input 0 on the fileout
done                 ; generate the audio
```
<br />
### Documentation:
