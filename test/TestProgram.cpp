
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

value: { 100, 200, 300, 400, 500, 600, 700 }
index: { 2, 3, 2, 2, 1, 5, 3, 4, 2, 2, 3 }

clock: clock~ sf / 5
disk: file~ "melody.raw"

clock(0)
-> step~ value[index]
-> filter~ 5
-> osc~
-> *0.2 -> disk(0)

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);
	log("Finished parsing");
	
	for (uint n = 0; n < SAMPLE_RATE * 60; n++) prog.run(0.f);
	prog.finish();
}

