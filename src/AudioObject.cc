
#include <vector>
#include <string>
#include <iostream>

#include "AudioObject.hh"
#include "Graph.hh"

namespace Volsung {

void AudioObject::finish() { }

void AudioObject::implement()
{
    for (std::size_t n = 0; n < inputs.size(); n++)
    {
        in[n][0] = inputs[n].read_value();
        in[n].increment_pointer();
    }

    for (auto const& value : linked_values) {
        if (inputs[value.input].is_connected())
            *value.parameter = in[value.input][0];
        else *value.parameter = value.default_value;
    }

    process(in, out);

    for (std::size_t n = 0; n < outputs.size(); n++)
    {
        outputs[n].write_value(out[n][0]);
        out[n].increment_pointer();
    }
}

bool AudioObject::is_connected(const uint in) const
{
    return inputs.at(in).is_connected();
}

void AudioObject::set_io(const uint num_inputs, const uint num_outputs)
{
    outputs.resize(num_outputs);
    inputs.resize(num_inputs);

    out.resize(num_outputs);
    in.resize(num_inputs);
}

void AudioObject::init(const uint ins, const uint outs, std::vector<TypedValue> arguments, std::vector<float*> values)
{
    set_io(ins, outs);

    for (std::size_t n = 0; n < arguments.size() && n < values.size(); n++) {
        *values[n] = arguments[n].get_value<Number>();
    }
}

void AudioObject::request_buffer_size(const std::size_t count)
{
    for (auto& buffer : in) buffer.resize_stream(count);
    for (auto& buffer : out) buffer.resize_stream(count);
//  buffer_size = count;
}

void AudioObject::set_defval(float* const parameter, const float default_value, const int input)
{
    linked_values.push_back(linked_value(parameter, default_value, input));
}

bool AudioObject::is_gate_high(const uint input) const
{
    if (in.at(input)[0] >= AudioObject::gate_threshold) return true;
    return false;
}

bool AudioObject::gate_opened(const uint input) const
{
    if (in.at(input)[0] >= AudioObject::gate_threshold &&
        in.at(input)[-1] < AudioObject::gate_threshold) return true;
    return false;
}

bool AudioObject::gate_closed(const uint input) const
{
    if (in.at(input)[0] <   AudioObject::gate_threshold &&
        in.at(input)[-1] >= AudioObject::gate_threshold) return true;
    return false;
}

}
