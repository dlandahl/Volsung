#pragma once

#include <string>

#include "../AudioObject.h"

class SquareObject : public AudioObject
{
	void  run(buf&, buf&, int) override;
	
	float pw        = 0.0;
	float phase     = 0;
	float frequency = 100;

public:
	SquareObject(std::string);
};
