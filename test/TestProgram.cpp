
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

	std::string code =
R"(

mk noise~ osc

mk filter~ a 100
mk filter~ b 100
mk filter~ c 100
mk filter~ d 100
mk filter~ e 100

mk mult~ gain 100000
mk add~ offset 500

ct osc(0) -> a(0)
ct a(0) -> b(0)
ct b(0) -> c(0)
ct c(0) -> d(0)
ct d(0) -> e(0)

ct e(0) -> gain(0)
ct gain(0) -> offset(0)

mk osc~ tone
ct offset(0) -> tone(0)

mk file~ file output.raw
ct tone(0) -> file(0)
ct offset(0) -> output(0)
)";
	
	//prog.make_graph(std::stringstream(code));
	//for (uint n = 0; n < 100; n++) std::cout << prog.run(n) << '\n';

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < SAMPLE_RATE * 10; n++) std::cout << prog.run(n) << '\n';
	prog.finish();
}

