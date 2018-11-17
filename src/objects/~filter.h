#pragma once

#include "../AudioObject.h"

namespace Yggdrasil {

class FilterObject : public AudioObject
{
	float  frequency = 100;
	double a;
	double b;
	
	void run(buf&, buf&, int) override;

public:
	FilterObject(std::string);
};

}