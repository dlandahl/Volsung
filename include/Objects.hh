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
	float sample_delay = SAMPLE_RATE/5;

public:
	DelayObject(std::vector<TypedValue>);
};



class DriveObject : public AudioObject
{
	float pregain = 2;
	
	void run(buf&, buf&) override;

public:
	DriveObject(std::vector<TypedValue>);
};



class FileoutObject : public AudioObject
{
	void run(buf&, buf&) override;
	void finish() override;

	std::vector<float> data;
	std::string filename;
	
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
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
	
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


class BiquadObject : public AudioObject
{
protected:
	float alpha;
	float cos_omega;

	virtual void calculate_coeffs() = 0;
	float filter(float);
};

class StepSequence : public AudioObject
{
	Sequence sequence;
	int current = 0;

	void run(buf&, buf&) override;
public:
	StepSequence(std::vector<TypedValue>);
};

}
