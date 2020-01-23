
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
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float default_value = 1;

public:
    AddObject(const std::vector<TypedValue>&);
};



class DelayObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float sample_delay = sample_rate;

public:
    DelayObject(const std::vector<TypedValue>&);
};



class DriveObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    DriveObject(const std::vector<TypedValue>&);
};



class FileoutObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    void finish() override;

    std::vector<float> in_data;
    std::vector<float> out_data;
    std::string filename;
    std::size_t pos = 0;
public:
    FileoutObject(const std::vector<TypedValue>&);
};



class FilterObject : public AudioObject
{
    float  frequency = 100;
    double a;
    double b;
    
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    FilterObject(const std::vector<TypedValue>&);
};



class MultObject : public AudioObject
{
    float default_value = 1;
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    MultObject(const std::vector<TypedValue>&);
};



class NoiseObject : public AudioObject
{
    std::uniform_real_distribution<float> distribution;
    std::default_random_engine generator;
        
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    NoiseObject(const std::vector<TypedValue>&);
};



class OscillatorObject : public AudioObject
{
    void  process(const MultichannelBuffer&, MultichannelBuffer&) override;
          
    float phase     = 0;
    float frequency = 100;

public:
    OscillatorObject(const std::vector<TypedValue>&);
};



class SquareObject : public AudioObject
{
    void  process(const MultichannelBuffer&, MultichannelBuffer&) override;
    
    float pw        = 0.0;
    float phase     = 0;
    float frequency = 100;

public:
    SquareObject(const std::vector<TypedValue>&);
};



class UserObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

    const CallbackFunctor callback;
    std::any user_data;
public:
    UserObject(const std::vector<TypedValue>&, const CallbackFunctor, std::any);
};



class AudioInputObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    
public:
    Frame data;
    AudioInputObject(const std::vector<TypedValue>&);
};

class AudioOutputObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    Frame data;
    AudioOutputObject(const std::vector<TypedValue>&);
};






class ComparatorObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float value = 0.f;

public:
    ComparatorObject(const std::vector<TypedValue>&);
};



class TimerObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float value = 0;

public:
    TimerObject(const std::vector<TypedValue>&);
};



class ClockObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float interval = 1;
    float elapsed = 0;

public:
    ClockObject(const std::vector<TypedValue>&);
};



class DivisionObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float divisor = 0;

public:
    DivisionObject(const std::vector<TypedValue>&);
};



class SubtractionObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float subtrahend = 0;

public:
    SubtractionObject(const std::vector<TypedValue>&);
};



class ModuloObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float divisor = 0;

public:
    ModuloObject(const std::vector<TypedValue>&);
};



class AbsoluteValueObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    AbsoluteValueObject(const std::vector<TypedValue>&);
};


class StepSequence : public AudioObject
{
    Sequence sequence;
    std::size_t current = 0;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    StepSequence(const std::vector<TypedValue>&);
};



class PowerObject : public AudioObject
{
    float exponent = 2;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    PowerObject(const std::vector<TypedValue>&);
};



class EnvelopeObject : public AudioObject
{
    float length = 1000;
    float start = 1.f;
    float end = 0.f;
    int time = 0;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    EnvelopeObject(const std::vector<TypedValue>&);
};


class RoundObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    RoundObject(const std::vector<TypedValue>&);
};


class SequenceObject : public AudioObject
{
    Sequence sequence;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    SequenceObject(const std::vector<TypedValue>&);
};


class SampleAndHoldObject : public AudioObject
{
    float value = 0;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    SampleAndHoldObject(const std::vector<TypedValue>&);
};



class ConstObject : public AudioObject
{
    float value = 0;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    ConstObject(const std::vector<TypedValue>&);
};

class SawObject : public AudioObject
{
    float phase = -1;
    float frequency;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    SawObject(const std::vector<TypedValue>&);
};

class TriangleObject : public AudioObject
{
    float phase = 0;
    float frequency;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    TriangleObject(const std::vector<TypedValue>&);
};

class BiquadObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
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
    BiquadObject(const std::vector<TypedValue>&);
};

class LowpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    LowpassObject(const std::vector<TypedValue>& parameters) :
        BiquadObject(parameters) { }
};

class HighpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    HighpassObject(const std::vector<TypedValue>& parameters) :
        BiquadObject(parameters) { }
};

class BandpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    BandpassObject(const std::vector<TypedValue>& parameters) :
        BiquadObject(parameters) { }
};

class AllpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    AllpassObject(const std::vector<TypedValue>& parameters) :
        BiquadObject(parameters) { }
};


class EnvelopeFollowerObject : public AudioObject
{
    static float constexpr time_constant = -0.99967234f;

    float attack = 441;
    float release = 441;
    float last_value = 0.f;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    EnvelopeFollowerObject(const std::vector<TypedValue>&);
};

class SubgraphObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    std::unique_ptr<Program> graph;
    SubgraphObject(const std::vector<TypedValue>&);
};

}
