
#pragma once

#include <vector>
#include <string>
#include <map>

#include "Volsung.h"
#include "AudioDataflow.h"

namespace Volsung {

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
	int buffer_size = 1;
	
protected:
	virtual void run(buf&, buf&) = 0;
	void set_io(int inputs, int outputs);
	void get_float_args(std::vector<std::string>, std::vector<float*>);
	void init(int, int, std::vector<std::string>, std::vector<float*>);
	void request_buffer_size(int requested_size);

	void set_defval(float*, float, int);
	bool is_gate_high(uint);
	bool gate_opened(uint);
	bool gate_closed(uint);
	bool is_connected(uint);

public:
	static constexpr float gate_threshold = .75f;


	std::vector<AudioInput>  inputs;
	std::vector<AudioOutput> outputs;

	void implement();
	virtual void finish();

	AudioObject(int, int);
	AudioObject() = default;
	virtual ~AudioObject() = default;
};

}
