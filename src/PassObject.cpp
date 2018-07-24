
#include "Yggdrasil.h"
#include "Block.h"
#include "objects.h"

void PassObject::run()
{
	if (in_block_count() > 0)
	{
		write_block(read_block());
	}
}