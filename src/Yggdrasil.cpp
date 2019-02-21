
#include <iostream>
#include <fstream>

#include "Patch.h"
#include "Yggdrasil.h"
#include "AudioObject.h"
#include "Objects.h"

using namespace Yggdrasil;

int main(int argc, char ** argv)
{
	Patch st;

 	if (argc >= 2)
 	{
 		std::ifstream file(argv[1]);
 
 		if (file) st.make_patch(file);
 		else std::cout << "What?\n";
 	}
 	else
 		st.make_patch(std::cin);

//	st.create_object<FilterObject>("osc", "osc osc 200");
//	st.create_object<FileoutObject>("out", "out out.raw");
//  Patch::connect_objects(st, "osc", 0, "out", 0);

	for (int i = 0; i < 500; i++)
		for (auto const& s : st)
			s.second->implement();

	for (auto const& s : st)
		s.second->finish();
}
