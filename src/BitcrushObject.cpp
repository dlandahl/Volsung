
#include "Yggdrasil.h"
#include "Block.h"
#include "objects.h"

#define RESOLUTION 5

void BitcrushObject::run()
{
	if (in_block_count() > 0)
	{
		Block b = read_block();

		for (int i = 0; i < BLOCKSIZE; i++) {
			float val = b[i];
			val *= RESOLUTION;
			val = (int)val;
			val = ((float)val) / RESOLUTION;

			b[i] = val;
		}

		write_block(b);
	}
}