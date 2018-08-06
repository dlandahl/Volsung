
#include <string>
#include <iostream>

#include "Yggdrasil.h"
#include "Block.h"
#include "OscillatorObject.h"

Block OscillatorObject::make_block()
{
	Block b;

	if (inputs[0].connections.size())
	{
		Block p = read_block()[0];
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			b[i] = sinf((i + block_index*BLOCKSIZE) * (p[i]*220) * TAU / SAMPLE_RATE);
		}
	} else {
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			b[i] = sinf((i + block_index*BLOCKSIZE) * frequency);
		}
	}
	block_index++;
	return b;
}

void OscillatorObject::run()
{
	write_block(make_block(), 0);
}

OscillatorObject::OscillatorObject(std::string a) : block_index(0)
{
	outputs.push_back({});
	inputs.push_back({});
	frequency = std::stoi(a, nullptr, 10) * TAU / SAMPLE_RATE;
	std::cout << frequency << std::endl;
}