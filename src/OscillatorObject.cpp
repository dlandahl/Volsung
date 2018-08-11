
#include <string>
#include <iostream>

#include "Yggdrasil.h"
#include "Block.h"
#include "OscillatorObject.h"

//The OscillatorObject IS an AudioObject which generates a sinewave and writes it
//	to an AudioOutput.

Block OscillatorObject::make_block()
{
	//We want to create a Block in order to write a sine wave to it.
	//A Block is just a list of samples as floats.
	Block b;

	//If there is an AudioConnector to the input we create in the constructor below, we
	//	will want to use the frequency from this input as the pitch.
	if (inputs[0].connections.size())
	{
		//Read a block and write a sinewave of the frequency described in the block
		//	to the block we created earlier.
		Block p = read_block()[0];
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			b[i] = sinf(TAU * phase);
			phase = phase + (p[i] + 1) * 220 / SAMPLE_RATE;;
			if (phase >= 1.0) { phase -= 1.0; }
		}
	//If there is no AudioConnector connected to the input:
	} else {
		for (uint i = 0; i < BLOCKSIZE; i++)
		{
			//Just fill the block with a sinewave at the frequency that is passed in as argument.
			b[i] = sinf((i + block_index*BLOCKSIZE) * frequency);
		}
	}
	block_index++;
	
	//Return the block.
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
