
#include "Yggdrasil.h"
#include "Block.h"
#include "LFOObject.h"

Block LFOObject::make_block()
{
	static uint block_index = 0;

	Block b;
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		b[i] = sinf((i + block_index*BLOCKSIZE) * 10.0_hz);
	}

	block_index++;
	return b;
}

void LFOObject::run()
{
	write_block(make_block(), 0);
}

LFOObject::LFOObject(std::string)
{
	outputs.push_back({});
}