
#include <math.h>
#include <vector>

#include "~filter.h"

void FilterObject::run(buf& x, buf& y, int i)
{
	if (is_connected(0))
		frequency = x[1][i];

	b = 2 - cos(TAU * frequency / SAMPLE_RATE);
	b = sqrt(b*b - 1) - b;
	a = 1 + b;

	y[0][i] = a*x[0][i] - b*y[0][i - 1];
}

FilterObject::FilterObject(std::string args)
{
	init(2, 1, args, { &frequency });
}
