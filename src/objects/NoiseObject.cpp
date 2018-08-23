
#include "NoiseObject.h"

void NoiseObject::run()
{
	Block b;

	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		b[i] = distribution(generator);
	}

	outputs[0].write_block(b);
}

NoiseObject::NoiseObject(std::string) : distribution(-1.0f, 1.0f)
{
	outputs.push_back({});
}