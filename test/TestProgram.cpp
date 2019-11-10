
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
degree: 3
f: 440

transposed: major + 1

source: add~ transposed[1]


; f * (2^(1/12))^major[degree - 1]
source(0) -> output(0)

)";
	
	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < 10; n++) log(std::to_string(prog.run(0.f)));
	prog.finish();
}

