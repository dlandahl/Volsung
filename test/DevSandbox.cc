
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

file_name: "Test_File.raw"
write_file("Other_File", read_file(file_name)^2)

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

