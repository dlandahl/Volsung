#pragma once

#include <any>
#include <string>
#include <random>
#include <functional>
#include <queue>

#include "AudioObject.h"
#include "Graph.h"

namespace Yggdrasil {

class AddObject : public AudioObject
{
	void run(buf&, buf&) override;
	float default_value = 1;

public:
	AddObject(std::vector<std::string>);
};



class DelayObject : public AudioObject
{
	void run(buf&, buf&) override;
	float sample_delay = SAMPLE_RATE/5;

public:
	DelayObject(std::vector<std::string>);
};



class DriveObject : public AudioObject
{
	float pregain = 2;
	float postgain = 0.5;
	
	void run(buf&, buf&) override;

public:
	DriveObject(std::vector<std::string>);
};



class FileoutObject : public AudioObject
{
	void run(buf&, buf&) override;
	void finish() override;

	std::vector<float> data;
	std::string filename;
	
public:
	FileoutObject(std::vector<std::string>);
};



class FilterObject : public AudioObject
{
	float  frequency = 100;
	double a;
	double b;
	
	void run(buf&, buf&) override;

public:
	FilterObject(std::vector<std::string>);
};



class MultObject : public AudioObject
{
	float default_value = 1;
	void run(buf&, buf&) override;

public:
	MultObject(std::vector<std::string>);
};



class NoiseObject : public AudioObject
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
	
	void run(buf&, buf&) override;

public:
	NoiseObject(std::vector<std::string>);
};



class OscillatorObject : public AudioObject
{
	void  run(buf&, buf&) override;
		  
	float phase     = 0;
	float frequency = 100;

public:
	OscillatorObject(std::vector<std::string>);
};



class SquareObject : public AudioObject
{
	void  run(buf&, buf&) override;
	
	float pw        = 0.0;
	float phase     = 0;
	float frequency = 100;

public:
	SquareObject(std::vector<std::string>);
};



class UserObject : public AudioObject
{
	void run(buf&, buf&) override;
	friend void Program::create_user_object(std::string, uint, uint, std::any, callback_functor);

	callback_functor callback = nullptr;
	std::any user_data;
public:
    UserObject(std::vector<std::string>);
};



class AudioInputObject : public AudioObject
{
	void run(buf&, buf&) override;
	
public:
	std::vector<float> data;
	AudioInputObject(std::vector<std::string>);
};

class AudioOutputObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	std::vector<float> data;
	AudioOutputObject(std::vector<std::string>);
};






class ComparatorObject : public AudioObject
{
	void run(buf&, buf&) override;
	float value = 0.f;

public:
	ComparatorObject(std::vector<std::string>);
};



class TimerObject : public AudioObject
{
	void run(buf&, buf&) override;
	float value = 0;

public:
	TimerObject(std::vector<std::string>);
};



class ClockObject : public AudioObject
{
	void run(buf&, buf&) override;
	float interval = 1;
	float elapsed = 0;

public:
	ClockObject(std::vector<std::string>);
};



class DivisionObject : public AudioObject
{
	void run(buf&, buf&) override;
	float divisor = 0;

public:
	DivisionObject(std::vector<std::string>);
};



class SubtractionObject : public AudioObject
{
	void run(buf&, buf&) override;
	float subtrahend = 0;

public:
	SubtractionObject(std::vector<std::string>);
};



class ModuloObject : public AudioObject
{
	void run(buf&, buf&) override;
	float divisor = 0;

public:
	ModuloObject(std::vector<std::string>);
};



class AbsoluteValueObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	AbsoluteValueObject(std::vector<std::string>);
};


class BiquadObject : public AudioObject
{
protected:
	float alpha;
	float cos_omega;

	virtual void calculate_coeffs() = 0;
	float filter(float);
};

}
