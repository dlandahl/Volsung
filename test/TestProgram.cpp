
#include <iostream>
#include <fstream>
#include <array>

#include "../include/YggdrasilCore.h"

using namespace Yggdrasil;

struct UserData {
	std::array<float, 10> data = { 0 };
};

int main(int argc, char ** argv)
{
	Patch st;
	UserData data;

	callback_functor output_callback = [] (buf& input, buf&, int n, std::any user_data) {
		UserData array = std::any_cast<UserData>(user_data);
		static int count = 0;
		if (count < array.data.size()) {
			array.data[count] = input[0][n];
			count++;
		}
	};

	SymbolTable::add_directive("three", [] (std::vector<std::string> args, SymbolTable*) {
		for (auto arg: args) log(arg);
		for (auto arg: args) log(arg);
		for (auto arg: args) log(arg);
	});

	
	st.create_user_object("output", 1, 0, data, output_callback);

	debug_callback = [] (std::string message) { std::cout << message; };
	
 	if (argc >= 2)
 	{
 		std::ifstream file(argv[1]);
 
 		if (file) st.make_patch(file);
 		else std::cout << "What?\n";
 	}
 	else
 		st.make_patch(std::cin);

	
	for (uint n = 0; n < 1000000; n++) st.run();
    st.finish();

}
