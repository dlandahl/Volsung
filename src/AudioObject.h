#pragma once

#include "Rumpetroll.h"
#include "AudioDataflow.h"
#include "Block.h"

class Object { };

class AudioObject : public Object
{
protected:
	Block read_block();

	uint in_block_count();
	uint out_block_count();

	void write_block(Block b);
public:

	AudioInput input;
	AudioOutput output;

	virtual void run() = 0;
};