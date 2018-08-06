#pragma once

#include "AudioObject.h"

class FileoutObject : public AudioObject
{
	void run() override;
public:
	FileoutObject(std::string);
};