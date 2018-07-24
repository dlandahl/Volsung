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
	AudioConnector connection;

public:
	Block get_block();
	void write_block(Block);
	uint get_block_count();
	void connect(AudioInput &);

	AudioOutput();
};