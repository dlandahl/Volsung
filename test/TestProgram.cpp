
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

major: { 0, 2, 4, 5, 7, 9, 11 }
f: 440

pitches: { f * (2^(1/12))^major[1 - 1], f * (2^(1/12))^major[3 - 1], f * (2^(1/12))^major[5 - 1] }

clock: clock~ sf / 6
disk:  file~  "melody.raw"

clock(0)
	-> step~   pitches / 3
	-> filter~ 20
	-> osc~    0 
	-> disk(0)

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < SAMPLE_RATE * 15; n++) prog.run(0.f);
	prog.finish();
}

