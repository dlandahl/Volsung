
#include <vector>
#include <string>
#include <iostream>

#include "AudioObject.h"
#include "StringFormat.h"

namespace Yggdrasil {

void AudioObject::finish() { }

void AudioObject::implement()
{
	for (uint i = 0; i < inputs.size(); i++)
	{
		Block b = inputs[i].read_block();
		for (uint j = 0; j < BLOCKSIZE; j++)
			in[i][j + index] = b[j];
	}

	for (uint i = 0; i < BLOCKSIZE; i++)
	{
		for (auto const& value : linked_values)
			if (inputs[value.input].is_connected())
				*value.parameter = in[value.input][index + i];

		run(in, out, index + i);
	}

	for (uint i = 0; i < outputs.size(); i++)
	{
		Block b;
		for (uint j = 0; j < BLOCKSIZE; j++)
			b[j] = out[i][index + j];

		outputs[i].write_block(b);
	}
	index += BLOCKSIZE;
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
   		try         { *float_members[i] = float(std::stof(sargl[i+3], nullptr)); }
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

void AudioObject::set_defval(float* parameter, float default_value, int input)
{
	linked_values.push_back(linked_value(parameter, default_value, input));
}

}
