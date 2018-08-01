#pragma once

#include "Block.h"

class AudioOutput;

class AudioConnector
{
public:
	BlockBuffer buf;
	//AudioOutput* out;	
};

class AudioInput
{
public:
	std::vector<AudioConnector*> connections;
	Block read_block();
	
};

class AudioOutput
{
public:
	std::vector<AudioConnector*> connections;
	void write_block(Block);
	//void connect(AudioInput &);

	//AudioOutput();
};