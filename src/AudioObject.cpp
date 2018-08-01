
#include <vector>

#include "AudioObject.h"

std::vector<Block> AudioObject::read_block()
{
	std::vector<Block> b;
	for (uint i = 0; i < inputs.size(); i++) {
		b.push_back(inputs[i].read_block());
	}
	return b;
}

//uint AudioObject::in_block_count() { return input.connection->get_block_count(); }
//uint AudioObject::out_block_count() { return output.get_block_count(); }

void AudioObject::write_block(Block b, uint output)
{
	outputs[output].write_block(b);
}