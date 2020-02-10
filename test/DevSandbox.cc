
#include <iostream>
#include <fstream>
#include <array>
#include <iomanip>

#include "VolsungHeader.hh"

using namespace Volsung;

int main()
{
    Program prog;

    prog.configure_io(0, 1);
    debug_callback = [] (std::string message) {
        std::cout << std::setprecision(3) << message << '\n';
    };

    uint time = 10;
    bool print = false;
    Program::add_directive("length", [&time, &print] (std::vector<TypedValue> arguments, Program*) {
        time = arguments[0].get_value<Number>();
        if (arguments.size() > 1) print = (bool) arguments[1].get_value<Number>();
    });

    Program::add_directive("print", [] (std::vector<TypedValue> arguments, Program*) {
        Volsung::log((Text) arguments[0].as_string());
    });

    const std::string code =
R"(



; Try editing these
rate: 1s / 3
max_decay: 330ms
delay_scale: 1

Stereo_Delay <1, 2>: {
	; Stereo delay with adjustable feedback,
    ; left delay time, and right delay time

    fb: _1
    delay_L: _2
    delay_R: _3

	delay: [2] Delay_Line~ { delay_L, delay_R }[n-1]
	feedback: [2] Multiply~ fb
    attenuate: [2] Divide~ 3

    input <> delay
	=> feedback
	=> delay
    => [2] Tanh~
    => attenuate

    attenuate[0] -> 0|output
	attenuate[1] -> 1|output
}

Random_Range <1, 1>: {
	; Generates a random value in a given range
    ; every time a gate is recieved

    min: _1
    max: _2

    Noise~ -> Bi_to_Unipolar~
    -> Multiply~ max - min
    -> Add~ min
    -> snh: Sample_And_Hold~
    -> output

    input -> 1|snh
}

; Generate the frequencies of the pentatonic scale
freqs: (2^(1/12))^{ 0, 2, 4, 7, 9 } * 329.63

; Master clock
clock: Clock~ rate

; Trigger envelope into VCA
eg: Envelope_Generator~
vca: Multiply~
clock
-> eg
-> Smooth~ 30
-> 1|vca

; Randomise decay time on clock pulse
clock
-> Random_Range~ 10ms, max_decay
-> 1|eg


; Choose a random note from the
; pentatonic scale on clock pulse
clock
-> Random_Range~ 0, 4.99
-> Index_Sequence~ freqs
-> Sine_Oscillator~
-> vca

; Apply echo and output
echo: Stereo_Delay~ 0.5, 500ms * delay_scale, 833.33ms * delay_scale
vca -> echo
echo|0 -> File~ "left"
echo|1 -> File~ "right"

&length(10s)

)";

    Parser parser;
    parser.source_code = code;
    prog.reset();
    parser.parse_program(prog);
    log("Finished parsing");

    if (print) for (uint n = 0; n < time; n++)
        std::cout << prog.run(0.f) << '\n' << std::flush;

    else for (uint n = 0; n < time; n++) prog.run(0.f);
    prog.finish();
}

