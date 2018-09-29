
#include <vector>

#include "~mult.h"

void MultObject::run(buf &in, buf &out, int i)
{
	out[0][i] = in[0][i] * default_value;
}

MultObject::MultObject(std::string arg)
{
	init(2, 1, arg, { &default_value });
	set_defval(&default_value, default_value, 1);
}
