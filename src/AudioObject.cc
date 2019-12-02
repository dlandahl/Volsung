
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
		in[n][0] = inputs[n].read_value();
		in[n].increment_pointer();
	}

	for (auto const& value : linked_values) {
		if (inputs[value.input].is_connected())
			*value.parameter = in[value.input][0];
		else *value.parameter = value.default_value;
	}

	run(in, out);

	for (uint n = 0; n < outputs.size(); n++)
	{
		outputs[n].write_value(out[n][0]);
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
			case(Type::number): success &= expected[n].is_type<float>(); break;
			case(Type::sequence): success &= expected[n].is_type<Sequence>(); break;
			case(Type::string): success &= expected[n].is_type<std::string>(); break;
		}
	}
	if (!success) log("Invalid argument type on object");
	return success;
}

void AudioObject::init(int ins, int outs, std::vector<TypedValue> arguments, std::vector<float*> values)
{
	set_io(ins, outs);

	for (uint n = 0; n < arguments.size() && n < values.size(); n++) {
		*values[n] = arguments[n].get_value<float>();
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
