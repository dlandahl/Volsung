
#include <vector>

#include "~add.h"

void AddObject::run(buf &in, buf &out, int index)
{
	if (is_connected(1))
		out[0][index] = in[0][index] + in[1][index];
	else
		out[0][index] = in[0][index] + default_value;
}

AddObject::AddObject(std::string arg)
{
	init(2, 1, arg, { &default_value });
}
