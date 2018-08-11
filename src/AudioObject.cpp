
#include <vector>

#include "AudioObject.h"

//This is code in the AudioObject class.
//Therefor, all AudioObjects such as oscillators and effects use this code,
//	because they derive from the AudioObject class.
//This code is used to read and write data to inputs and outputs.

//To read a block:
std::vector<Block> AudioObject::read_block()
{
	//Create a vector of blocks:
	std::vector<Block> b;
	
	//From each input to the AudioObject, read a block into
	//	one slot of the vector
	for (uint i = 0; i < inputs.size(); i++) {
		b.push_back(inputs[i].read_block());
	}
	
	//Return the vector
	return b;
	
	//This way, you may never read from just one particular input.
	//Thereby, audio processing stays in sync.
}

//To write a block to a particular output, just pass the block to that output.
void AudioObject::write_block(Block b, uint output)
{
	outputs[output].write_block(b);
}

AudioObject::AudioObject() { }
