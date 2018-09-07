
#include "Yggdrasil.h"
#include "Block.h"

float& Block::operator[](uint n)
{
	return sample_data[n];
}

Block BlockBuffer::read_block()
{
	Block b;

	if (block_data.size())
	{
		b = block_data[0];
		block_data.erase(block_data.begin());
	}
	return b;
};

void BlockBuffer::write_block(Block b)
{
	block_data.push_back(b);
}
