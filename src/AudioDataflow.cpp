
#include "Block.h"
#include "AudioDataflow.h"

Block AudioInput::read_block()
{
	Block b;

	if (connections.size()) {
		for (uint j = 0; j < connections.size(); j++) {
			Block other_block = connections[j]->buf.read_block();
	
			for (uint i = 0; i < BLOCKSIZE; i++) {
				b[i] = other_block[i];
			}
		}

		//for (uint i = 0; i < BLOCKSIZE; i++) {
		//	b[i] /= connections.size();
		//}
	}
	return b;
}

void AudioOutput::write_block(Block b)
{
	for (uint i = 0; i < connections.size(); i++) {
		connections[i]->buf.write_block(b);
	}
}