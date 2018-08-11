
#include <iostream>

#include "Yggdrasil.h"
#include "Block.h"

//A block is an array of samples. Samples are just amplitudes
//	stored as float values.

//This overloads the [] operator to read from a block as though it were
//	an array.
float& Block::operator[](size_t n)
{
	return sdata[n];
}

//A BlockBuffer is a vector of blocks.
Block BlockBuffer::read_block()
{
	Block b;

	//When we try to read from a BlockBuffer, the block
	//	that is returned gets deleted, so it cannot be read twice.
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
