#pragma once

#include "../AudioObject.h"

class DelayObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	float sample_delay = 44100/5;

public:
	DelayObject(std::string);
};
