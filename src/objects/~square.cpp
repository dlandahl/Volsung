
#include <string>
#include <math.h>
#include <iostream>

#include "~square.h"

void SquareObject::run(buf &, buf &out, int index)
{	
	out[0][index] = (float)sign<float>(sinf(TAU * phase) + pw);

	phase = phase + frequency / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

SquareObject::SquareObject(std::string args)
{
	init(2, 1, args, { &frequency, &pw });
	set_defval(&frequency, frequency, 0);
	set_defval(&pw, pw, 1);
}
