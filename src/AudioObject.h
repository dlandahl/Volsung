#pragma once

#include <vector>
#include <string>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "Block.h"

class Object { };

class AudioObject : public Object
{
protected:
	virtual std::vector<Block>	read_block();
	virtual void				write_block(Block b, uint output);

	std::string					args;
public:
	std::vector<float>			data;

	std::vector<AudioInput>		inputs;
	std::vector<AudioOutput>	outputs;

	virtual void				run() = 0;

	AudioObject();

};