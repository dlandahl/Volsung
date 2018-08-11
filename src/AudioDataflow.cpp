
#include <iostream>

#include "Block.h"
#include "AudioDataflow.h"

//Blocks are arrays of audio samples stored as floats

//When an AudioObject wants to read a block from an input:
Block AudioInput::read_block()
{
	//Create a block that will be read into
	Block b;

	//If there are connections to the input:
	if (connections.size()) {
		//For each connection:
		for (uint j = 0; j < connections.size(); j++) {
			//Load the block from that connection
			Block other_block = connections[j]->buf.read_block();
			//And paste it into the block we created
			for (uint i = 0; i < BLOCKSIZE; i++) {
				b[i] = other_block[i];
			}
		}
	}
	//Then return that block
	//If there are no connections, the block will be zeroes
	return b;
}

//To write a block to an output:
void AudioOutput::write_block(Block b)
{
	//Just take ablock and write it to the buffer of each connection
	for (uint i = 0; i < connections.size(); i++) {
		connections[i]->buf.write_block(b);
	}
}
