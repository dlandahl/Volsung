
#include "~noise.h"

void NoiseObject::run(buf &, buf &out, int index)
{
	out[0][index] = distribution(generator);
}

NoiseObject::NoiseObject(std::string) :
	distribution(-1.0f, 1.0f)
{ set_io(0, 1); }
