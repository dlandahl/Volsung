
#include "Rumpetroll.h"
#include "Block.h"
#include "objects.h"

Block SawtoothOscillatorObject::make_block()
{
	static uint block_index = 0;

	Block b;
	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		b[i] = 0;
		for(int j = 1; j < 150; j++)
			b[i] += (float)sinf((float)(i + block_index*BLOCKSIZE)
								/ SAMPLE_RATE * 110 * TAU * j) / j;
	}

	block_index++;
	return b;
}

void SawtoothOscillatorObject::run()
{
	if (out_block_count() < 5)
	{
		write_block(make_block());
	}
}