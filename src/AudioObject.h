#pragma once

#include <vector>
#include <string>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "Block.h"

//An audio object might be an oscillator, effect or anything else you can think of.
//It is an abstract class because of the pure virtual function run();
//run() = 0; forces derived classes to implement this function.
//	That function is used to do the operation of the object, i.e. read a block from an input,
//	change the data in the block, then output that block on a particular output.
class AudioObject
{
protected:
	//The read_block function returns a vector of blocks, one from each input.
	virtual std::vector<Block> read_block();
	
	//The write_block function writes a block to a specified output.
	virtual void               write_block(Block b, uint output);
	
	std::string                args;
public:
	std::vector<float>         data;

	//AudioInputs and AudioOutputs are themselves classes, you can find them
	//in the AudioDataflow header and cpp file.
	//This AudioObject class contains a vector of each, allowing any number of inputs
	//	or outputs to be added. How many of each is decided by the derived classes,
	//	i.e. the specialisations such as oscillators or effects.
	std::vector<AudioInput>    inputs;
	std::vector<AudioOutput>   outputs;

	virtual void               run() = 0;

	AudioObject();
};
