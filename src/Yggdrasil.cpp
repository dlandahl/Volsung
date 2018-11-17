
#include <iostream>
#include <fstream>

#include "Patch.h"
#include "Yggdrasil.h"
#include "AudioObject.h"

using namespace ygg;

int main(int argc, char* argv[])
{
	YggdrasilSymbolTable st;

	if (argc >= 2)
	{
		std::ifstream file;
		file.open(argv[1]);

		if (file) st.make_patch(file);
		else std::cout << "What?\n";
	}
	else
		st.make_patch(std::cin);
	
	for (int i = 0; i < 500; i++)
	{
		for (auto const& s : st)
			s.second->implement();
			
		AudioObject::index += BLOCKSIZE;
	}
	for (auto const& s : st)
		s.second->finish();
}
