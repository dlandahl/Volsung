
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

    uint time = 10 * sample_rate;
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

(e^(pi*i)).print()

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
