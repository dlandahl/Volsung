#pragma once

#include <random>
#include <functional>

#include "../AudioObject.h"

class NoiseObject : public AudioObject
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;

public:
	void run(buf&, buf&, int) override;
	NoiseObject(std::string);
};
