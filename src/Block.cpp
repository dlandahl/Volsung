
#include "Yggdrasil.h"
#include "Block.h"

float& Block::operator[](size_t n)
{
	return sdata[n];
}

Block BlockBuffer::read_block()
{
	Block b;

	if (bdata.size())
	{
		b = bdata[0];
		bdata.erase(bdata.begin());
	}
	return b;
};

void BlockBuffer::write_block(Block b)
{
	bdata.push_back(b);
}