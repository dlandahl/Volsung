
#include <fstream>
#include <vector>

#include "FileoutObject.h"
#include "Yggdrasil.h"
#include "Block.h"

void FileoutObject::run()
{
	std::vector<Block> b = read_block();
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		data.push_back(b[0][i]);
	}
}

FileoutObject::FileoutObject(std::string)
{
	inputs.push_back({});
}