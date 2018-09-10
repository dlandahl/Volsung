
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <type_traits>

#include "Patch.h"
#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "AudioObject.h"
#include "StringFormat.h"

int main(int argc, char* argv[])
{
	st_type       symbol_table;
	st_type& st = symbol_table;

	if (argc >= 2)
	{
		std::ifstream file;
		file.open(argv[1]);

		make_patch(st, file);
	}
	else
		make_patch(st, std::cin);
	
	for (uint i = 0; i < 500; i++)
	{
		for (auto const& s : st)
			s.second->implement();
	
		AudioObject::index += BLOCKSIZE;
	}
		for (auto const& s : st)
			s.second->finish();
}
