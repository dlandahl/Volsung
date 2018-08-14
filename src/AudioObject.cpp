
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

void AudioObject::write_block(Block b, uint output)
{
	outputs[output].write_block(b);
}

void AudioObject::finish() { }

AudioObject::AudioObject() { }
