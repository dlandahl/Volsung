
#pragma once

#include <vector>
#include <string>
#include <map>

#include "Volsung.hh"
#include "AudioDataflow.hh"

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
	void request_buffer_size(int);
	bool verify_argument_types(std::vector<Type>, std::vector<TypedValue>);
	void set_io(int, int);
	void init(int, int, std::vector<TypedValue>, std::vector<float*>);
	void set_defval(float*, float, int);

	bool is_gate_high(uint);
	bool gate_opened(uint);
	bool gate_closed(uint);
	bool is_connected(uint);

public:
	void implement();
	static constexpr float gate_threshold = .75f;
	
	std::vector<AudioInput>  inputs;
	std::vector<AudioOutput> outputs;
	AudioObject() = default;

	virtual void finish();
	virtual ~AudioObject() = default;
};

}
