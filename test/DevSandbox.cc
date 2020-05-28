
#include <iostream>
#include <fstream>
#include <array>
#include <iomanip>

#include "Volsung.hh"

using namespace Volsung;

int main()
{
    Program prog;

    prog.configure_io(0, 1);
    set_debug_callback([] (std::string message) {
        std::cout << std::setprecision(3) << message << '\n';
    });

    uint time = 10 * get_sample_rate();
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

; Defining a function for converting to decibel
to_db: |lin| { 20 * log(lin) }

N: 2^4

; Generate a component at half nyquist and one at DC
; The value.operation(operand) syntax is equivalent to operation(value, operand)
data: {0, 1, 0, -1}.repeat(N / 4) + 1

; Generate a Hann window
window: sin(pi*(0..(N-1)) / N)^2

; Analyse it and take the lower half - we don't need to print the aliases.
spectrum: (data * window).FFT()[0..N / 2]

; Display the magnitude spectrum
; Notice a peak in the middle (fs/4) and one on the left (dc)
print("Magnitudes (dB): ", spectrum.abs().to_db())

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
