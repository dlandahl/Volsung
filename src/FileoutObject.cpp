#include <fstream>

#include "Rumpetroll.h"
#include "objects.h"
#include "Block.h"

void FileoutObject::run()
{
	if (in_block_count() > 0)
	{
		Block b = read_block();
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			data.push_back(b[i]);
		}
	}
}