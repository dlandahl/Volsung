#pragma once

#include "../AudioObject.h"

class MultObject : public AudioObject
{
	float default_value;
	void run(buf&, buf&, int) override;

public:
	MultObject(std::string);
};
