#pragma once

#include "Block.h"

class AudioConnector
{
public:
	BlockBuffer buf;
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
};