#pragma once

#include "AudioObject.h"

class OscillatorObject : public AudioObject
{
	void run() override;
	Block make_block();
public:
	OscillatorObject();
};