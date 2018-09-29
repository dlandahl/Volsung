
#include <string>
#include <math.h>

#include "~oscillator.h"

void OscillatorObject::run(buf &in, buf &out, int index)
{
	out[0][index] = sinf(TAU * phase);

	phase = phase + frequency / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

OscillatorObject::OscillatorObject(std::string args) :  phase(0)
{
	init(1, 1, args, {&frequency} );
	set_defval(&frequency, frequency, 0);
}
