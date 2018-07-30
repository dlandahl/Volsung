
#include "AudioObject.h"

Block AudioObject::read_block()
{
	return input.connection->get_block();
}

uint AudioObject::in_block_count() { return input.connection->get_block_count(); }
uint AudioObject::out_block_count() { return output.get_block_count(); }

void AudioObject::write_block(Block b)
{
	output.write_block(b);
}