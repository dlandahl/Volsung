
#include <iostream>
#include <fstream>
#include <array>

#include "Volsung.hh"

using namespace Volsung;

int main(int argc, char ** argv)
{
	Program prog;

	prog.configure_io(1, 1);

	debug_callback = [] (std::string message) { std::cout << message; };

	uint time = 10;
	Program::add_directive("length", [&time] (std::vector<std::string> arguments, Program*) {
		time = std::stof(arguments[0]);
	});

	std::string code =
R"(

scale: { 0, 2, 3, 5, 7, 8, 10, 12 }
root: 440

freqs: (2^(1/12))^scale * root

clock: clock~ sf / 5
disk: file~ "melody.raw"

tremolo: mult~
mod: osc~ 8
mod(0) -> *0.7 -> +0.3 -> tremolo(1)

clock(0)
-> step~ freqs[{ 1, 2, 3, 5, 7, 8, 7, 5, 3, 2 } - 1]
-> filter~ 10
-> osc~
-> tremolo(0)
tremolo(0) -> disk(0)

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);
	log("Finished parsing");
	
	for (uint n = 0; n < SAMPLE_RATE * 20; n++) prog.run(0.f);
	prog.finish();
}

