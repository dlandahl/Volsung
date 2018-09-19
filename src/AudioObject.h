#pragma once

#include <vector>
#include <string>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "Block.h"

class AudioObject
{
	std::vector<CircularBuffer> in, out;
	std::string name;

protected:
	virtual void run(buf&, buf&, int) = 0;
	void         set_io(int, int);
	void         get_float_args(std::string, std::vector<float*>);
	void         init(int, int, std::string, std::vector<float*>);

	bool         is_connected(uint);

public:
	static int index;

	std::vector<AudioInput>  inputs;
	std::vector<AudioOutput> outputs;

	void implement();
	virtual void finish();

	std::string get_name() { return name; }

	AudioObject(int, int);
	AudioObject() = default;
};
