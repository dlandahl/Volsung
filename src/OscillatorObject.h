#pragma once

#include <string>

#include "AudioObject.h"

class OscillatorObject : public AudioObject
{
	void  run() override;
	Block make_block();
		  
	float phase;

	float frequency;
	uint  block_index;
public:
	OscillatorObject(std::string);
};