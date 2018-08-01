
#include "Yggdrasil.h"
#include "Block.h"
#include "LFOObject.h"

Block LFOObject::make_block()
{
	static uint block_index = 0;

	Block b;
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		b[i] = sinf((float)(i + block_index*BLOCKSIZE) / SAMPLE_RATE * 1 * TAU);
	}

	block_index++;
	return b;
}

void LFOObject::run()
{
	write_block(make_block(), 0);
}

LFOObject::LFOObject()
{
	outputs.push_back({});
}