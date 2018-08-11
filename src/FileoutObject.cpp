
#include <fstream>
#include <vector>

#include "FileoutObject.h"
#include "Yggdrasil.h"
#include "Block.h"

//The run function for this AudioObject is implemented to simply save
//	data from its input to its data storage, which is later written
//	to a file.
void FileoutObject::run()
{
	std::vector<Block> b = read_block();
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		data.push_back(b[0][i]);
	}
}

//This is the constructor for the object.
FileoutObject::FileoutObject(std::string)
{
	//In the constructor, we push back the AudioInput array, to create a single input
	//	This input will read the data that will be stored in a file
	inputs.push_back({});
}
