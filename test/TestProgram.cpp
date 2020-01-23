
#include <iostream>
#include <fstream>
#include <array>

#include "Volsung.hh"

using namespace Volsung;

int main()
{
    Program prog;

    prog.configure_io(0, 1);

    debug_callback = [] (std::string message) { std::cout << message; };

    uint time = 10;
    bool print = false;
    Program::add_directive("config", [&time, &print] (std::vector<TypedValue> arguments, Program*) {
        time = arguments[0].get_value<Number>();
        if (arguments.size() > 1) print = (bool) arguments[1].get_value<Number>();
    });

    std::string code =
R"(

; Use five saw oscillators
N: 5
swarm: [N] Saw_Oscillator~ 

; Random offset frequencies
offset: [N] Multiply~ (r-0.5) * 200

; The base frequency for each oscillator will
; be added to the offset
base_frequency: [N] Add~ n * 55

; Use a timer~ to decrease the amount of offset over time
; sat~ is used to stop the timer exceeding 1
Timer~
-> Tanh~ -> *-1 -> +1
<> offset
=> base_frequency
=> swarm
>> *0.1 -> output


&config 10, 1

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
