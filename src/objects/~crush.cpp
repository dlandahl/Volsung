
#include "~crush.h"

void CrushObject::run(buf &in, buf &out, int index)
{
	if (is_connected(1))
		resolution = in[1][index];

	if (is_connected(2))
		samples_to_repeat = in[2][index];

	if(samples_to_repeat <= ++samples_repeated)
	{
		val = in[0][index];

		val *= resolution; val = (int)val;
		val = ((float)val) / resolution;

		samples_repeated = 0;
	}
	
	out[0][index] = val;
}

CrushObject::CrushObject(std::string args)
{
	init(3, 1, args, { &resolution, &samples_to_repeat });
}
