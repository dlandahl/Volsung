#pragma once

#include "../AudioObject.h"

class MultObject : public AudioObject
{
	float default_value;

public:
	void run(buf&, buf&, int) override;
	MultObject(std::string);
};
