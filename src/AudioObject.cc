
#include <vector>
#include <string>
#include <iostream>

#include "AudioObject.hh"
#include "Graph.hh"

namespace Volsung {

void AudioObject::finish() { }

void AudioObject::implement()
{
    for (size_t n = 0; n < inputs.size(); n++)
    {
        in[n] = inputs[n].read_buffer();
    }

    process(in, out);

    for (size_t n = 0; n < outputs.size(); n++)
    {
        outputs[n].write_buffer(out[n]);
    }
}

bool AudioObject::is_connected(const uint input_index) const
{
    return inputs.at(input_index).is_connected();
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

    for (size_t n = 0; n < arguments.size() && n < values.size(); n++) {
        *values[n] = arguments[n].get_value<Number>();
    }
}

void AudioObject::link_value(float* const parameter, const float default_value, const uint input)
{
    linked_values.push_back({ parameter, default_value, input });
    *parameter = default_value;
}

void AudioObject::update_parameters(size_t n)
{
    for (auto const& value : linked_values) {
        if (inputs[value.input].is_connected())
            *value.parameter = in[value.input][n];
    }
}

bool operator& (GateState lhs, GateState rhs)
{
    using T = std::underlying_type_t<GateState>;
    return static_cast<bool> (static_cast<T> (lhs) & static_cast<T> (rhs));
}

GateState operator| (GateState lhs, GateState rhs)
{
    using T = std::underlying_type_t<GateState>;
    return static_cast<GateState> (static_cast<T> (lhs) | static_cast<T> (rhs));
}

GateState GateListener::read_gate_state(float current_value)
{
    const float _last_value = last_value;
    last_value = current_value;
    
    if (current_value >= gate_threshold && _last_value >= gate_threshold)
        return GateState::open;
        
    if (current_value <  gate_threshold && _last_value < gate_threshold)
        return GateState::closed;
        
    if (current_value >= gate_threshold && _last_value < gate_threshold)
        return GateState::just_opened | GateState::open;

    return GateState::just_closed | GateState::closed;
}

}
