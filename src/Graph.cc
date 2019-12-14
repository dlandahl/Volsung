
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>
#include <map>

#include "Volsung.hh"
#include "Graph.hh"
#include "Objects.hh"

namespace Volsung {

Type TypedValue::get_type()
{
	if (is_type<Number>()) return Type::number;
	if (is_type<Sequence>()) return Type::sequence;
	return Type::text;
}

void TypedValue::operator+=(TypedValue other)
{
	if (is_type<Text>() && other.is_type<Number>())
		*this = get_value<Text>() + std::to_string((int) other.get_value<Number>());

	else if (is_type<Number>() && other.is_type<Text>())
		*this = std::to_string((int) get_value<Number>()) + other.get_value<Text>();

	else if (is_type<Sequence>() && other.is_type<Text>())
		*this = std::string(get_value<Sequence>()) + other.get_value<Text>();

	else if (is_type<Text>() && other.is_type<Sequence>())
		*this = get_value<Text>() + std::string(other.get_value<Sequence>());

	else if (is_type<Number>() && other.is_type<Number>())
		*this = TypedValue(get_value<Number>() + other.get_value<Number>());

	else if (is_type<Sequence>() && other.is_type<Number>()) {
		for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] += other.get_value<Number>();
	}

	else if (is_type<Sequence>() && other.is_type<Sequence>()) {
		for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] += other.get_value<Sequence>().data[n];
	}

	else error("Invalid arguments on + operator");
}

void TypedValue::operator-=(TypedValue other)
{
	if (is_type<Number>() && other.is_type<Number>())
		*this = TypedValue(get_value<Number>() - other.get_value<Number>());

	else if (is_type<Sequence>() && other.is_type<Number>()) {
		for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] -= other.get_value<Number>();
	}
	else if (is_type<Number>() && other.is_type<Sequence>()) {
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			other.get_value<Sequence>().data[n] = get_value<Number>() - other.get_value<Sequence>().data[n];
		*this = other;
	}
	else if (is_type<Sequence>() && other.is_type<Sequence>()) {
		assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to subtract sequences of inequal size");
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] = other.get_value<Sequence>().data[n];
	}
	else error("Invalid argument on - operator");
}

void TypedValue::operator*=(TypedValue other)
{
	if (is_type<Number>() && other.is_type<Number>())
		*this = TypedValue(get_value<Number>() * other.get_value<Number>());

	else if (is_type<Sequence>() && other.is_type<Number>()) {
		for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] *= other.get_value<Number>();
	}
	else if (is_type<Number>() && other.is_type<Sequence>()) {
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			other.get_value<Sequence>().data[n] = get_value<Number>() * other.get_value<Sequence>().data[n];
		*this = other;
	}
	else if (is_type<Sequence>() && other.is_type<Sequence>()) {
		assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to multiply sequences of inequal size");
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] *= other.get_value<Sequence>().data[n];
	}
	else error("Invalid arguments on * operator");
}

void TypedValue::operator/=(TypedValue other)
{
	if (is_type<Number>() && other.is_type<Number>())
		*this = TypedValue(get_value<Number>() / other.get_value<Number>());

	else if (is_type<Sequence>() && other.is_type<Number>()) {
		for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] /= other.get_value<Number>();
	}
	else if (is_type<Number>() && other.is_type<Sequence>()) {
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			other.get_value<Sequence>().data[n] = get_value<Number>() / other.get_value<Sequence>().data[n];
		*this = other;
	}
	else if (is_type<Sequence>() && other.is_type<Sequence>()) {
		assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to divide sequences of inequal size");
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] /= other.get_value<Sequence>().data[n];
	}
	else error("Invalid argument on / operator");
}

void TypedValue::operator^=(TypedValue other)
{
	if (is_type<Number>() && other.is_type<Number>())
		*this = std::pow(get_value<Number>(), other.get_value<Number>());

	else if (is_type<Sequence>() && other.is_type<Number>()) {
		for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] = std::pow(get_value<Sequence>().data[n], other.get_value<Number>());

	}
	else if (is_type<Number>() && other.is_type<Sequence>()) {
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			other.get_value<Sequence>().data[n] = std::pow(get_value<Number>(), other.get_value<Sequence>().data[n]);
		*this = other;
	}
	else if (is_type<Sequence>() && other.is_type<Sequence>()) {
		assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to exponentiate sequences of inequal size");
		for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
			get_value<Sequence>().data[n] = std::pow(other.get_value<Sequence>().data[n],
			                                         get_value<Sequence>().data[n]);
	}
	else error("Invalid argument on ^ operator");
}

TypedValue TypedValue::operator-()
{
	switch(get_type()) {
		case(Type::number): *this = -this->get_value<Number>(); break;
		case(Type::sequence): for (auto& value: this->get_value<Sequence>().data) value = -value; break;
		case(Type::text): error("Attempted to negate expression of type string");
	}
	return *this;
}

void Program::create_user_object(std::string name, uint inputs, uint outputs, std::any user_data, callback_functor callback)
{
	create_object<UserObject>(name, { TypedValue(inputs), TypedValue(outputs) });

	UserObject* object = get_audio_object_raw_pointer<UserObject>(name);
	object->callback = callback;
	object->user_data = user_data;
}

void Program::connect_objects(
	std::unique_ptr<AudioObject> &a, uint out,
    std::unique_ptr<AudioObject> &b, uint in)
{
	if (a->outputs.size() > out && b->inputs.size() > in)
		a->outputs[out].connect(b->inputs[in]);
	else error("Index out of range for connection");

}

void Program::expect_to_be_group(std::string name)
{
	if (table.count(name)) error(name + " is an object, not a group");
	else if (!group_sizes.count(name)) error("Group " + name + " has not been declared");
}

void Program::expect_to_be_object(std::string name)
{
	if (group_sizes.count(name)) error(name + " is a group, not an object");
	else if (!table.count(name)) error("Object " + name + " has not been declared");
}

void Program::connect_objects(std::string a, uint out,
	                          std::string b, uint in, ConnectionType type)
{
	if (type == ConnectionType::one_to_one) {
		expect_to_be_object(a);
		expect_to_be_object(b);
		connect_objects(table[a], out, table[b], in);
	}
	
	else if (type == ConnectionType::many_to_one) {
		expect_to_be_group(a);
		expect_to_be_object(b);
		for (std::size_t n = 0; n < group_sizes[a]; n++)
			connect_objects(table["__grp_" + a + std::to_string(n)], out, table[b], in);
	}
	
	else if (type == ConnectionType::one_to_many) {
		expect_to_be_object(a);
		expect_to_be_group(b);
		for (std::size_t n = 0; n < group_sizes[b]; n++)
			connect_objects(table[a], out, table["__grp_" + b + std::to_string(n)], in);
	}

	else if (type == ConnectionType::cross) {
		expect_to_be_group(a);
		expect_to_be_group(b);
		for (std::size_t na = 0; na < group_sizes[a]; na++) {
			for (std::size_t nb = 0; nb < group_sizes[b]; nb++) {
				connect_objects(table["__grp_" + a + std::to_string(na)], out,
								table["__grp_" + b + std::to_string(nb)], in);
			}
		}
	}
	
	else if (type == ConnectionType::many_to_many) {
		expect_to_be_group(a);
		expect_to_be_group(b);
		if (group_sizes[a] != group_sizes[b]) error("Group sizes to be connected in parallel are not identical");
		for (std::size_t n = 0; n < group_sizes[a]; n++)
			connect_objects(table["__grp_" + a + std::to_string(n)], out,
			                table["__grp_" + b + std::to_string(n)], in);
	}
}

bool Program::object_exists(std::string name)
{
	if (table.count(name) || group_sizes.count(name)) return true;
	return false;
}

void Program::run()
{
	for (auto const& entry : table) {
		entry.second->implement();
	}
}

float Program::run(float sample)
{
	if (inputs) {
		AudioInputObject* object = get_audio_object_raw_pointer<AudioInputObject>("input");
		object->data = { 0 };
		object->data[0] = sample;
	}
	
	run();
	float out = 0;
	
	if (outputs) {
		AudioOutputObject* object = get_audio_object_raw_pointer<AudioOutputObject>("output");
		out = object->data[0];
	}
	return out;
}

Frame Program::run(Frame sample)
{
	if (inputs) {
		AudioInputObject* object = get_audio_object_raw_pointer<AudioInputObject>("input");
		object->data = { };
		for (int n = 0; n < inputs; n++) object->data.push_back(sample[n]);
	}
	
	run();
	Frame out;

	
	if (outputs) {
		AudioOutputObject* object = get_audio_object_raw_pointer<AudioOutputObject>("output");
		for (int n = 0; n < outputs; n++) out.push_back(object->data[n]);
		object->data = { };
	}
	return out;
}


void Program::finish()
{
	for (auto const& entry : table)
		entry.second->finish();
}

void Program::reset()
{
	table.clear();
	symbol_table.clear();
	group_sizes.clear();
	
	if (inputs) create_object<AudioInputObject>("input", { inputs });
	if (outputs) create_object<AudioOutputObject>("output", { outputs });
}

void Program::add_directive(std::string name, directive_functor function)
{
	if (!custom_directives.count(name))
		custom_directives[name] = function;
}

void Program::invoke_directive(std::string name, std::vector<TypedValue> arguments)
{
	if (!custom_directives.count(name)) error("Unknown directive");

	custom_directives[name](arguments, this);
}

void Program::configure_io(uint i, uint o)
{
	inputs = i;
	outputs = o;
}

void Program::add_symbol(std::string identifier, TypedValue value)
{
	if (identifier == "r") error("Name 'r' is reserved");
	symbol_table[identifier] = value;
}

void Program::remove_symbol(std::string identifier)
{
	if (symbol_exists(identifier)) symbol_table.erase(identifier);
}

bool Program::symbol_exists(std::string identifier)
{
	if (identifier == "r") return true;
	return symbol_table.count(identifier) == 1;
}

TypedValue Program::get_symbol_value(std::string identifier)
{
	float r = distribution(generator);
	if (identifier == "r") return r;

	return symbol_table[identifier];
}

}
