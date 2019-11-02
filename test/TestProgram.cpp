
#include <iostream>
#include <fstream>
#include <array>

#include "../include/YggdrasilCore.h"
#include "Parser.h"

using namespace Yggdrasil;

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

f: 220
m: 2
file: "test_data.raw"

source: osc~ f * 0.5 * m
disk: file~ file
source{0} -> disk{0}

)";
	
	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < SAMPLE_RATE; n++) prog.run(n);
	prog.finish();
}


