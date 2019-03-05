#pragma once

#include "../AudioObject.h"

namespace Yggdrasil {

class DriveObject : public AudioObject
{
	float pregain = 2;
	float postgain = 0.5;
	
	void run(buf&, buf&, int) override;

public:
	DriveObject(std::string);
};

}
