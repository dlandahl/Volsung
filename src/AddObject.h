#pragma once

#include "AudioObject.h"

class AddObject : public AudioObject
{
	void run()    override;
	void finish() override;

	std::string filename;
public:
	AddObject(std::string);
};