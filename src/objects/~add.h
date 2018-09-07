#pragma once

#include "../AudioObject.h"

class AddObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	float default_value;

public:
	AddObject(std::string);
};
