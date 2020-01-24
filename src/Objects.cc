
#include <fstream>
#include <vector>
#include <cmath>

#include "Objects.hh"

namespace Volsung {

void AddObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = in[0][0] + default_value;
}

AddObject::AddObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &default_value });
    set_defval(&default_value, default_value, 1);
}



void DelayObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    const float lower = in[0][-sample_delay];
    const float upper = in[0][-(sample_delay+0.5)];
    const float ratio = sample_delay - (int) sample_delay;
    
    out[0][0] = (1-ratio) * lower + ratio * upper;
}

DelayObject::DelayObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &sample_delay });
    set_defval(&sample_delay, sample_delay, 1);
    request_buffer_size(sample_delay + 1);
}



void DriveObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = tanh(in[0][0]);
}

DriveObject::DriveObject(const std::vector<TypedValue>&)
{
    set_io(2, 1);
}



void FileoutObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    if (is_connected(0)) in_data.push_back(in[0][0]);
    if (out_data.size()) {
        out[0][0] = out_data[pos++];
        if (pos >= out_data.size()) pos = out_data.size()-1;
    }
}

void FileoutObject::finish()
{
    if (is_connected(0)) {
        log("writing file");
        std::ofstream file(filename, std::fstream::out | std::fstream::binary);
        for (std::size_t n = 0; n < in_data.size(); n++)
            file.write((const char*)& in_data[n], sizeof(float));

        file.close();
    }
}

FileoutObject::FileoutObject(const std::vector<TypedValue>& parameters)
{
    if (!parameters.size()) error("Expected a string argument on file object");
    filename = parameters[0].get_value<Text>();
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    
    if (file.good()) {
        log("reading file");
        log("size: " + std::to_string(file.tellg()));
        out_data.resize(file.tellg() / sizeof(float));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(out_data.data()), out_data.size() * sizeof(float));
    }
    set_io(1, 1);
}



void FilterObject::process(const MultichannelBuffer& x, MultichannelBuffer& y)
{
    b = 2 - cos(TAU * frequency / sample_rate);
    b = sqrt(b*b - 1) - b;
    a = 1 + b;

    y[0][0] = a*x[0][0] - b*y[0][-1];
}

FilterObject::FilterObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &frequency });
    set_defval(&frequency, frequency, 1);
}



void MultObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = in[0][0] * default_value;
}

MultObject::MultObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &default_value });
    set_defval(&default_value, default_value, 1);
}



void NoiseObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    out[0][0] = distribution(generator);
}

NoiseObject::NoiseObject(const std::vector<TypedValue>&) :
    distribution(-1.0f, 1.0f), generator(std::chrono::system_clock::now().time_since_epoch().count())
{ set_io(0, 1); }



void OscillatorObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    out[0][0] = sinf(TAU * phase);

    phase = phase + frequency / sample_rate;

    if (phase >= 1.0) { phase -= 1.0; }
    if (gate_opened(1)) phase = 0;
}

OscillatorObject::OscillatorObject(const std::vector<TypedValue>& parameters) :  phase(0)
{
    init(2, 1, parameters, {&frequency} );
    set_defval(&frequency, frequency, 0);
}



void SquareObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{   
    out[0][0] = (float)sign<float>(sinf(TAU * phase) + pw);

    phase = phase + frequency / sample_rate;

    if (phase >= 1.0) { phase -= 1.0; }
}

SquareObject::SquareObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &frequency, &pw });
    set_defval(&frequency, frequency, 0);
    set_defval(&pw, pw, 1);
}



void UserObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    if (callback)
        callback(in, out, user_data);
}

UserObject::UserObject(const std::vector<TypedValue>& parameters, const CallbackFunctor callback_, std::any user_data_) : callback(callback_), user_data(user_data_)
{
    float inputs = 0, outputs = 0;
    inputs  = parameters[0].get_value<Number>();
    outputs = parameters[1].get_value<Number>();
    set_io(inputs, outputs);
}



void AudioInputObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    for (auto& output : out) {
        output[0] = data[0];
    }   
    for (std::size_t n = 0; n < out.size(); n++) out[n][0] = data[n];
}

AudioInputObject::AudioInputObject(const std::vector<TypedValue>& parameters)
{
    float outputs = 0;
    outputs = parameters[0].get_value<Number>();
    set_io(0, outputs);
    data.resize(outputs);
}



void AudioOutputObject::process(const MultichannelBuffer& in, MultichannelBuffer&)
{
    for (std::size_t n = 0; n < in.size(); n++) data[n] = in[n][0];
}

AudioOutputObject::AudioOutputObject(const std::vector<TypedValue>& parameters)
{
    float inputs = 0;
    inputs = parameters[0].get_value<Number>();
    set_io(inputs, 0);
    data.resize(inputs);
}



void ComparatorObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    if (in[0][0] > value) out[0][0] = 1.f;
    else out[0][0] = 0.f;
}

ComparatorObject::ComparatorObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &value });
    set_defval(&value, value, 1);
}


void TimerObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    out[0][0] = value;
    value += 1.f / sample_rate;
    if (gate_opened(0)) value = 0.f;
}

TimerObject::TimerObject(const std::vector<TypedValue>&)
{
    set_io(1, 1);
}


void ClockObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    out[0][0] = 0;
    if (elapsed >= interval) {
        out[0][0] = 1;
        elapsed = 0.f;
    }
    
    elapsed += 1.f;
}

ClockObject::ClockObject(const std::vector<TypedValue>& parameters)
{
    init(1, 1, parameters, { &interval });
    set_defval(&interval, interval, 0);
    Volsung::log("created clock objects, output count: " + std::to_string(outputs.size()));
}


void DivisionObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = in[0][0] / divisor;
}

DivisionObject::DivisionObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &divisor });
    set_defval(&divisor, divisor, 1);
}


void SubtractionObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = in[0][0] - subtrahend;
}

SubtractionObject::SubtractionObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &subtrahend });
    set_defval(&subtrahend, subtrahend, 1);
}


void ModuloObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = std::fmod(in[0][0], divisor);
}

ModuloObject::ModuloObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &divisor });
    set_defval(&divisor, divisor, 1);
}


void AbsoluteValueObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = std::fabs(in[0][0]);
}

AbsoluteValueObject::AbsoluteValueObject(const std::vector<TypedValue>&)
{
    set_io(1, 1);
}


void StepSequence::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    if (gate_opened(0)) {
        current %= sequence.size();
        current++;
    }
    out[0][0] = sequence[current];
}

StepSequence::StepSequence(const std::vector<TypedValue>& parameters)
{
    set_io(1, 1);
    sequence = parameters[0].get_value<Sequence>();
}


void PowerObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = std::pow(in[0][0], exponent);
}

PowerObject::PowerObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &exponent });
    set_defval(&exponent, exponent, 1);
}




void EnvelopeObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    if (gate_opened(0)) time = 0;
    if (time > length) time = length;
    
    const float ratio = float(time) / (length+0.001);
    out[0][0] = (1-ratio) * start + ratio * end;
    time++;
    
    //out[1][0] = 0.f;
    //if (time == length) out[1][0] = 1.f;
}

EnvelopeObject::EnvelopeObject(const std::vector<TypedValue>& parameters)
{
    init(4, 1, parameters, { &length, &start, &end });
    set_defval(&length, length, 1);
    set_defval(&start, start, 2);
    set_defval(&end, end, 3);
}

void RoundObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = std::round(in[0][0]);
}

RoundObject::RoundObject(const std::vector<TypedValue>&)
{
    set_io(1, 1);
}


void SequenceObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    out[0][0] = sequence[(int) in[0][0]];
}

SequenceObject::SequenceObject(const std::vector<TypedValue>& parameters)
{
    set_io(1, 1);
    sequence = parameters[0].get_value<Sequence>();
}


void SampleAndHoldObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    if (gate_opened(1)) value = in[0][0];
    out[0][0] = value;
}

SampleAndHoldObject::SampleAndHoldObject(const std::vector<TypedValue>&)
{
    set_io(2, 1);
}


void ConstObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    out[0][0] = value;
}

ConstObject::ConstObject(const std::vector<TypedValue>& parameters)
{
    value = parameters[0].get_value<Number>();
    set_io(0, 1);
}


void SawObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    if (gate_opened(1)) phase = -1;
    
    phase += std::abs(2.f * frequency / sample_rate);
    if (phase > 1.f) phase = -1.f;
    
    if (frequency < 0) out[0][0] = -phase;
    else out[0][0] = phase;
}

SawObject::SawObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &frequency });
    set_defval(&frequency, frequency, 0);
}

void TriangleObject::process(const MultichannelBuffer&, MultichannelBuffer& out)
{
    if (gate_opened(1)) phase = 0;
    
    phase += frequency / sample_rate;
    if (phase >= 1.f) phase -= 1.f;
    out[0][0] = 2.f * fabs(2.f * phase - 1.f) - 1.f;
}

TriangleObject::TriangleObject(const std::vector<TypedValue>& parameters)
{
    init(2, 1, parameters, { &frequency });
    set_defval(&frequency, frequency, 0);
}

#include <limits>
void BiquadObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    if (!resonance) resonance = std::numeric_limits<float>::min();
    omega = TAU * frequency / sample_rate;
    alpha = std::sin(omega) / (2.f * resonance);
    cos_omega = std::cos(omega);

    calculate_coefficients();

    auto& x = in[0];
    auto& y = out[0];

    y[0] = (b0*x[0] + b1*x[-1] + b2*x[-2] - a1*y[-1] - a2*y[-2]) / a0;
}

BiquadObject::BiquadObject(const std::vector<TypedValue>& parameters)
{
    init(3, 1, parameters, { &frequency, &resonance });
    set_defval(&frequency, frequency, 1);
    set_defval(&resonance, resonance, 2);
    request_buffer_size(4);
}

void LowpassObject::calculate_coefficients()
{
    a0 = 1 + alpha;
    a1 = -2 * cos_omega;
    a2 = 1 - alpha;
    b0 = (1 - cos_omega) / 2;
    b1 = 1 - cos_omega;
    b2 = b0;
}

void HighpassObject::calculate_coefficients()
{
    a0 = 1 + alpha;
    a1 = -2 * cos_omega;
    a2 = 1 - alpha;
    b0 = (1 + cos_omega) / 2;
    b1 = -(1 + cos_omega);
    b2 = b0;
}

void BandpassObject::calculate_coefficients()
{
    a0 = 1 + alpha;
    a1 = -2 * cos_omega;
    a2 = 1 - alpha;
    b0 = resonance * alpha;
    b1 = 0;
    b2 = -resonance * alpha;
}

void AllpassObject::calculate_coefficients()
{
    a0 = 1 + alpha;
    a1 = -2 * cos_omega;
    a2 = 1 - alpha;
    b0 = 1 - alpha;
    b1 = -2 * cos_omega;
    b2 = 1 + alpha;
}




void EnvelopeFollowerObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    const float sample = std::fabs(in[0][0]);
    
    const float internal_attack = std::exp(time_constant / attack);
    const float internal_release = std::exp(time_constant / release);
    
    float detector_value = 0.f;
    if (sample > last_value)
        detector_value = internal_attack * (last_value - sample) + sample;
    else
        detector_value = internal_release * (last_value - sample) + sample;

    if (detector_value < 0.f) detector_value = 0.f;

    last_value = detector_value;
    out[0][0] = detector_value;
}

EnvelopeFollowerObject::EnvelopeFollowerObject(const std::vector<TypedValue>& parameters)
{
    init(3, 1, parameters, { &attack, &release });
    set_defval(&attack, attack, 1);
    set_defval(&release, release, 2);
}

void SubgraphObject::process(const MultichannelBuffer& in, MultichannelBuffer& out)
{
    if (!graph) error("Null pointer for subpatch");

    Frame input_frame;
    for (std::size_t n = 0; n < inputs.size(); n++) input_frame.push_back(in[n][0]);

    const Frame output_frame = graph->run(input_frame);

    for (std::size_t n = 0; n < outputs.size(); n++) out[n][0] = output_frame[n];
}

SubgraphObject::SubgraphObject(const std::vector<TypedValue>& parameters)
{
    float inputs = 0, outputs = 0;
    inputs  = parameters[0].get_value<Number>();
    outputs = parameters[1].get_value<Number>();
    set_io(inputs, outputs);
}

}
