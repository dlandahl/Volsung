
#include <math.h>
#include <vector>

#include "~drive.h"

namespace Yggdrasil {

void DriveObject::run(buf& in, buf& out, int n)
{
	out[0][n] = tanh(pregain * in[0][n]) * postgain;
}

DriveObject::DriveObject(std::string args)
{
	init(3, 1, args, { &pregain, &postgain });
	set_defval(&pregain, pregain, 1);
	set_defval(&postgain, postgain, 2);
}

}
