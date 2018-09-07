#pragma once

#include <string>

#include "../AudioObject.h"

class OscillatorObject : public AudioObject
{
	void  run(buf&, buf&, int) override;
		  
	float phase;
	float frequency;

public:
	OscillatorObject(std::string);
};
