
#include <fstream>
#include <vector>
#include <cmath>

#include "Objects.hh"

namespace Volsung {

void AddObject::run(buf &in, buf &out)
{
	out[0][0] = in[0][0] + default_value;
}

AddObject::AddObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &default_value });
	set_defval(&default_value, default_value, 1);
}



void DelayObject::run(buf &in, buf &out)
{
	out[0][0] = in[0][-sample_delay];
}

DelayObject::DelayObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &sample_delay });
	set_defval(&sample_delay, sample_delay, 1);
	request_buffer_size(sample_delay + 1);
}



void DriveObject::run(buf& in, buf& out)
{
	out[0][0] = tanh(pregain * in[0][0]) * postgain;
}

DriveObject::DriveObject(std::vector<TypedValue> args)
{
	init(3, 1, args, { &pregain, &postgain });
	set_defval(&pregain, pregain, 1);
	set_defval(&postgain, postgain, 2);
}



void FileoutObject::run(buf &in, buf &)
{
	data.push_back(in[0][0]);
}

void FileoutObject::finish()
{
	std::fstream file(filename, std::fstream::out | std::fstream::binary);

	for (uint n = 0; n < data.size(); n++)
	{
		file.write((const char*)& data[n], sizeof(float));
	}

	file.close();
}

FileoutObject::FileoutObject(std::vector<TypedValue> args)
{
	filename = std::get<std::string>(args[0]);
	set_io(1, 0);
}



void FilterObject::run(buf& x, buf& y)
{
	b = 2 - cos(TAU * frequency / SAMPLE_RATE);
	b = sqrt(b*b - 1) - b;
	a = 1 + b;

	y[0][0] = a*x[0][0] - b*y[0][-1];
}

FilterObject::FilterObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &frequency });
	set_defval(&frequency, frequency, 1);
}



void MultObject::run(buf &in, buf &out)
{
	out[0][0] = in[0][0] * default_value;
}

MultObject::MultObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &default_value });
	set_defval(&default_value, default_value, 1);
}



void NoiseObject::run(buf &, buf &out)
{
	out[0][0] = distribution(generator);
}

NoiseObject::NoiseObject(std::vector<TypedValue>) :
	distribution(-1.0f, 1.0f)
{ set_io(0, 1); }



void OscillatorObject::run(buf &, buf &out)
{
	out[0][0] = sinf(TAU * phase);

	phase = phase + frequency / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

OscillatorObject::OscillatorObject(std::vector<TypedValue> args) :  phase(0)
{
	init(1, 1, args, {&frequency} );
	set_defval(&frequency, frequency, 0);
}



void SquareObject::run(buf &, buf &out)
{	
	out[0][0] = (float)sign<float>(sinf(TAU * phase) + pw);

	phase = phase + frequency / SAMPLE_RATE;

	if (phase >= 1.0) { phase -= 1.0; }
}

SquareObject::SquareObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &frequency, &pw });
	set_defval(&frequency, frequency, 0);
	set_defval(&pw, pw, 1);
}



void UserObject::run(buf& in, buf& out)
{
	if (callback)
		callback(in, out, user_data);
}

UserObject::UserObject(std::vector<TypedValue> args)
{
	float inputs = 0, outputs = 0;
	inputs  = std::get<float>(args[0]);
	outputs = std::get<float>(args[1]);
	set_io(inputs, outputs);
}



void AudioInputObject::run(buf& in, buf& out)
{
	for (auto& output : out) {
		output[0] = data[0];
	}
}

AudioInputObject::AudioInputObject(std::vector<TypedValue> args)
{
	float outputs = 0;
	outputs = std::get<float>(args[0]);
	set_io(0, outputs);
	data.resize(outputs);
}



void AudioOutputObject::run(buf& in, buf& out)
{
	for (auto& input : in) data[0] = input[0];
}

AudioOutputObject::AudioOutputObject(std::vector<TypedValue> args)
{
	float inputs = 0;
	inputs = std::get<float>(args[0]);
	set_io(inputs, 0);
	data.resize(inputs);
}



void ComparatorObject::run(buf &in, buf &out)
{
	if (in[0][0] > value) out[0][0] = 1.f;
	else out[0][0] = 0.f;
}

ComparatorObject::ComparatorObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &value });
	set_defval(&value, value, 1);
}


void TimerObject::run(buf&, buf &out)
{
	out[0][0] = value;
	value += 1.f / SAMPLE_RATE;
	if (gate_opened(0)) value = 0.f;
}

TimerObject::TimerObject(std::vector<TypedValue>)
{
	set_io(1, 1);
}


void ClockObject::run(buf&, buf &out)
{
	out[0][0] = 0;
	if (elapsed >= interval) {
		out[0][0] = 1;
		elapsed = 0.f;
	}
	
	elapsed += 1.f;
}

ClockObject::ClockObject(std::vector<TypedValue> args)
{
	init(1, 1, args, { &interval });
	set_defval(&interval, interval, 0);
}


void DivisionObject::run(buf &in, buf &out)
{
	out[0][0] = in[0][0] / divisor;
}

DivisionObject::DivisionObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &divisor });
	set_defval(&divisor, divisor, 1);
}


void SubtractionObject::run(buf &in, buf &out)
{
	out[0][0] = in[0][0] - subtrahend;
}

SubtractionObject::SubtractionObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &subtrahend });
	set_defval(&subtrahend, subtrahend, 1);
}


void ModuloObject::run(buf &in, buf &out)
{
	out[0][0] = std::fmod(in[0][0], divisor);
}

ModuloObject::ModuloObject(std::vector<TypedValue> args)
{
	init(2, 1, args, { &divisor });
	set_defval(&divisor, divisor, 1);
}


void AbsoluteValueObject::run(buf &in, buf &out)
{
	out[0][0] = std::fabs(in[0][0]);
}

AbsoluteValueObject::AbsoluteValueObject(std::vector<TypedValue>)
{
	set_io(1, 1);
}



}

