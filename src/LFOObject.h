#pragma once

#include "AudioObject.h"

class LFOObject : public AudioObject
{
	void run() override;
	Block make_block();
public:
	LFOObject();
};