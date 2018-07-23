#pragma once

#include "Rumpetroll.h"
#include "AudioObject.h"
#include "Block.h"

class FileoutObject : public AudioObject
{
public:
	std::vector<float> data;

	void run();
};

class OscillatorObject : public AudioObject
{
	Block make_block();

public:
	void run();
};

class PassObject : public AudioObject
{
public:
	void run();
};