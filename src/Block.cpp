#include "Rumpetroll.h"
#include "Block.h"

float& Block::operator[](size_t n)
{
	return sdata[n];
}

Block BlockBuffer::get_block()
{
	Block b = bdata[0];
	bdata.erase(bdata.begin());
	block_count -= 1;
	return b;
};

void BlockBuffer::write_block(Block b)
{
	bdata.push_back(b);
	block_count += 1;
}

uint BlockBuffer::get_block_count()
{
	return block_count;
}

BlockBuffer::BlockBuffer() : block_count(0) { }