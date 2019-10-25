
#include <iostream>
#include <fstream>
#include <array>

#include "../include/YggdrasilCore.h"

using namespace Yggdrasil;

int main(int argc, char ** argv)
{
	Program prog;

	prog.configure_io(1, 1);
	
	debug_callback = [] (std::string message) { std::cout << message; };

	/*
 	if (argc >= 2)
 	{
 		std::ifstream file(argv[1]);
 
 		if (file) st.make_graph(file);
 		else std::cout << "What?\n";
 	}
 	else
 		st.make_graph(std::cin);
	*/

	std::string code =
R"(

mk mult mult 5
mk delay delay 10
ct input0>mult0
ct mult0>delay0
ct delay0>output0
done

)";
	
	prog.make_graph(std::stringstream(code));
	
	for (uint n = 0; n < 100; n++) std::cout << prog.run(n) << '\n';
}
