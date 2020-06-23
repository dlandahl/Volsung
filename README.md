
## Overview
Volsung is a new language for audio, currently in development. It can be used for sound synthesis, audio effect processing, and procedural game audio. Aimed at sound designers, it prioritises being visual and intuitive while providing language constructs and abstractions that enable readable and terse algorithm specification. Volsung draws inspiration from similar DSP languages such as Miller Puckette's [Pure Data](https://puredata.info), Stanford's [ChucK](http://chuck.stanford.edu), and [FAUST](https://faust.grame.fr), developed at GRAME. 

## How to Use
### Web Interpreter
There is a web implementation based on Emscripten, targeting WebAssembly. You can use it to get an idea of what the language is.
<br /> You can try it at: http://landahl.tech/volsung/ 

### API
You can compile the source into a library and use a simple C++ API to compile code and generate audio in any environment, such as game engine, DAW plugins, etc.
<br /> <br /> Include the `include/Volsung.hh` header file, then use the example below to understand how to use the API:

```c++

#include "Volsung.hh"

inline auto generate_audio(const std::string& volsung_code) -> Volsung::MultichannelBuffer {
    // Create a program
    Volsung::Program program;

    // Set the program to have one output and no input channels
    program.configure_io(0, 1);

    // You can optionally supply a function pointer to display error messages
    Volsung::set_debug_callback([] (const std::string& message) {
        std::cout << message << '\n' << std::flush;
    });

    // Create a parser and specify the code to be parsed
    Volsung::Parser parser;
    parser.source_code = volsung_code;

    program.reset();
    parser.parse_program(program);

    // Run the program. This generates and returns a block of 64 samples per output channel.
    // If you wanted to have inputs in the program, you would pass a `MultichannelBuffer` into the `run` function.
    Volsung::MultichannelBuffer output = program.run();

    // `MultichannelBuffer` is a dynamic array of blocks, which are arrays of samples of size 64.
    // You can get the data out by subscripting it.
    return output;
}
```

### Unity game engine
There is a plugin to use the language in the Unity game engine.
See here: https://github.com/long-march/Volsung-Unity
