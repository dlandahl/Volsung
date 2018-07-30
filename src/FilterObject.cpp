
#include "Yggdrasil.h"
#include "Block.h"
#include "objects.h"

void FilterObject::run()
{
	if (in_block_count() > 0)
	{
		Block b = read_block();

		b[0] = bal*b[0] + (1 - bal)*last;
		for (uint i = 1; i < BLOCKSIZE; i++)
		{
			b[i] = bal*b[i] + (1 - bal)*b[i - 1];
		}

		last = b[BLOCKSIZE - 1];

		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			b[i] *= 0.5f;
		}
		
		write_block(b);
	}
}

FilterObject::FilterObject() : last(0), bal(0.2f) { }