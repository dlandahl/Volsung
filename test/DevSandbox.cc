
#include <iostream>
#include <fstream>
#include <array>
#include <iomanip>

#include "Volsung.hh"

using namespace Volsung;

int main()
{
    Program prog;

    prog.configure_io(0, 2);
    set_debug_callback([] (const std::string& message) {
        std::cout << std::setprecision(3) << message << '\n';
    });

    uint time = get_sample_rate();
    bool print = false;

    const std::string code =
R"(

; Try listening without the reverb, to understand the
; effect the program has on the raw input
enable_reverb: true

Delay_Modulator <0, 1>: {
    ; A subgraph that modulates a delay buffer relative to its length

    size: _1
    rate: _2
    mod_depth: _3

    Sine_Oscillator~ rate, random(tau)
    -> Bi_to_Unipolar~
    -> Multiply~    size * mod_depth
    -> Add~  size - size * mod_depth
    -> output
}

One_Pole <1, 1>: {
    ; A filter with a single pole on the real axis.
    ; Used as a damping filter in the reverb feedback loop, to simulate
    ; higher frequency components dissipating more quickly

    a: _1
    input -> *1-a -> Pole~ a -> output
}

LBCF <1, 1>: {
    ; Modulated Schroeder-Moorer lowpass feedback comb filter
    ; https://ccrma.stanford.edu/~jos/pasp/Lowpass_Feedback_Comb_Filter.html

    N: _1   ; Filter order
    f: _2   ; Feedback
    d: 0.22 ; Damping

    input
    -> sum: Add~
    -> buffer: Delay_Line~ N
    -> * -f
    -> One_Pole~ d
    -> 1|sum

    -> output

    Delay_Modulator~ N, random(0.1), 0.1
    -> 1|buffer
}

Reverb <1, 2>: {
    offset: _1
    
    ; From freeverb,
    orders: { 1557, 1617, 1491, 1422, 1277, 1356, 1188, 1113 } + offset

    ; Eight parallel combs create a sufficient echo density
    input <> [8] LBCF~ orders[n-1], _2
    >> * 0.1 -> output
}

Switch <1, 2>: {
    input <> bypass: [2] Multiply~ _1 - (n-1)
    bypass[0] -> 0|output
    bypass[1] -> 1|output
}

decay: 0.89

; Also try `files/drumbeat`
path: "../test/test_programs/files/antikythera"

Read_File~ path
-> bypass: Switch~ enable_reverb
<> reverb: [2] Reverb~ 20*(n-1), decay

reverb[0] -> 0|output
reverb[1] -> 1|output
bypass|1 -> 0|output
bypass|1 -> 1|output

)";

    Parser parser;
    parser.source_code = code;
    prog.reset();
    parser.parse_program(prog);
    log("Finished parsing");

    const size_t num_blocks = time / AudioBuffer::blocksize;
    if (print) for (uint n = 0; n < num_blocks; n++)
        for (uint n = 0; n < AudioBuffer::blocksize; n++)
            std::cout << prog.run()[0][n] << '\n' << std::flush;

    else for (uint n = 0; n < num_blocks; n++) prog.run();
    prog.finish();
}
