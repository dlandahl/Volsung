#pragma once

#include "Block.h"

class AudioOutput;

class AudioConnector
{

public:
	AudioOutput* out;
	Block get_block();
	uint get_block_count();
};

class AudioInput { public: AudioConnector* connection; };

class AudioOutput
{
	BlockBuffer buf;

public:
	AudioConnector* connection;

	Block get_block();
	void write_block(Block b);
	uint get_block_count();
};