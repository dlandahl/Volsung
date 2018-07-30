#pragma once

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "Block.h"

class Object { };

class AudioObject : public Object
{
protected:
	Block read_block();
	uint out_block_count();
	void write_block(Block b);

public:
	std::vector<float> data;
	uint in_block_count();

	AudioInput input;
	AudioOutput output;

	virtual void run() = 0;
};