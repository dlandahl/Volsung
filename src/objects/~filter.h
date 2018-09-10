#pragma once

#include "../AudioObject.h"

class FilterObject : public AudioObject
{
	float  frequency;
	double a;
	double b;
	
	void run(buf&, buf&, int) override;

public:
	FilterObject(std::string);
};
