
#include <limits>
#include <fstream>
#include <vector>
#include <cmath>

#include "Objects.hh"

namespace Volsung {

void AddObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        output_buffer[0][n] = input_buffer[0][n] + default_value;
    }
}

AddObject::AddObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &default_value });
    link_value(&default_value, default_value, 1);
}



void DelayObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        delay_buffer[0] = input_buffer[0][n];

        const float lower = delay_buffer[(int) -std::ceil (sample_delay)];
        const float upper = delay_buffer[(int) -std::floor(sample_delay)];
        const float ratio = sample_delay - std::floor(sample_delay);

        output_buffer[0][n] = (1-ratio) * lower + ratio * upper;
        delay_buffer.increment_pointer();
    }
}

DelayObject::DelayObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &sample_delay });
    link_value(&sample_delay, sample_delay, 1);
    delay_buffer.resize_stream(sample_delay + 10000);
}



void DriveObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        output_buffer[0][n] = std::tanh(input_buffer[0][n]);
    }
}

DriveObject::DriveObject(const ArgumentList&)
{
    set_io(2, 1);
}



void FileoutObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer&)
{
    if (pos + AudioBuffer::blocksize >= size) return;
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        data[pos++] = input_buffer[0][n];
    }
}

void FileoutObject::finish()
{
    std::ofstream file(filename, std::fstream::out | std::fstream::binary);

    for (size_t n = 0; n < data.size(); n++) {
        file.write(reinterpret_cast<const char*> (&data[n]), sizeof(float));
    }

    file.close();
}

FileoutObject::FileoutObject(const ArgumentList& parameters)
{
    if (!parameters.size()) error("Expected a string argument on file object");
    filename = static_cast<std::string> (parameters[0].get_value<Text>());

    if (!(parameters.size() - 1)) error("Expected a size to write to file");
    size = parameters[1].get_value<Number>();
    data.resize(size);
    set_io(1, 0);
}


void FileinObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        if (pos < data.size()) {
            output_buffer[0][n] = data[pos++];
        }
        else output_buffer[0][n] = 0.f;
    }
}

FileinObject::FileinObject(const ArgumentList& parameters)
{
    if (!parameters.size()) error("Expected a string argument on file object");
    filename = parameters[0].get_value<Text>();
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

    if (file.good()) {
        data.resize(file.tellg() / sizeof(float));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(float));
    }
    else error("Input file '" + filename + "' could not be read, not found");
    set_io(0, 1);
}



void FilterObject::process(const MultichannelBuffer& x, MultichannelBuffer& y)
{
    update_parameters(0);
    b = 2.0 - std::cos(TAU * frequency / get_sample_rate());
    b = std::sqrt(b*b - 1.0) - b;
    a = 1.0 + b;

    y[0][0] = a*x[0][0] - b*last_value;

    for (size_t n = 1; n < AudioBuffer::blocksize; n++) { 
        update_parameters(n);
        b = 2.0 - std::cos(TAU * frequency / get_sample_rate());
        b = sqrt(b*b - 1.0) - b;
        a = 1.0 + b;

        y[0][n] = a*x[0][n] - b*y[0][n-1];
    }

    last_value = y[0][AudioBuffer::blocksize - 1];
}

FilterObject::FilterObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency });
    link_value(&frequency, frequency, 1);
}



void MultObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        output_buffer[0][n] = input_buffer[0][n] * multiplier;
    }
}

MultObject::MultObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &multiplier });
    link_value(&multiplier, multiplier, 1);
}



void NoiseObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = distribution(generator);
}

NoiseObject::NoiseObject(const ArgumentList&) :
    distribution(-1.0f, 1.0f), generator(seed++)
{ set_io(0, 1); }



void OscillatorObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        if (sync.read_gate_state(input_buffer[1][n]) & GateState::just_opened)
            phase = 0;

        output_buffer[0][n] = std::sin(TAU * phase + phase_offset);

        phase = phase + frequency / get_sample_rate();

        if (phase >= 1.0) { phase -= 1.0; }
    }
}

OscillatorObject::OscillatorObject(const ArgumentList& parameters) :  phase(0)
{
    init(3, 1, parameters, { &frequency, &phase_offset } );
    link_value(&frequency, frequency, 0);
    link_value(&phase_offset, phase_offset, 2);
}



void SquareObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        output_buffer[0][n] = (float) sign<float>(sinf(TAU * phase) + pw);

        phase = phase + frequency / get_sample_rate();

        if (phase >= 1.0) { phase -= 1.0; }
    }
}

SquareObject::SquareObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency, &pw });
    link_value(&frequency, frequency, 0);
    link_value(&pw, pw, 1);
}



void UserObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    if (callback) {
        callback(input_buffer, output_buffer, user_data);
    }
}

UserObject::UserObject(const ArgumentList& parameters, const AudioProcessingCallback callback_, std::any user_data_) : callback(callback_), user_data(user_data_)
{
    const float num_inputs  = parameters[0].get_value<Number>();
    const float num_outputs = parameters[1].get_value<Number>();
    set_io((uint) num_inputs, (uint) num_outputs);
}



void AudioInputObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    output_buffer = data;
}

AudioInputObject::AudioInputObject(const ArgumentList& parameters)
{
    const float num_outputs = parameters[0].get_value<Number>();
    set_io(0, (uint) num_outputs);
    data.resize((uint) num_outputs);
}



void AudioOutputObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer&)
{
    data = input_buffer;
}

AudioOutputObject::AudioOutputObject(const ArgumentList& parameters)
{
    const float num_inputs = parameters[0].get_value<Number>();
    set_io((uint) num_inputs, 0);
    data.resize((uint) num_inputs);
}



void ComparatorObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        if (input_buffer[0][n] > value) output_buffer[0][n] = 1.f;
        else output_buffer[0][n] = 0.f;
    }
}

ComparatorObject::ComparatorObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &value });
    link_value(&value, value, 1);
}


void TimerObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        if (reset.read_gate_state(input_buffer[0][n]) & GateState::just_opened) value = 0.f;

        output_buffer[0][n] = value;
        value += 1.f / get_sample_rate();
    }
}

TimerObject::TimerObject(const ArgumentList&)
{
    set_io(1, 1);
}


void ClockObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        
        output_buffer[0][n] = 0;

        if (elapsed >= interval) elapsed = 0.f;
        if (!elapsed) output_buffer[0][n] = 1;
        elapsed += 1.f;
    }
}

ClockObject::ClockObject(const ArgumentList& parameters)
{
    init(1, 1, parameters, { &interval });
    link_value(&interval, interval, 0);
}


void DivisionObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        output_buffer[0][n] = input_buffer[0][n] / divisor;
    }
}

DivisionObject::DivisionObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &divisor });
    link_value(&divisor, divisor, 1);
}


void SubtractionObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        output_buffer[0][n] = input_buffer[0][n] - subtrahend;
    }
}

SubtractionObject::SubtractionObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &subtrahend });
    link_value(&subtrahend, subtrahend, 1);
}


void ModuloObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        output_buffer[0][n] = std::fmod(input_buffer[0][n], divisor);
    }
}

ModuloObject::ModuloObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &divisor });
    link_value(&divisor, divisor, 1);
}


void AbsoluteValueObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = std::fabs(input_buffer[0][n]);
}

AbsoluteValueObject::AbsoluteValueObject(const ArgumentList&)
{
    set_io(1, 1);
}


void StepSequence::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        if (step.read_gate_state(input_buffer[0][n]) & GateState::just_opened) {
            current++;
            current %= sequence.size();
        }
        output_buffer[0][n] = sequence[current];
    }
}

StepSequence::StepSequence(const ArgumentList& parameters)
{
    set_io(1, 1);
    sequence = parameters[0].get_value<Sequence>();
}


void PowerObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        output_buffer[0][n] = std::pow(input_buffer[0][n], exponent);
    }
}

PowerObject::PowerObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &exponent });
    link_value(&exponent, exponent, 1);
}




void EnvelopeObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        if (trigger.read_gate_state(input_buffer[0][n]) & GateState::just_opened) time = 0;
        if (time > length) time = (int) length;
        if (length == 0.f) length = std::numeric_limits<float>::min();

        const float ratio = float(time) / length;
        output_buffer[0][n] = (1-ratio) * start + ratio * end;
        time++;
    }
}

EnvelopeObject::EnvelopeObject(const ArgumentList& parameters)
{
    init(4, 1, parameters, { &length, &start, &end });
    link_value(&length, length, 1);
    link_value(&start, start, 2);
    link_value(&end, end, 3);
    time = length;
}

void RoundObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = std::round(input_buffer[0][n]);
}

RoundObject::RoundObject(const ArgumentList&)
{
    set_io(1, 1);
}


void SequenceObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        float index = std::max(0.f, input_buffer[0][n]);
        if (index >= sequence.size()) index = sequence.size() - 1;

        const float lower = sequence[std::floor(index)];
        const float upper = sequence[(size_t) std::ceil(index) % sequence.size()];
        const float ratio = index - std::floor(index);

        output_buffer[0][n] = (1-ratio) * lower + ratio * upper;
    }
}

SequenceObject::SequenceObject(const ArgumentList& parameters)
{
    set_io(1, 1);
    sequence = parameters[0].get_value<Sequence>();
}


void SampleAndHoldObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        if (trigger.read_gate_state(input_buffer[1][n]) & GateState::just_opened) value = input_buffer[0][n];
        output_buffer[0][n] = value;
    }
}

SampleAndHoldObject::SampleAndHoldObject(const ArgumentList&)
{
    set_io(2, 1);
}


void ConstObject::process(const MultichannelBuffer&, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = value;
}

ConstObject::ConstObject(const ArgumentList& parameters)
{
    value = parameters[0].get_value<Number>();
    set_io(0, 1);
}


void SawObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        if (sync.read_gate_state(input_buffer[1][n]) & GateState::just_opened) phase = -1;

        phase += std::abs(2.f * frequency / get_sample_rate());
        if (phase > 1.f) phase = -1.f;

        if (frequency < 0) output_buffer[0][n] = -phase;
        else output_buffer[0][n] = phase;
    }
}

SawObject::SawObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency });
    link_value(&frequency, frequency, 0);
}

void TriangleObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        if (sync.read_gate_state(input_buffer[1][n]) & GateState::just_opened) phase = 0;

        phase += frequency / get_sample_rate();
        if (phase >= 1.f) phase -= 1.f;
        output_buffer[0][n] = 2.f * fabs(2.f * phase - 1.f) - 1.f;
    }
}

TriangleObject::TriangleObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &frequency });
    link_value(&frequency, frequency, 0);
}

void BiquadObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        if (!resonance) resonance = std::numeric_limits<float>::min();
        omega = TAU * frequency / get_sample_rate();
        alpha = std::sin(omega) / (2.f * resonance);
        cos_omega = std::cos(omega);

        calculate_coefficients();

        x[0] = input_buffer[0][n];
        output_buffer[0][n] = y[0] = (b0*x[0] + b1*x[-1] + b2*x[-2] - a1*y[-1] - a2*y[-2]) / a0;

        x.increment_pointer();
        y.increment_pointer();
    }
}

BiquadObject::BiquadObject(const ArgumentList& parameters)
: x(4), y(4)
{
    init(3, 1, parameters, { &frequency, &resonance });
    link_value(&frequency, frequency, 1);
    link_value(&resonance, resonance, 2);
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
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        const float sample = std::fabs(input_buffer[0][n]);

        const float internal_attack = std::exp(time_constant / attack);
        const float internal_release = std::exp(time_constant / release);

        float detector_value = 0.f;
        if (sample > last_value)
            detector_value = internal_attack * (last_value - sample) + sample;
        else
            detector_value = internal_release * (last_value - sample) + sample;

        if (detector_value < 0.f) detector_value = 0.f;

        last_value = detector_value;
        output_buffer[0][n] = detector_value;
    }
}

EnvelopeFollowerObject::EnvelopeFollowerObject(const ArgumentList& parameters)
{
    init(3, 1, parameters, { &attack, &release });
    link_value(&attack, attack, 1);
    link_value(&release, release, 2);
}

void SubgraphObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    output_buffer = graph->run(input_buffer);
}

SubgraphObject::SubgraphObject(const ArgumentList& parameters)
{
    const auto num_inputs  = (uint) parameters[0].get_value<Number>();
    const auto num_outputs = (uint) parameters[1].get_value<Number>();
    set_io(num_inputs, num_outputs);
}

void ConvolveObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        signal[0] = input_buffer[0][n];
        
        float value = 0;
        for (size_t i = 0; i < impulse_response.size(); i++) {
            value += impulse_response[i] * signal[(long) i-1];
        }
        output_buffer[0][n] = value;
        signal.increment_pointer();
    }
}

ConvolveObject::ConvolveObject(const ArgumentList& parameters)
{
    set_io(1, 1);
    impulse_response = parameters[0].get_value<Sequence>();
    signal.resize_stream(impulse_response.size());
}


void PoleObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        if (position.imag() == 0.f) {
            a2 = 0;
            a1 = -position.real();
        }
        else {
            a1 = -2.f * position.magnitude() * std::cos(position.angle());
            a2 = position.magnitude() * position.magnitude();
        }

        output_buffer[0][n] = y[0] = input_buffer[0][n] - a1*y[-1] - a2*y[-2];
        y.increment_pointer();
    }
}

PoleObject::PoleObject(const ArgumentList& parameters)
{
    set_io(3, 1);
    if (parameters.size()) position = parameters[0].get_value<Number>();

    link_value(&position.real(), position.real(), 1);
    link_value(&position.imag(), position.imag(), 2);
    y.resize_stream(4);
}


void ZeroObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);

        b1 = -2.f * position.magnitude() * std::cos(position.angle());
        b2 = position.magnitude() * position.magnitude();
        
        x[0] = input_buffer[0][n];
        output_buffer[0][n] = x[0] + b1*x[-1] + b2*x[-2];
        x.increment_pointer();
    }
}

ZeroObject::ZeroObject(const ArgumentList& parameters)
{
    set_io(3, 1);
    if (parameters.size()) position = parameters[0].get_value<Number>();

    link_value(&position.real(), position.real(), 1);
    link_value(&position.imag(), position.imag(), 2);
    x.resize_stream(4);
}

void BiToUnipolarObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = 0.5f + 0.5f * input_buffer[0][n];
}

BiToUnipolarObject::BiToUnipolarObject(const ArgumentList&)
{
    set_io(1, 1);
}





void CeilObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = std::ceil(input_buffer[0][n]);
}

CeilObject::CeilObject(const ArgumentList&)
{
    set_io(1, 1);
}

void SinObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = std::sin(input_buffer[0][n]);
}

SinObject::SinObject(const ArgumentList&)
{
    set_io(1, 1);
}

void CosObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = std::cos(input_buffer[0][n]);
}

CosObject::CosObject(const ArgumentList&)
{
    set_io(1, 1);
}

void ClampObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        output_buffer[0][n] = std::clamp(input_buffer[0][n], min, max);
    }
}

ClampObject::ClampObject(const ArgumentList& parameters)
{
    init(3, 1, parameters, { &min, &max });
    link_value(&min, min, 1);
    link_value(&max, max, 2);
}


void ReciprocalObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = 1.f / input_buffer[0][n];
}

ReciprocalObject::ReciprocalObject(const ArgumentList&)
{
    set_io(1, 1);
}

void InverseObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++)
        output_buffer[0][n] = -input_buffer[0][n];
}

InverseObject::InverseObject(const ArgumentList&)
{
    set_io(1, 1);
}






void SignObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        output_buffer[0][n] = input_buffer[0][n] >= 0 ? 1 : -1;
    }
}

SignObject::SignObject(const ArgumentList&)
{
    set_io(1, 1);
}

void LogarithmObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        output_buffer[0][n] = std::log(input_buffer[0][n]) / std::log(base);
    }
}

LogarithmObject::LogarithmObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &base });
    link_value(&base, base, 1);
}

void ExponentialObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        output_buffer[0][n] = std::pow(base, input_buffer[0][n]);
    }
}

ExponentialObject::ExponentialObject(const ArgumentList& parameters)
{
    init(2, 1, parameters, { &base });
    link_value(&base, base, 1);
}

void AtanObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        output_buffer[0][n] = std::atan(input_buffer[0][n]);
    }
}

AtanObject::AtanObject(const ArgumentList&)
{
    set_io(1, 1);
}



void PhasorObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        update_parameters(n);
        if (sync.read_gate_state(input_buffer[1][n]) & GateState::just_opened)
            phase = 0;

        output_buffer[0][n] = phase + phase_offset;
        phase += 1.0 / period;
        if (phase >= 1.0) { phase -= 1.0; }
    }
}

PhasorObject::PhasorObject(const ArgumentList& parameters) :  phase(0)
{
    init(3, 1, parameters, { &period, &phase_offset } );
    link_value(&period, period, 0);
    link_value(&phase_offset, phase_offset, 2);
}



void InvokeObject::process(const MultichannelBuffer& input_buffer, MultichannelBuffer& output_buffer)
{
    for (size_t n = 0; n < AudioBuffer::blocksize; n++) {
        output_buffer[0][n] = function({ (Number) input_buffer[0][n] }, nullptr).get_value<Number>();
    }
}

InvokeObject::InvokeObject(const ArgumentList& parameters)
{
    set_io(1, 1);
    function = parameters[0].get_value<Procedure>().implementation;
}

}
