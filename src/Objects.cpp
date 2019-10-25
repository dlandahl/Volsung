
#include <fstream>
#include <math.h>
#include <vector>

#include "StringFormat.h"
#include "Objects.h"

namespace Yggdrasil {

void AddObject::run(buf &in, buf &out, int index)
{
	out[0][index] = in[0][index] + default_value;
}

AddObject::AddObject(std::string arg)
{
	init(2, 1, arg, { &default_value });
	set_defval(&default_value, default_value, 1);
}



void CrushObject::run(buf &in, buf &out, int index)
{
	if(samples_to_repeat <= ++samples_repeated)
	{
		val = in[0][index];

		val *= resolution; val = (int)val;
		val = ((float)val) / resolution;

		samples_repeated = 0;
	}
	
	out[0][index] = val;
}

CrushObject::CrushObject(std::string args)
{
	init(3, 1, args, { &resolution, &samples_to_repeat });
	set_defval(&resolution, resolution, 1);
	set_defval(&samples_to_repeat, samples_to_repeat, 2);
}



void DelayObject::run(buf &in, buf &out, int index)
{
	out[0][index] = in[0][index - sample_delay];
}

DelayObject::DelayObject(std::string arg)
{
	init(2, 1, arg, { &sample_delay });
	set_defval(&sample_delay, sample_delay, 1);
	request_buffer_size(sample_delay + 1);
}



void DriveObject::run(buf& in, buf& out, int n)
{
	out[0][n] = tanh(pregain * in[0][n]) * postgain;
}

DriveObject::DriveObject(std::string args)
{
	init(3, 1, args, { &pregain, &postgain });
	set_defval(&pregain, pregain, 1);
	set_defval(&postgain, postgain, 2);
}



void FileoutObject::run(buf &in, buf &, int index)
{
	data.push_back(in[0][index]);
}

void FileoutObject::finish()
{
	std::fstream file(filename, std::fstream::out | std::fstream::binary);

	for (uint i = 0; i < data.size(); i++)
	{
		file.write((const char*)& data[i], sizeof(float));
	}

	file.close();
}

FileoutObject::FileoutObject(std::string filename) :
	filename(split_by(filename, ' ')[3])
{ set_io(1, 0); }



void FilterObject::run(buf& x, buf& y, int i)
{
	b = 2 - cos(TAU * frequency / SAMPLE_RATE);
	b = sqrt(b*b - 1) - b;
	a = 1 + b;

	y[0][i] = a*x[0][i] - b*y[0][i - 1];
}

FilterObject::FilterObject(std::string args)
{
	init(2, 1, args, { &frequency });
	set_defval(&frequency, frequency, 1);
}



void MultObject::run(buf &in, buf &out, int i)
{
	out[0][i] = in[0][i] * default_value;
}

MultObject::MultObject(std::string arg)
{
	init(2, 1, arg, { &default_value });
	set_defval(&default_value, default_value, 1);
}



void NoiseObject::run(buf &, buf &out, int index)
{
	out[0][index] = distribution(generator);
}

NoiseObject::NoiseObject(std::string) :
	distribution(-1.0f, 1.0f)
{ set_io(0, 1); }



void OscillatorObject::run(buf &, buf &out, int index)
{
	out[0][index] = sinf(TAU * phase);

	phase = phase + frequency / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

OscillatorObject::OscillatorObject(std::string args) :  phase(0)
{
	init(1, 1, args, {&frequency} );
	set_defval(&frequency, frequency, 0);
}



void SquareObject::run(buf &, buf &out, int index)
{	
	out[0][index] = (float)sign<float>(sinf(TAU * phase) + pw);

	phase = phase + frequency / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

SquareObject::SquareObject(std::string args)
{
	init(2, 1, args, { &frequency, &pw });
	set_defval(&frequency, frequency, 0);
	set_defval(&pw, pw, 1);
}



void UserObject::run(buf& in, buf& out, int index)
{
	if (callback)
		callback(in, out, index, user_data);
}

UserObject::UserObject(std::string args)
{
	float inputs = 0, outputs = 0;
	get_float_args(args, { &inputs, &outputs });
	set_io(inputs, outputs);
}



void AudioInputObject::run(buf& in, buf& out, int index)
{
	for (auto& output : out) {
		output[index] = data[index];
	}
}

AudioInputObject::AudioInputObject(std::string args)
{
	float outputs = 0;
	get_float_args(args, { &outputs });
	set_io(0, outputs);
}



void AudioOutputObject::run(buf& in, buf& out, int index)
{
	for (auto& input : in) data[index] = input[index];
}

AudioOutputObject::AudioOutputObject(std::string args)
{
	float inputs = 0;
	get_float_args(args, { &inputs });
	set_io(inputs, 0);
}


}

