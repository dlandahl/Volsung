
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

f: --100

source: add~ (f-f*3+50)*-1
source{0} -> output{0}

&length 10

)";
	
	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < time; n++) log(std::to_string(prog.run(0.f)));
	prog.finish();
}

