
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
			b[i] = sinf(TAU * phase);
			phase = phase + (p[i] + 1) * 220 / SAMPLE_RATE;;
			if (phase >= 1.0) { phase -= 1.0; }
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

//Everytime the run function is called on this AudioObject, it should write a block
//	to its 0th output using the make_block function defined above.
void OscillatorObject::run()
{
	write_block(make_block(), 0);
}

OscillatorObject::OscillatorObject(std::string a) : block_index(0), phase(0)
{
	//Create an output and an input.
	//The input is for pitch modulation, the output is to output the sine wave signal.
	outputs.push_back({});
	inputs.push_back({});
	
	//This reads the argument that is passed in and sets the freqency based on the argument.
	frequency = std::stoi(a, nullptr, 10) * TAU / SAMPLE_RATE;
}
