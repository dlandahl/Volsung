
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

name: { 0, 50, 1 }
n: 2
other: 45
source: add~ { other, name[1] }[name[n]]
source(0) -> output(0)

)";
	
	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < 10; n++) log(std::to_string(prog.run(0.f)));
	prog.finish();
}

