
#include <vector>

#include "AddObject.h"

void AddObject::run()
{
	std::vector<Block> b = read_block();
	Block out;

	if (inputs[1].connections.size())
	{
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			out[i] = b[0][i] + b[1][i];
		}
	} else {
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			out[i] = b[0][i] + default_value;
		}
	}

	write_block(out, 0);
}

AddObject::AddObject(std::string arg)
{
	try { default_value = std::stoi(arg, nullptr, 10); }
	catch (...) { default_value = 0; }

	inputs.push_back({});
	inputs.push_back({});
	outputs.push_back({});
}