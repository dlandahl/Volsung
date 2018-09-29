
#include <vector>

#include "~delay.h"

void DelayObject::run(buf &in, buf &out, int index)
{
	out[0][index] = in[1][index - sample_delay];
}

DelayObject::DelayObject(std::string arg)
{
	init(2, 1, arg, { &sample_delay });
	set_defval(&sample_delay, sample_delay, 1);
}
