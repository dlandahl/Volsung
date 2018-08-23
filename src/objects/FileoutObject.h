#pragma once

#include "../AudioObject.h"

class FileoutObject : public AudioObject
{
	void run()    override;
	void finish() override;

	std::string filename;
public:
	FileoutObject(std::string);
};