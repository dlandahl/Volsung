
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

; hi there
; hello
; how goes it
mk    osc~ osc 1234
mk delay~ ddl 10
ct osc  0 >     ddl0; we got stuff goin on
ct ddl0>output0

)";
	
	//prog.make_graph(std::stringstream(code));
	//for (uint n = 0; n < 100; n++) std::cout << prog.run(n) << '\n';

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);

	for (uint n = 0; n < 100; n++) std::cout << prog.run(n) << '\n';
	
}
