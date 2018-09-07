
#include <vector>

#include "~mult.h"

void MultObject::run(buf &in, buf &out, int i)
{
	if (is_connected(1))
		out[0][i] = in[0][i] * in[1][i];
	else
		out[0][i] = in[0][i] * default_value;
}

MultObject::MultObject(std::string arg)
{
	init(2, 1, arg, { &default_value });
}
