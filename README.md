
### The language that does bleep bloop

Syntax:
```
my_source: osc~ 400
my_output: file~ "~/Documents/output.raw"
my_source(0) -> my_output(0)
```

Constants, expressions, and comments:
```
f: 440
source: osc~ f
octave: osc~ f*2

disk: file~ "~/Documents/output.raw"
source(0) -> disk(0)
octave(0) -> disk(0) ; This will cause clipping
; Not a problem because we use 32bit PCM
```

Inline arithmetic and string constants:
```
output_path: "~/Documents/output.raw"

source: noise~
mod: osc~ 0.5
gain: mult~

source(0) -> gain(0)
mod(0)
	-> *0.5
	-> +0.5
	-> gain(1)

disk: file~ output_path
gain(0) -> disk(0)
```

Sequences, index sequences with sequences, step through sequences:
```
value: { 100, 200, 300, 400, 500, 600, 700 }
index: { 2, 3, 2, 2, 1, 5, 3, 4, 2, 2, 3 }

clock: clock~ sf / 5
disk: file~ "melody.raw"

clock(0)
-> step~ value[index]
-> filter~ 5
-> osc~
-> *0.2 -> disk(0)
```