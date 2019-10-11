#pragma once

#include "AudioObject.h"
#include <string>
#include <random>
#include <functional>
#include <any>

namespace Yggdrasil {

class AddObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	float default_value = 1;

public:
	AddObject(std::string);
};



class CrushObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	
	float val               = 0;
	float samples_repeated  = 0;
	float samples_to_repeat = 16;
	float resolution        = 16;
	
public:
	CrushObject(std::string);
};



class DelayObject : public AudioObject
{
	void run(buf&, buf&, int) override;
	float sample_delay = SAMPLE_RATE/5;

public:
	DelayObject(std::string);
};



class DriveObject : public AudioObject
{
	float pregain = 2;
	float postgain = 0.5;
	
	void run(buf&, buf&, int) override;

public:
	DriveObject(std::string);
};



class FileoutObject : public AudioObject
{
	void run(buf&, buf&, int) override;
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
	
	void run(buf&, buf&, int) override;

public:
	FilterObject(std::string);
};



class MultObject : public AudioObject
{
	float default_value = 1;
	void run(buf&, buf&, int) override;

public:
	MultObject(std::string);
};



class NoiseObject : public AudioObject
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
	
	void run(buf&, buf&, int) override;

public:
	NoiseObject(std::string);
};



class OscillatorObject : public AudioObject
{
	void  run(buf&, buf&, int) override;
		  
	float phase     = 0;
	float frequency = 100;

public:
	OscillatorObject(std::string);
};




class SquareObject : public AudioObject
{
	void  run(buf&, buf&, int) override;
	
	float pw        = 0.0;
	float phase     = 0;
	float frequency = 100;

public:
	SquareObject(std::string);
};



using callback_functor = std::function<void(buf&, buf&, int, std::any)>;

class UserObject : public AudioObject
{
	void run(buf&, buf&, int) override;

public:
	callback_functor callback = nullptr;
	std::any user_data;
    UserObject(std::string);
};

}
