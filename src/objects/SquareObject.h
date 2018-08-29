#pragma once

#include <string>

#include "AudioObject.h"

class SquareObject : public AudioObject
{
	void  run() override;
	Block make_block();
		  
    float sample_count;
    float sample_index;
	float frequency;
    int value;
public:
	SquareObject(std::string);
};
