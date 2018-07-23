#include "Rumpetroll.h"
#include "Block.h"
#include "objects.h"

Block OscillatorObject::make_block()
{
	static uint block_index = 0;

	Block b;
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		b[i] = sinf((float)(i + block_index*BLOCKSIZE) / SAMPLE_RATE * 110 * TAU);
	}

	block_index++;
	return b;
}

void OscillatorObject::run()
{
	if (out_block_count() < 5)
	{
		write_block(make_block());
	}
}