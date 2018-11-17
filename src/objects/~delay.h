#pragma once

#include "../AudioObject.h"

namespace Yggdrasil {

class DelayObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	float sample_delay = SAMPLE_RATE/5;

public:
	DelayObject(std::string);
};

}