#pragma once

#include <any>
#include <string>
#include <random>
#include <functional>
#include <queue>

#include "AudioObject.hh"
#include "Graph.hh"

namespace Volsung {

class AddObject : public AudioObject
{
	void run(buf&, buf&) override;
	float default_value = 1;

public:
	AddObject(std::vector<TypedValue>);
};



class DelayObject : public AudioObject
{
	void run(buf&, buf&) override;
	float sample_delay = sample_rate/5;

public:
	DelayObject(std::vector<TypedValue>);
};



class DriveObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	DriveObject(std::vector<TypedValue>);
};



class FileoutObject : public AudioObject
{
	void run(buf&, buf&) override;
	void finish() override;

	std::vector<float> in_data;
	std::vector<float> out_data;
	std::string filename;
	int pos = 0;
public:
	FileoutObject(std::vector<TypedValue>);
};



class FilterObject : public AudioObject
{
	float  frequency = 100;
	double a;
	double b;
	
	void run(buf&, buf&) override;

public:
	FilterObject(std::vector<TypedValue>);
};



class MultObject : public AudioObject
{
	float default_value = 1;
	void run(buf&, buf&) override;

public:
	MultObject(std::vector<TypedValue>);
};



class NoiseObject : public AudioObject
{
    std::uniform_real_distribution<float> distribution;
	std::default_random_engine generator;
		
	void run(buf&, buf&) override;

public:
	NoiseObject(std::vector<TypedValue>);
};



class OscillatorObject : public AudioObject
{
	void  run(buf&, buf&) override;
		  
	float phase     = 0;
	float frequency = 100;

public:
	OscillatorObject(std::vector<TypedValue>);
};



class SquareObject : public AudioObject
{
	void  run(buf&, buf&) override;
	
	float pw        = 0.0;
	float phase     = 0;
	float frequency = 100;

public:
	SquareObject(std::vector<TypedValue>);
};



class UserObject : public AudioObject
{
	void run(buf&, buf&) override;
	friend void Program::create_user_object(std::string, uint, uint, std::any, callback_functor);

	callback_functor callback = nullptr;
	std::any user_data;
public:
    UserObject(std::vector<TypedValue>);
};



class AudioInputObject : public AudioObject
{
	void run(buf&, buf&) override;
	
public:
	std::vector<float> data;
	AudioInputObject(std::vector<TypedValue>);
};

class AudioOutputObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	std::vector<float> data;
	AudioOutputObject(std::vector<TypedValue>);
};






class ComparatorObject : public AudioObject
{
	void run(buf&, buf&) override;
	float value = 0.f;

public:
	ComparatorObject(std::vector<TypedValue>);
};



class TimerObject : public AudioObject
{
	void run(buf&, buf&) override;
	float value = 0;

public:
	TimerObject(std::vector<TypedValue>);
};



class ClockObject : public AudioObject
{
	void run(buf&, buf&) override;
	float interval = 1;
	float elapsed = 0;

public:
	ClockObject(std::vector<TypedValue>);
};



class DivisionObject : public AudioObject
{
	void run(buf&, buf&) override;
	float divisor = 0;

public:
	DivisionObject(std::vector<TypedValue>);
};



class SubtractionObject : public AudioObject
{
	void run(buf&, buf&) override;
	float subtrahend = 0;

public:
	SubtractionObject(std::vector<TypedValue>);
};



class ModuloObject : public AudioObject
{
	void run(buf&, buf&) override;
	float divisor = 0;

public:
	ModuloObject(std::vector<TypedValue>);
};



class AbsoluteValueObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	AbsoluteValueObject(std::vector<TypedValue>);
};


class StepSequence : public AudioObject
{
	Sequence sequence;
	int current = 0;

	void run(buf&, buf&) override;
public:
	StepSequence(std::vector<TypedValue>);
};



class PowerObject : public AudioObject
{
	float exponent = 2;

	void run(buf&, buf&) override;
public:
	PowerObject(std::vector<TypedValue>);
};



class EnvelopeObject : public AudioObject
{
	float length = 1000;
	float start = 1.f;
	float end = 0.f;
	int time = 0;

	void run(buf&, buf&) override;
public:
	EnvelopeObject(std::vector<TypedValue>);
};


class RoundObject : public AudioObject
{
	void run(buf&, buf&) override;
public:
	RoundObject(std::vector<TypedValue>);
};


class SequenceObject : public AudioObject
{
	Sequence sequence;

	void run(buf&, buf&) override;
public:
	SequenceObject(std::vector<TypedValue>);
};


class SampleAndHoldObject : public AudioObject
{
	float value = 0;

	void run(buf&, buf&) override;
public:
	SampleAndHoldObject(std::vector<TypedValue>);
};



class ConstObject : public AudioObject
{
	float value = 0;

	void run(buf&, buf&) override;
public:
	ConstObject(std::vector<TypedValue>);
};

class SawObject : public AudioObject
{
	float phase = -1;
	float frequency;

	void run(buf&, buf&) override;
public:
	SawObject(std::vector<TypedValue>);
};

class TriangleObject : public AudioObject
{
	float phase = 0;
	float frequency;

	void run(buf&, buf&) override;

public:
	TriangleObject(std::vector<TypedValue>);
};

class BiquadObject : public AudioObject
{
	void run(buf&, buf&) override;
protected:
	float a0, a1, a2, b0, b1, b2;

	float frequency;
	float resonance = 1.f;

	double omega;
	double alpha;
	double cos_omega;
	double A;

	virtual void calculate_coefficients() = 0;

public:
	BiquadObject(std::vector<TypedValue>);
};

class LowpassObject : public BiquadObject
{
	void calculate_coefficients() override;
public:
	LowpassObject(std::vector<TypedValue> arguments) :
		BiquadObject(arguments) { }
};

class HighpassObject : public BiquadObject
{
	void calculate_coefficients() override;
public:
	HighpassObject(std::vector<TypedValue> arguments) :
		BiquadObject(arguments) { }
};

class BandpassObject : public BiquadObject
{
	void calculate_coefficients() override;
public:
	BandpassObject(std::vector<TypedValue> arguments) :
		BiquadObject(arguments) { }
};


class EnvelopeFollowerObject : public AudioObject
{
	static float constexpr time_constant = -0.99967234f;

	float attack = 441;
	float release = 441;
	float last_value = 0.f;

	void run(buf&, buf&) override;

public:
	EnvelopeFollowerObject(std::vector<TypedValue>);
};

}
