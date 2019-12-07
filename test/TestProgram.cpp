
#include <iostream>
#include <fstream>
#include <array>

#include "Volsung.hh"

using namespace Volsung;

int main(int argc, char ** argv)
{
	Program prog;

	prog.configure_io(0, 1);

	debug_callback = [] (std::string message) { std::cout << message; };

	uint time = 10;
	bool print = false;
	Program::add_directive("config", [&time, &print] (std::vector<TypedValue> arguments, Program*) {
		time = arguments[0].get_value<float>();
		if (arguments.size() > 1) print = (bool) arguments[1].get_value<float>();
	});

	std::string code =
R"(

clock~ 6s
-> lpf~ 500, 2^(1/2)/2
-> [2] file~ "output.raw"

&config 10s

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);
	log("Finished parsing");

	if (print) for (uint n = 0; n < time; n++) std::cout << prog.run(0.f) << '\n' << std::flush;
	else for (uint n = 0; n < time; n++) prog.run(0.f);
	prog.finish();
}
