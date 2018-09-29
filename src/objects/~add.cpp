
#include <vector>

#include "~add.h"

void AddObject::run(buf &in, buf &out, int index)
{
	out[0][index] = in[0][index] + default_value;
}

AddObject::AddObject(std::string arg)
{
	init(2, 1, arg, { &default_value });
	set_defval(&default_value, default_value, 1);
}
