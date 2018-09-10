#pragma once

#include <vector>
#include <string>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "Block.h"

class Object { };
class AudioObject : public Object
{
	std::vector<CircularBuffer> in, out;
	str name;

protected:
	virtual void run(buf&, buf&, int) = 0;
	void         set_io(int, int);
	void         get_float_args(str, std::vector<float*>);
	void         init(int, int, str, std::vector<float*>);

	bool         is_connected(uint);

public:
	static int index;

	std::vector<AudioInput>  inputs;
	std::vector<AudioOutput> outputs;

	void implement();
	virtual void finish();

	str get_name() { return name; }

	AudioObject(int, int);
	AudioObject() = default;
};
