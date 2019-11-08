
#include <vector>
#include <string>
#include <iostream>

#include "AudioObject.hh"

namespace Volsung {

void AudioObject::finish() { }

void AudioObject::implement()
{
	for (uint n = 0; n < inputs.size(); n++)
	{
		float value = inputs[n].read_value();
		in[n][0] = value;
		in[n].increment_pointer();
	}

	for (auto const& value : linked_values)
		if (inputs[value.input].is_connected())
			*value.parameter = in[value.input][0];
	run(in, out);

	for (uint n = 0; n < outputs.size(); n++)
	{
		float value = out[n][0];
		outputs[n].write_value(value);
		out[n].increment_pointer();
	}
}

bool AudioObject::is_connected(uint in)
{
	return inputs[in].is_connected();
}

void AudioObject::set_io(int num_inputs, int num_outputs)
{
	outputs.resize(num_outputs);
	inputs.resize(num_inputs);

	out.resize(num_outputs);
	in.resize(num_inputs);
}

bool AudioObject::verify_argument_types(std::vector<Type> recieved, std::vector<TypedValue> expected)
{
	bool success = true;
	for (uint n = 0; n < expected.size(); n++) {
		Type value = recieved[n];
		switch (value) {
			case(Type::number): success &= std::holds_alternative<float>(expected[n]); break;
			case(Type::sequence): success &= std::holds_alternative<Sequence>(expected[n]); break;
			case(Type::string): success &= std::holds_alternative<std::string>(expected[n]); break;
		}
	}
	if (!success) log("Invalid argument type on object");
	return success;
}

void AudioObject::init(int ins, int outs, std::vector<TypedValue> arguments, std::vector<float*> values)
{
	set_io(ins, outs);

	for (uint n = 0; n < values.size(); n++) {
		*values[n] = std::get<float>(arguments[n]);
	}
}

void AudioObject::request_buffer_size(int count)
{
	for (auto& buffer : in) buffer.resize_stream(count);
	for (auto& buffer : out) buffer.resize_stream(count);
	buffer_size = count;
}

void AudioObject::set_defval(float* parameter, float default_value, int input)
{
	linked_values.push_back(linked_value(parameter, default_value, input));
}

bool AudioObject::is_gate_high(uint input)
{
	if (in[input][0] >= AudioObject::gate_threshold) return true;
	return false;
}

bool AudioObject::gate_opened(uint input)
{
	if (in[input][0] >= AudioObject::gate_threshold &&
		in[input][-1] < AudioObject::gate_threshold) return true;
	return false;
}

bool AudioObject::gate_closed(uint input)
{
	if (in[input][0] <   AudioObject::gate_threshold &&
		in[input][-1] >= AudioObject::gate_threshold) return true;
	return false;
}

}
