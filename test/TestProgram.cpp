
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

clock~ 2
-> step~ 1..10|3
-> output

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);
	log("Finished parsing");

	for (uint n = 0; n < 30; n++) std::cout << prog.run(0.f) << '\n';
	prog.finish();
}

