#pragma once

#include "../AudioObject.h"

class MultObject : public AudioObject
{
	int default_value;

public:
	void run() override;
	MultObject(std::string);
};