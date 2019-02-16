#pragma once

#include "../AudioObject.h"

namespace Yggdrasil {

class AddObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	float default_value = 1;

public:
	AddObject(std::string);
};

}
