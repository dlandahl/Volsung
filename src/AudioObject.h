#pragma once

#include <vector>
#include <string>
#include <map>

#include "Yggdrasil.h"
#include "AudioDataflow.h"

namespace Yggdrasil {

struct linked_value
{
	float* parameter;
	float  default_value;
	int input;

	linked_value(float* p, float dv, int in):
		parameter(p), default_value(dv), input(in)
	{ }
};

class AudioObject
{
private:
	std::vector<CircularBuffer> in, out;
	std::vector<linked_value> linked_values;
	int index = 0;

protected:
	virtual void run(buf&, buf&, int) = 0;
	void set_io(int inputs, int outputs);
	void get_float_args(std::string, std::vector<float*>);
	void init(int, int, std::string, std::vector<float*>);

	void set_defval(float*, float, int);

	bool is_connected(uint);

public:
	std::string name;

	std::vector<AudioInput>  inputs;
	std::vector<AudioOutput> outputs;

	void implement();
	virtual void finish();

	CircularBuffer& get_out_buffer(uint buffer) { return out[buffer]; };

	AudioObject(int, int);
	AudioObject() = default;
	virtual ~AudioObject() = default;
};

}
