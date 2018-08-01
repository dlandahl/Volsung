#pragma once

#include <vector>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "Block.h"

class Object { };

class AudioObject : public Object
{
protected:
	virtual std::vector<Block> read_block();
	virtual void write_block(Block b, uint output);

public:
	std::vector<float> data;
	//uint in_block_count(uint input);
	//uint out_block_count(uint output);

	std::vector<AudioInput> inputs;
	std::vector<AudioOutput> outputs;

	virtual void run() = 0;
};