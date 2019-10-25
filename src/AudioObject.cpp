
#include <vector>
#include <string>
#include <iostream>

#include "AudioObject.h"
#include "StringFormat.h"

namespace Yggdrasil {

void AudioObject::finish() { }

void AudioObject::implement()
{
	for (uint n = 0; n < inputs.size(); n++)
	{
		float value = inputs[n].read_value();
		in[n][index] = value;
	}

	for (auto const& value : linked_values)
		if (inputs[value.input].is_connected())
			*value.parameter = in[value.input][index];
	run(in, out, index);

	for (uint n = 0; n < outputs.size(); n++)
	{
		float value = out[n][index];

		outputs[n].write_value(value);
	}
	
	index++;
	if (index >= buffer_size) index -= buffer_size;
}

bool AudioObject::is_connected(uint in)
{
	return inputs[in].is_connected();
}

void AudioObject::init(int num_inputs, int num_outputs,
                       std::string args, std::vector<float*> float_members)
{
	set_io(num_inputs, num_outputs);
	get_float_args(args, float_members);
}

void AudioObject::get_float_args(std::string args, std::vector<float*> float_members)
{
	std::vector<std::string> sargl = split_by(args, ' ');
	name = sargl[2];
	
	for (uint i = 0; i < float_members.size() && i < sargl.size() - 3; i++)
	{
        float multiplier = 1.f;
        if (sargl[i + 3].back() == 's') {
            multiplier = SAMPLE_RATE;
            if (sargl[i + 3][sargl[i + 3].size() - 2] == 'm') multiplier *= 0.001;
        }
        
   		try         { *float_members[i] = float(std::stof(sargl[i+3], nullptr)) * multiplier; }
		catch (...) { std::cout << "Failed to initialize value on " << name << "\n"; }
	}
}

void AudioObject::set_io(int num_inputs, int num_outputs)
{
	outputs.resize(num_outputs);
	inputs.resize(num_inputs);

	out.resize(num_outputs);
	in.resize(num_inputs);
}

void AudioObject::request_buffer_size(int count)
{
	for (auto& buffer : in) buffer.resize_stream(count);
	for (auto& buffer : out) buffer.resize_stream(count);
	buffer_size = count;
}

void AudioObject::set_defval(float* parameter, float default_value, int input)
{
	linked_values.push_back(linked_value(parameter, default_value, input));
}

}
