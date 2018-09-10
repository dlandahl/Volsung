#pragma once

#include "../AudioObject.h"

class CrushObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	
	float val               = 0;
	float samples_repeated  = 0;
	float samples_to_repeat = 0;
	float resolution        = 0;
	
public:
	CrushObject(std::string);
};
