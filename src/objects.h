#pragma once

#include "Rumpetroll.h"
#include "AudioObject.h"
#include "Block.h"

class FileoutObject : public AudioObject
{
public:
	void run();
};

class OscillatorObject : public AudioObject
{
	Block make_block();
public:
	void run();
};

class SawtoothOscillatorObject : public AudioObject
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

class FilterObject : public AudioObject
{
	float last;
public:
	FilterObject();
	void run();
};