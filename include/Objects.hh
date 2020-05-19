
#pragma once

#include <any>
#include <string>
#include <random>
#include <functional>
#include <queue>

#include "Volsung.hh"
#include "AudioObject.hh"
#include "Graph.hh"

namespace Volsung {

class AddObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float default_value = 1;

public:
    AddObject(const ArgumentList&);
};

class DelayObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float sample_delay = sample_rate;
    CircularBuffer delay_buffer;

public:
    DelayObject(const ArgumentList&);
};

class DriveObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    DriveObject(const ArgumentList&);
};

class FileoutObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    void finish() override;

    std::vector<float> data;
    std::string filename;
    size_t size = 0;
    size_t pos = 0;

public:
    FileoutObject(const ArgumentList&);
};

class FileinObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

    std::vector<float> data;
    std::string filename;
    size_t pos = 0;

public:
    FileinObject(const ArgumentList&);
};

class FilterObject : public AudioObject
{
    float frequency = 100;
    double a;
    double b;
    double last_value = 0.f;
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    FilterObject(const ArgumentList&);
};

class MultObject : public AudioObject
{
    float multiplier = 1;
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    MultObject(const ArgumentList&);
};

class NoiseObject : public AudioObject
{
    std::uniform_real_distribution<float> distribution;
    std::default_random_engine generator;
    inline static size_t seed = 0;
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    NoiseObject(const ArgumentList&);
};

class OscillatorObject : public AudioObject
{
    void  process(const MultichannelBuffer&, MultichannelBuffer&) override;

    GateListener sync;
    double phase     = 0;
    float phase_offset = 0;
    float frequency = 100;

public:
    OscillatorObject(const ArgumentList&);
};

class SquareObject : public AudioObject
{
    void  process(const MultichannelBuffer&, MultichannelBuffer&) override;

    float pw        = 0.0;
    double phase     = 0;
    float frequency = 100;

public:
    SquareObject(const ArgumentList&);
};

class UserObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

    const AudioProcessingCallback callback;
    std::any user_data;

public:
    UserObject(const ArgumentList&, const AudioProcessingCallback, std::any);
};

class AudioInputObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    
public:
    MultichannelBuffer data;
    AudioInputObject(const ArgumentList&);
};

class AudioOutputObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    MultichannelBuffer data;
    AudioOutputObject(const ArgumentList&);
};

class ComparatorObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float value = 0.f;

public:
    ComparatorObject(const ArgumentList&);
};

class TimerObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float value = 0;
    GateListener reset;

public:
    TimerObject(const ArgumentList&);
};

class ClockObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float interval = 1;
    float elapsed = 0;

public:
    ClockObject(const ArgumentList&);
};

class DivisionObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float divisor = 0;

public:
    DivisionObject(const ArgumentList&);
};

class SubtractionObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float subtrahend = 0;

public:
    SubtractionObject(const ArgumentList&);
};

class ModuloObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float divisor = 0;

public:
    ModuloObject(const ArgumentList&);
};

class AbsoluteValueObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    AbsoluteValueObject(const ArgumentList&);
};

class StepSequence : public AudioObject
{
    Sequence sequence;
    std::size_t current = 0;
    GateListener step;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    StepSequence(const ArgumentList&);
};

class PowerObject : public AudioObject
{
    float exponent = 2;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    PowerObject(const ArgumentList&);
};

class EnvelopeObject : public AudioObject
{
    float length = 1000;
    float start = 1.f;
    float end = 0.f;
    int time;

    GateListener trigger;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    EnvelopeObject(const ArgumentList&);
};

class RoundObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    RoundObject(const ArgumentList&);
};

class SequenceObject : public AudioObject
{
    Sequence sequence;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    SequenceObject(const ArgumentList&);
};

class SampleAndHoldObject : public AudioObject
{
    float value = 0;
    GateListener trigger;
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    SampleAndHoldObject(const ArgumentList&);
};

class ConstObject : public AudioObject
{
    float value = 0;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    ConstObject(const ArgumentList&);
};

class SawObject : public AudioObject
{
    double phase = -1;
    float frequency;
    GateListener sync;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
public:
    SawObject(const ArgumentList&);
};

class TriangleObject : public AudioObject
{
    double phase = 0;
    float frequency;
    GateListener sync;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    TriangleObject(const ArgumentList&);
};

class BiquadObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
protected:
    float a0, a1, a2, b0, b1, b2;

    float frequency;
    float resonance = 1.f;

    float omega;
    float alpha;
    float cos_omega;
    float A;

    CircularBuffer x, y;

    virtual void calculate_coefficients() = 0;

public:
    BiquadObject(const ArgumentList&);
};

class LowpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    LowpassObject(const ArgumentList& parameters) :
        BiquadObject(parameters) { }
};

class HighpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    HighpassObject(const ArgumentList& parameters) :
        BiquadObject(parameters) { }
};

class BandpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    BandpassObject(const ArgumentList& parameters) :
        BiquadObject(parameters) { }
};

class AllpassObject : public BiquadObject
{
    void calculate_coefficients() override;
public:
    AllpassObject(const ArgumentList& parameters) :
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
    EnvelopeFollowerObject(const ArgumentList&);
};

class SubgraphObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    std::unique_ptr<Program> graph;
    SubgraphObject(const ArgumentList&);
};

class ConvolveObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    CircularBuffer signal;
    Sequence impulse_response;
public:
    ConvolveObject(const ArgumentList&);
};

class PoleObject : public AudioObject
{
    float a1 = 0;
    float a2 = 0;
    Number position;
    CircularBuffer y;
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    PoleObject(const ArgumentList&);
};

class ZeroObject : public AudioObject
{
    float b1 = 0;
    float b2 = 0;
    Number position;
    CircularBuffer x;

    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    ZeroObject(const ArgumentList&);
};


class BiToUnipolarObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    BiToUnipolarObject(const ArgumentList&);
};




class CeilObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    CeilObject(const ArgumentList&);
};

class CosObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    CosObject(const ArgumentList&);
};

class SinObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    SinObject(const ArgumentList&);
};

class ClampObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float min = 0.f;
    float max = 1.f;
public:
    ClampObject(const ArgumentList&);
};

class ReciprocalObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    ReciprocalObject(const ArgumentList&);
};

class InverseObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float offset;
public:
    InverseObject(const ArgumentList&);
};






class SignObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    SignObject(const ArgumentList&);
};

class LogarithmObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float base = 2.71828182846;

public:
    LogarithmObject(const ArgumentList&);
};

class ExponentialObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;
    float base = 2.71828182846;

public:
    ExponentialObject(const ArgumentList&);
};

class AtanObject : public AudioObject
{
    void process(const MultichannelBuffer&, MultichannelBuffer&) override;

public:
    AtanObject(const ArgumentList&);
};


class PhasorObject : public AudioObject
{
    void  process(const MultichannelBuffer&, MultichannelBuffer&) override;

    GateListener sync;
    double phase     = 0;
    float phase_offset = 0;
    float period = sample_rate;

public:
    PhasorObject(const ArgumentList&);
};



class InvokeObject : public AudioObject
{
    void  process(const MultichannelBuffer&, MultichannelBuffer&) override;

    Procedure::Implementation function;

public:
    InvokeObject(const ArgumentList&);
};

}
