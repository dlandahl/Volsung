
#include <math.h>

#include "FilterObject.h"

void FilterObject::run()
{
	std::vector<Block> x = read_block();
	Block y;

	b = 2 - cosf(TAU * x[1][0] / SAMPLE_RATE);
	b = sqrt(b*b - 1) - b;
	a = 1 + b;

	y[0] = a*x[0][0] - b*last_value;

	for (uint i = 1; i < BLOCKSIZE; i++)
	{
		b = 2 - cosf(TAU * x[1][i] / SAMPLE_RATE);
		b = sqrt(b*b - 1) - b;
		a = 1 + b;

		y[i] = a*x[0][i] - b*y[i - 1];
	}

	last_value = y[BLOCKSIZE - 1];

	outputs[0].write_block(y);
}

FilterObject::FilterObject(std::string) : last_value(0)
{
	outputs.push_back({});
	inputs.push_back({});
	inputs.push_back({});
}