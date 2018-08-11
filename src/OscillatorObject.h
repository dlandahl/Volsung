#pragma once

#include <string>

#include "AudioObject.h"

//The OscillatorObject class derives from the AudioObject class...
//This means that it IS an AudioObject.
//Therefore it will already have the code to read and write block of samples inherited.

//We only need to implement the code to generate the sine wave and write it
//	to an output. This is the functionality that is unique to an oscillator,
//	thererfore it is not part of the base class AudioObject.
class OscillatorObject : public AudioObject
{
	void  run() override;
	Block make_block();
		  
	float phase;

	float frequency;
	uint  block_index;
public:
	OscillatorObject(std::string);
};
