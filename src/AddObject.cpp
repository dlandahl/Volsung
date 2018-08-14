
#include <fstream>
#include <vector>

#include "AddObject.h"
#include "Yggdrasil.h"
#include "Block.h"

void AddObject::run()
{
	std::vector<Block> b = read_block();
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		data.push_back(b[0][i]);
	}
}

void AddObject::finish()
{
	std::fstream file(filename, std::fstream::out | std::fstream::binary);

	for (uint i = 0; i < data.size(); i++) {
		file.write((const char*)& data[i], sizeof(float));
	}

	file.close();
}

AddObject::AddObject(std::string filename) : filename(filename)
{
	inputs.push_back({});
}