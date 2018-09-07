#pragma once

#include "../AudioObject.h"

class FilterObject : public AudioObject
{
	float frequency;
	float a;
	float b;

public:
	void run(buf&, buf&, int) override;
	FilterObject(std::string);
};
