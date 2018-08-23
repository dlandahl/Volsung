#pragma once

#include "../AudioObject.h"

class AddObject : public AudioObject
{
	void run() override;
	int default_value;

public:
	AddObject(std::string);
};