
#include <string>
#include <math.h>
#include <iostream>

#include "~square.h"

void SquareObject::run(buf &in, buf &out, int index)
{
	if (is_connected(0))
		frequency = in[0][index];
	
	if (is_connected(1))
		pw = in[1][index];
	
	out[0][index] = (float)sign<float>(sinf(TAU * phase) + pw);

	phase = phase + abs(frequency) / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

SquareObject::SquareObject(str args)
{
	init(2, 1, args, { &frequency, &pw });
}
