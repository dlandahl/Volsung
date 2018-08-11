#pragma once

#include "Block.h"

//An AudioConnector object is used to move data between inputs and outputs
//An AudioConnector contains a buffer, which is a vector of Blocks.
//Blocks are arrays of samples.

//An AudioObject may write a block to an output, which will place it in all
//	AudioConnectors that are connected. THE input that each Connector
//	is also connected to can then read the block.
class AudioConnector
{
public:
	BlockBuffer                  buf;
};

//An input has a list of pointers to connectors that are connected, and a function
//	for reading from their buffers.
class AudioInput
{
public:
	std::vector<AudioConnector*> connections;
	Block                        read_block();
};

//An output is the opposite, it has a list of pointers to connectors, and a function
//	for writing to them.
//This way multiple connections can be on one input or output.
class AudioOutput
{
public:
	std::vector<AudioConnector*> connections;
	void                         write_block(Block);
};
