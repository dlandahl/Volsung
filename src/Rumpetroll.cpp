
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "objects.h"
#include "Rumpetroll.h"
#include "Block.h"
#include "AudioDataflow.h"
#include "AudioObject.h"

int main()
{
	OscillatorObject osc;
	PassObject pass;
	FileoutObject out;

	AudioConnector con1;
	AudioConnector con2;

	AudioConnector* con1ptr = &con1;
	AudioConnector* con2ptr = &con2;

	osc.output.connection = con1ptr;
	pass.input.connection = con1ptr;
	con1.out = &osc.output;

	pass.output.connection = con2ptr;
	out.input.connection = con2ptr;
	con2.out = &pass.output;

	for (uint i = 0; i < 1000; i++)
	{
		osc.run();
		pass.run();
		out.run();
	}

	std::vector<float> out_data;
	out_data = out.data;

	std::fstream file("audio.raw", std::fstream::out | std::fstream::binary);

	for (uint i = 0; i < out_data.size(); i++) {
		file.write((const char*)&out_data[i], sizeof(float));
	}

	file.close();
}