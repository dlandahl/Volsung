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
	AddObject(std::string);
};



class DelayObject : public AudioObject
{
	void run(buf&, buf&) override;
	float sample_delay = SAMPLE_RATE/5;

public:
	DelayObject(std::string);
};



class DriveObject : public AudioObject
{
	float pregain = 2;
	float postgain = 0.5;
	
	void run(buf&, buf&) override;

public:
	DriveObject(std::string);
};



class FileoutObject : public AudioObject
{
	void run(buf&, buf&) override;
	void finish() override;

	std::vector<float> data;
	std::string filename;
	
public:
	FileoutObject(std::string);
};



class FilterObject : public AudioObject
{
	float  frequency = 100;
	double a;
	double b;
	
	void run(buf&, buf&) override;

public:
	FilterObject(std::string);
};



class MultObject : public AudioObject
{
	float default_value = 1;
	void run(buf&, buf&) override;

public:
	MultObject(std::string);
};



class NoiseObject : public AudioObject
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
	
	void run(buf&, buf&) override;

public:
	NoiseObject(std::string);
};



class OscillatorObject : public AudioObject
{
	void  run(buf&, buf&) override;
		  
	float phase     = 0;
	float frequency = 100;

public:
	OscillatorObject(std::string);
};



class SquareObject : public AudioObject
{
	void  run(buf&, buf&) override;
	
	float pw        = 0.0;
	float phase     = 0;
	float frequency = 100;

public:
	SquareObject(std::string);
};



class UserObject : public AudioObject
{
	void run(buf&, buf&) override;
	friend void Program::create_user_object(std::string, uint, uint, std::any, callback_functor);

	callback_functor callback = nullptr;
	std::any user_data;
public:
    UserObject(std::string);
};



class AudioInputObject : public AudioObject
{
	void run(buf&, buf&) override;
	
public:
	std::vector<float> data;
	AudioInputObject(std::string);
};

class AudioOutputObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	std::vector<float> data;
	AudioOutputObject(std::string);
};






class ComparatorObject : public AudioObject
{
	void run(buf&, buf&) override;
	float value = 0.f;

public:
	ComparatorObject(std::string);
};



class TimerObject : public AudioObject
{
	void run(buf&, buf&) override;
	float value = 0;

public:
	TimerObject(std::string);
};



class ClockObject : public AudioObject
{
	void run(buf&, buf&) override;
	float interval = 1;
	float elapsed = 0;

public:
	ClockObject(std::string);
};



class DivisionObject : public AudioObject
{
	void run(buf&, buf&) override;
	float divisor = 0;

public:
	DivisionObject(std::string);
};



class SubtractionObject : public AudioObject
{
	void run(buf&, buf&) override;
	float subtrahend = 0;

public:
	SubtractionObject(std::string);
};



class ModuloObject : public AudioObject
{
	void run(buf&, buf&) override;
	float divisor = 0;

public:
	ModuloObject(std::string);
};



class AbsoluteValueObject : public AudioObject
{
	void run(buf&, buf&) override;

public:
	AbsoluteValueObject(std::string);
};
}
