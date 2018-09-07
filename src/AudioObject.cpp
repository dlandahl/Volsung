
#include <vector>
#include <string>
#include <iostream>

#include "AudioObject.h"
#include "StringFormat.h"

void AudioObject::finish() { }

int AudioObject::index = 0;

void AudioObject::implement()
{
	for (uint i = 0; i < inputs.size(); i++)
	{
		Block b = inputs[i].read_block();
		for (uint j = 0; j < BLOCKSIZE; j++)
			in[i][j + AudioObject::index] = b[j];
	}

	for (uint i = 0; i < BLOCKSIZE; i++)
		run(in, out, AudioObject::index + i);

	for (uint i = 0; i < outputs.size(); i++)
	{
		Block b;
		for (uint j = 0; j < BLOCKSIZE; j++)
			b[j] = out[i][AudioObject::index + j];

		outputs[i].write_block(b);
	}
}

bool AudioObject::is_connected(uint in)
{
	return inputs[in].is_connected();
}

bool AudioObject::init(int num_inputs, int num_outputs,
						 str args, std::vector<float*> float_members)
{
	inputs.resize(num_inputs);
	outputs.resize(num_outputs);

	in.resize(num_inputs);
	out.resize(num_outputs);

	std::vector<str> sargl = sf::split_by(args, ' ');
	name = sargl[2];
	
	for (uint i = 0; i < float_members.size(); i++)
	{
   		try         { *float_members[i] = float(std::stof(sargl[i+3], nullptr)); }
		catch (...) { std::cout << "Failed to initialize value on " << name << "\n"; }
	}
	
	return true;
}

AudioObject::AudioObject(int num_inputs, int num_outputs)
{
	outputs.resize(num_outputs);
	inputs.resize(num_inputs);

	out.resize(num_outputs);
	in.resize(num_inputs);
}
