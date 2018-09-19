#pragma once

#include "../AudioObject.h"

class CrushObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	
	float val               = 0;
	float samples_repeated  = 0;
	float samples_to_repeat = 16;
	float resolution        = 16;
	
public:
	CrushObject(std::string);
};
