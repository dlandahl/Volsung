
#include <iostream>
#include <fstream>

#include "YggdrasilCore.h"

using namespace Yggdrasil;

int main(int argc, char ** argv)
{
	Patch st;
	float array[10] = { 0 };

	callback_functor output_callback = [&array] (buf& input, buf&, int n) {
		array[n % 10] = input[0][n];
		return nullptr;
	};

	SymbolTable::add_directive("three", [] (std::vector<std::string> args, SymbolTable*) {
		for (auto arg: args) log(arg);
		for (auto arg: args) log(arg);
		for (auto arg: args) log(arg);
	});

	
	st.create_user_object("output", 1, 0, output_callback);

	debug_callback = [] (std::string message) { std::cout << message; };
	
 	if (argc >= 2)
 	{
 		std::ifstream file(argv[1]);
 
 		if (file) st.make_patch(file);
 		else std::cout << "What?\n";
 	}
 	else
 		st.make_patch(std::cin);

	
	for (uint n = 0; n < 10; n++) st.run();
	for (uint n = 0; n < 10; n++) std::cout << array[n];
	
//st.create_object<UserObject>("output", "output

//	st.create_object<FilterObject>("osc", "osc osc 200");
//	st.create_object<FileoutObject>("out", "out out.raw");
//	Patch::connect_objects(st, "osc", 0, "out", 0);
}
