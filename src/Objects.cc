
#include <limits>
#include <fstream>
#include <vector>
#include <cmath>

#include "Objects.hh"

namespace Volsung {

void AddObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = input_buffer[0][0] + default_value;
}

AddObject::AddObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &default_value });
    set_defval(&default_value, default_value, 1);
}



void DelayObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    const float lower = input_buffer[0][(long) -sample_delay];
    const float upper = input_buffer[0][(long) -(sample_delay+0.5f)];
    const float ratio = sample_delay - (int) sample_delay;
    
    output_buffer[0][0] = (1-ratio) * lower + ratio * upper;
}

DelayObject::DelayObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &sample_delay });
    set_defval(&sample_delay, sample_delay, 1);
    request_buffer_size((size_t) sample_delay + 1);
}



void DriveObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = tanh(input_buffer[0][0]);
}

DriveObject::DriveObject(const ArgumentList&)
{
    set_io(2, 1);
}



void FileoutObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (is_connected(0)) in_data.push_back(input_buffer[0][0]);
    if (out_data.size()) {
        output_buffer[0][0] = out_data[pos++];
        if (pos >= out_data.size()) pos = out_data.size()-1;
    }
}

void FileoutObject::finish()
{
    if (is_connected(0)) {
        std::ofstream file(filename, std::fstream::out | std::fstream::binary);
        for (size_t n = 0; n < in_data.size(); n++)
            file.write((const char*)& in_data[n], sizeof(float));

        file.close();
    }
}

FileoutObject::FileoutObject(const ArgumentList& parameters)
{
    if (!parameters.size()) error("Expected a string argument on file object");
    filename = parameters[0].get_value<Text>();
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    
    if (file.good()) {
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

FilterObject::FilterObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency });
    set_defval(&frequency, frequency, 1);
}



void MultObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = input_buffer[0][0] * default_value;
}

MultObject::MultObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &default_value });
    set_defval(&default_value, default_value, 1);
}



void NoiseObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = distribution(generator);
}

NoiseObject::NoiseObject(const ArgumentList&) :
    distribution(-1.0f, 1.0f), generator((uint) std::chrono::system_clock::now().time_since_epoch().count())
{ set_io(0, 1); }



void OscillatorObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = sinf(TAU * phase);

    phase = phase + frequency / sample_rate;

    if (phase >= 1.0) { phase -= 1.0; }
    if (gate_opened(1)) phase = 0;
}

OscillatorObject::OscillatorObject(const ArgumentList& parameters) :  phase(0)
{
    init(2, 1, parameters, {&frequency} );
    set_defval(&frequency, frequency, 0);
}



void SquareObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{   
    output_buffer[0][0] = (float)sign<float>(sinf(TAU * phase) + pw);

    phase = phase + frequency / sample_rate;

    if (phase >= 1.0) { phase -= 1.0; }
}

SquareObject::SquareObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency, &pw });
    set_defval(&frequency, frequency, 0);
    set_defval(&pw, pw, 1);
}



void UserObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (callback)
        callback(input_buffer, output_buffer, user_data);
}

UserObject::UserObject(const ArgumentList& parameters, const AudioProcessingCallback callback_, std::any user_data_) : callback(callback_), user_data(user_data_)
{
    const float num_inputs  = parameters[0].get_value<Number>();
    const float num_outputs = parameters[1].get_value<Number>();
    set_io((uint) num_inputs, (uint) num_outputs);
}



void AudioInputObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    for (auto& output : output_buffer) {
        output[0] = data[0];
    }   
    for (size_t n = 0; n < output_buffer.size(); n++) output_buffer[n][0] = data[n];
}

AudioInputObject::AudioInputObject(const ArgumentList& parameters)
{
    const float num_outputs = parameters[0].get_value<Number>();
    set_io(0, (uint) num_outputs);
    data.resize((uint) num_outputs);
}



void AudioOutputObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer&)
{
    for (size_t n = 0; n < input_buffer.size(); n++) data[n] = input_buffer[n][0];
}

AudioOutputObject::AudioOutputObject(const ArgumentList& parameters)
{
    const float num_inputs = parameters[0].get_value<Number>();
    set_io((uint) num_inputs, 0);
    data.resize((uint) num_inputs);
}



void ComparatorObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (input_buffer[0][0] > value) output_buffer[0][0] = 1.f;
    else output_buffer[0][0] = 0.f;
}

ComparatorObject::ComparatorObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &value });
    set_defval(&value, value, 1);
}


void TimerObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = value;
    value += 1.f / sample_rate;
    if (gate_opened(0)) value = 0.f;
}

TimerObject::TimerObject(const ArgumentList&)
{
    set_io(1, 1);
}


void ClockObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = 0;
    if (elapsed >= interval) {
        output_buffer[0][0] = 1;
        elapsed = 0.f;
    }
    
    elapsed += 1.f;
}

ClockObject::ClockObject(const ArgumentList& parameters)
{
    init(1, 1, parameters, { &interval });
    set_defval(&interval, interval, 0);
}


void DivisionObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = input_buffer[0][0] / divisor;
}

DivisionObject::DivisionObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &divisor });
    set_defval(&divisor, divisor, 1);
}


void SubtractionObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = input_buffer[0][0] - subtrahend;
}

SubtractionObject::SubtractionObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &subtrahend });
    set_defval(&subtrahend, subtrahend, 1);
}


void ModuloObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = std::fmod(input_buffer[0][0], divisor);
}

ModuloObject::ModuloObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &divisor });
    set_defval(&divisor, divisor, 1);
}


void AbsoluteValueObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = std::fabs(input_buffer[0][0]);
}

AbsoluteValueObject::AbsoluteValueObject(const ArgumentList&)
{
    set_io(1, 1);
}


void StepSequence::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    if (gate_opened(0)) {
        current %= sequence.size();
        current++;
    }
    output_buffer[0][0] = sequence[current];
}

StepSequence::StepSequence(const ArgumentList& parameters)
{
    set_io(1, 1);
    sequence = parameters[0].get_value<Sequence>();
}


void PowerObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = std::pow(input_buffer[0][0], exponent);
}

PowerObject::PowerObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &exponent });
    set_defval(&exponent, exponent, 1);
}




void EnvelopeObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    if (gate_opened(0)) time = 0;
    if (time > length) time = (int) length;
    
    const float ratio = float(time) / (length + 0.001f);
    output_buffer[0][0] = (1-ratio) * start + ratio * end;
    time++;
    
    //output_buffer[1][0] = 0.f;
    //if (time == length) output_buffer[1][0] = 1.f;
}

EnvelopeObject::EnvelopeObject(const ArgumentList& parameters)
{
    init(4, 1, parameters, { &length, &start, &end });
    set_defval(&length, length, 1);
    set_defval(&start, start, 2);
    set_defval(&end, end, 3);
}

void RoundObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = std::round(input_buffer[0][0]);
}

RoundObject::RoundObject(const ArgumentList&)
{
    set_io(1, 1);
}


void SequenceObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    auto index = (size_t) std::max(0.f, input_buffer[0][0]);
    if (index >= sequence.size()) index = sequence.size() - 1;
    output_buffer[0][0] = sequence[index];
}

SequenceObject::SequenceObject(const ArgumentList& parameters)
{
    set_io(1, 1);
    sequence = parameters[0].get_value<Sequence>();
}


void SampleAndHoldObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (gate_opened(1)) value = input_buffer[0][0];
    output_buffer[0][0] = value;
}

SampleAndHoldObject::SampleAndHoldObject(const ArgumentList&)
{
    set_io(2, 1);
}


void ConstObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = value;
}

ConstObject::ConstObject(const ArgumentList& parameters)
{
    value = parameters[0].get_value<Number>();
    set_io(0, 1);
}


void SawObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    if (gate_opened(1)) phase = -1;
    
    phase += std::abs(2.f * frequency / sample_rate);
    if (phase > 1.f) phase = -1.f;
    
    if (frequency < 0) output_buffer[0][0] = -phase;
    else output_buffer[0][0] = phase;
}

SawObject::SawObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency });
    set_defval(&frequency, frequency, 0);
}

void TriangleObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    if (gate_opened(1)) phase = 0;
    
    phase += frequency / sample_rate;
    if (phase >= 1.f) phase -= 1.f;
    output_buffer[0][0] = 2.f * fabs(2.f * phase - 1.f) - 1.f;
}

TriangleObject::TriangleObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency });
    set_defval(&frequency, frequency, 0);
}

void BiquadObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (!resonance) resonance = std::numeric_limits<float>::min();
    omega = TAU * frequency / sample_rate;
    alpha = std::sin(omega) / (2.f * resonance);
    cos_omega = std::cos(omega);

    calculate_coefficients();

    auto& x = input_buffer[0];
    auto& y = output_buffer[0];

    y[0] = (b0*x[0] + b1*x[-1] + b2*x[-2] - a1*y[-1] - a2*y[-2]) / a0;
}

BiquadObject::BiquadObject(const ArgumentList& parameters)
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




void EnvelopeFollowerObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    const float sample = std::fabs(input_buffer[0][0]);
    
    const float internal_attack = std::exp(time_constant / attack);
    const float internal_release = std::exp(time_constant / release);
    
    float detector_value = 0.f;
    if (sample > last_value)
        detector_value = internal_attack * (last_value - sample) + sample;
    else
        detector_value = internal_release * (last_value - sample) + sample;

    if (detector_value < 0.f) detector_value = 0.f;

    last_value = detector_value;
    output_buffer[0][0] = detector_value;
}

EnvelopeFollowerObject::EnvelopeFollowerObject(const ArgumentList& parameters)
{
    init(3, 1, parameters, { &attack, &release });
    set_defval(&attack, attack, 1);
    set_defval(&release, release, 2);
}

void SubgraphObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (!graph) error("Null pointer for subpatch");

    Frame input_frame;
    for (size_t n = 0; n < inputs.size(); n++) input_frame.push_back(input_buffer[n][0]);

    const Frame output_frame = graph->run(input_frame);

    for (size_t n = 0; n < outputs.size(); n++) output_buffer[n][0] = output_frame[n];
}

SubgraphObject::SubgraphObject(const ArgumentList& parameters)
{
    const auto num_inputs  = (uint) parameters[0].get_value<Number>();
    const auto num_outputs = (uint) parameters[1].get_value<Number>();
    set_io(num_inputs, num_outputs);
}

void ConvolveObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    float value = 0;
    for (size_t n = 0; n < impulse_response.size(); n++) {
        value += impulse_response[n] * input_buffer[0][(long) n-1];
    }
    output_buffer[0][0] = value;
}

ConvolveObject::ConvolveObject(const ArgumentList& parameters)
{
    set_io(1, 1);
    impulse_response = parameters[0].get_value<Sequence>();
    request_buffer_size(impulse_response.size());
}

void ZPlaneObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    auto& x = input_buffer[0];
    auto& y = output_buffer[0];

    y[0] = (x[0] + b1*x[-1] + b2*x[-2] - a1*y[-1] - a2*y[-2]);
}

ZPlaneObject::ZPlaneObject(const ArgumentList& parameters)
{
    set_io(1, 1);
    if (parameters.size() > 0) zero = parameters[0].get_value<Number>();
    if (parameters.size() > 1) pole = parameters[1].get_value<Number>();
    request_buffer_size(4);

    if (zero.magnitude()) {
        b1 = -2.f * zero.magnitude() * std::cos(zero.angle());
        b2 = zero.magnitude() * zero.magnitude();
    }

    if (pole.magnitude()) {
        a1 = -2.f * pole.magnitude() * std::cos(pole.angle());
        a2 = pole.magnitude() * pole.magnitude();
    }
}


void PoleObject::process(const MultichannelBuffer& x, MultichannelBuffer& y)
{
    y[0][0] = x[0][0] + a*y[0][-1];
}

PoleObject::PoleObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &a });
    set_defval(&a, a, 1);
}


void ZeroObject::process(const MultichannelBuffer& x, MultichannelBuffer& y)
{
    y[0][0] = x[0][0] + b*x[0][-1];
}

ZeroObject::ZeroObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &b });
    set_defval(&b, b, 1);
}

void BiToUnipolarObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer[0][0] = 0.5f + 0.5f * input_buffer[0][0];
}

BiToUnipolarObject::BiToUnipolarObject(const ArgumentList&)
{
    set_io(1, 1);
}


}
