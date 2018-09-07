
#include "~noise.h"

void NoiseObject::run(buf &in, buf &out, int index)
{
	out[0][index] = distribution(generator);
}

NoiseObject::NoiseObject(std::string) :
	AudioObject(0, 1), distribution(-1.0f, 1.0f)
{ }
