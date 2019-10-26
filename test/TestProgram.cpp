
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
ct osc  0 >     output3487; we got stuff goin on
done

)";
	
	//prog.make_graph(std::stringstream(code));
	//for (uint n = 0; n < 100; n++) std::cout << prog.run(n) << '\n';

	Lexer lexer;
	lexer.source_code = code;

	Token token;
	while (true) {
		std::cin.get();
		token = lexer.get_next_token();

		std::cout << token.type << std::endl;
		if (std::holds_alternative<std::string>(token.value))
			std::cout << std::get<std::string>(token.value) << std::endl;
			
		if (std::holds_alternative<int>(token.value))
			std::cout << std::get<int>(token.value) << std::endl;
	}
}
