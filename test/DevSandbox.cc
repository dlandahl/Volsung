
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


; Define a vocoder type
Vocoder(2, 1): {
    N: _1

    ; Frequency bands are spaced evenly on a logarithmic scale
    frequency_bands: ((1..N)/N)^2 * 3000
    
    &print "Frequency Bands: "
    &print frequency_bands
    
    ; Define bandpass filter banks for carrier and modulator
    carrier_bands: [N] Bandpass_Filter~ frequency_bands[n-1], 16
    mod_bands: [N] Bandpass_Filter~ frequency_bands[n-1], 8

    ; Envelope followers applied to the modulator bands determine
    ; the gain applied to the carrier bands
    amps: [N] Multiply~
    followers: [N] Envelope_Follower~ 10ms

    ; Connect them up
    input|0 <> mod_bands     => followers => 1|amps
    input|1 <> carrier_bands => amps      >> /6 -> output
}

&length 13.5s

; Make a vocoder with twenty-four bands
voc: Vocoder~ 24

; Acquire carrier and modulator signals
carrier: [4] Saw_Oscillator~ n*85
modulator: File~ "antikythera"

carrier >> 1|voc
modulator -> voc -> File~ "vocode"

)";

    Parser parser;
    parser.source_code = code;
    prog.reset();
    parser.parse_program(prog);
    log("Finished parsing");

    if (print) for (uint n = 0; n < time; n++) std::cout << prog.run(0.f) << '\n' << std::flush;
    else for (uint n = 0; n < time; n++) prog.run(0.f);
    prog.finish();
}

