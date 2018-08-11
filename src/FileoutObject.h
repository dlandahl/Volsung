#pragma once

#include "AudioObject.h"

//This object derives from AudioObject, and it stores 
//	data from its single input to a file.
class FileoutObject : public AudioObject
{
	void run() override;
public:
	FileoutObject(std::string);
};
