#pragma once

#include "AudioObject.h"

//This object derives from AudioObject, and it stores data to a file
class FileoutObject : public AudioObject
{
	void run() override;
public:
	FileoutObject(std::string);
};
