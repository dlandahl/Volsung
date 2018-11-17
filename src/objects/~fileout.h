#pragma once

#include "../AudioObject.h"

namespace Yggdrasil {

class FileoutObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	void finish() override;

	std::vector<float> data;
	std::string filename;
	
public:
	FileoutObject(std::string);
};

}