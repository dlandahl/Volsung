
#include "Rumpetroll.h"
#include "Block.h"
#include "objects.h"

void FilterObject::run()
{
	if (in_block_count() > 0)
	{
		Block b = read_block();

		b[0] = .01*b[0] + .99*last;
		for (int i = 1; i < BLOCKSIZE; i++)
		{
			b[i] = .01f*b[i] + .99f*b[i - 1];
			b[i] *= 0.5;
		}

		last = b[BLOCKSIZE - 1];
		write_block(b);
	}
}

FilterObject::FilterObject() : last(0) { }