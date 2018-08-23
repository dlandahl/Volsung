#pragma once

#include "../AudioObject.h"

class FilterObject : public AudioObject
{
	float last_value;
	float next_last_value;
	float a;
	float b;
	float c;

public:
	void run() override;
	FilterObject(std::string);
};