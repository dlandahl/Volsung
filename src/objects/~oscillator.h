#pragma once

#include <string>

#include "../AudioObject.h"

namespace Yggdrasil {

class OscillatorObject : public AudioObject
{
	void  run(buf&, buf&, int) override;
		  
	float phase     = 0;
	float frequency = 100;

public:
	OscillatorObject(std::string);
};

}
