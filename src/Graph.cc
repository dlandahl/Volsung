
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>
#include <map>

#include "Graph.hh"
#include "Objects.hh"

namespace Volsung {

Number::operator float&()
{
    return real_part;
}

Number::operator float() const
{
    return real_part;
}

Number::operator Text() const
{
    std::string ret = std::to_string(real_part);
    if (imag_part) ret += " + " + std::to_string(imag_part) + "i";
    return (Text) ret;
}

bool Number::is_complex() const
{
    return (bool) imag_part;
}

Number::Number(float initial_value) : real_part(initial_value) {}

Number::Number(float initial_real_part, float initial_imag_part)
    : real_part(initial_real_part),
      imag_part(initial_imag_part) {}


#define DEFINE_ARITHMETIC_OPERATION_ON_NUMBER(op)                               \
TypedValue Number::op(const TypedValue& other)                                  \
{                                                                               \
    switch (other.get_type()) {                                                 \
        case (Type::number): return op##_num(other.get_value<Number>());        \
        case (Type::sequence): {                                                \
            Sequence seq = other.get_value<Sequence>();                         \
            for (auto& element: seq) element = op##_num(element);               \
            return seq;                                                         \
        }                                                                       \
        case (Type::text): error("Cannot perform arithmetic on expression of type 'Text'");       \
    }                                                                           \
    return TypedValue(0);                                                       \
}                                                                               \

DEFINE_ARITHMETIC_OPERATION_ON_NUMBER(add)
DEFINE_ARITHMETIC_OPERATION_ON_NUMBER(subtract)
DEFINE_ARITHMETIC_OPERATION_ON_NUMBER(multiply)
DEFINE_ARITHMETIC_OPERATION_ON_NUMBER(divide)
DEFINE_ARITHMETIC_OPERATION_ON_NUMBER(exponentiate)
#undef DEFINE_ARITHMETIC_OPERATION_ON_NUMBER

#define DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE(op)                             \
TypedValue Sequence::op(const TypedValue& other)                                \
{                                                                               \
    switch (other.get_type()) {                                                 \
        case (Type::number): {                                                  \
            const Number value = other.get_value<Number>();                     \
            for (auto& element: *this) element = element.op##_num(value);       \
            return *this;                                                       \
        }                                                                       \
        case (Type::sequence): {                                                \
            Sequence seq = other.get_value<Sequence>();                         \
            for (std::size_t n = 0; n < size(); n++)                            \
                seq[n] = data[n].op##_num(seq[n]);                              \
            return seq;                                                         \
        }                                                                       \
        case (Type::text): error("Cannot perform arithmetic on expression of type 'Text'");       \
    }                                                                           \
    return TypedValue(0);                                                       \
}                                                                               \

DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE(add)
DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE(subtract)
DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE(multiply)
DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE(divide)
DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE(exponentiate)
#undef DEFINE_ARITHMETIC_OPERATION_ON_SEQUENCE


Number Number::add_num(const Number& other) const
{
    return Number(real_part + other.real_part,
           imag_part + other.imag_part);
}

Number Number::subtract_num(const Number& other) const
{
    return Number(real_part - other.real_part,
           imag_part - other.imag_part);
}

Number Number::multiply_num(const Number& other) const
{
    const float new_real_part = real_part * other.real_part - imag_part * other.imag_part;
    const float new_imag_part = imag_part * other.real_part + real_part * other.imag_part;
    return Number (new_real_part, new_imag_part);
}

Number Number::divide_num(const Number& other) const
{
    const Number conjugate = Number(real_part, -imag_part);
    const Number denominator = other.multiply_num(conjugate);
    const Number inter = multiply_num(conjugate);
    return Number(inter.real_part / denominator.real_part, inter.imag_part / denominator.real_part);
}

Number Number::exponentiate_num(const Number& other) const
{
    return (Number) std::pow(real_part, other.real_part);
}

std::size_t Sequence::size() const
{
    return data.size();
}

Sequence::operator Text() const
{
    std::string string = "{ ";

    string += std::to_string((float) data[0]);
    for (std::size_t n = 1; n < size(); n++) string += ", " + std::to_string((float) data[n]);
    string += " }\n";

    return Text(string);
}

void Sequence::add_element(const Number value)
{
    data.push_back(value);
}

void Sequence::perform_range_check(const long long n) const
{
    if (n >= long(size()) || n < -long(size()))
        error("Sequence index out of range. Index is: " + std::to_string(n) + ", length is: " + std::to_string(size()));
}

Number& Sequence::operator[](long long n)
{
    if (n < 0) n += size();
    perform_range_check(n);
    return data[n];
}

const Number& Sequence::operator[](long long n) const
{
    if (n < 0) n += size();
    perform_range_check(n);
    return data.at(n);
}

Type TypedValue::get_type() const
{
    if (is_type<Number>()) return Type::number;
    if (is_type<Sequence>()) return Type::sequence;
    return Type::text;
}

std::string TypedValue::as_string() const
{
    switch(get_type()) {
        case(Type::number): return (Text) get_value<Number>();
        case(Type::sequence): return (Text) get_value<Sequence>();
        case(Type::text): return get_value<Text>();
    }
    return "";
}

void TypedValue::operator+=(const TypedValue& other)
{
    switch(get_type()) {
        case(Type::number): *this = get_value<Number>().add(other); break;
        case(Type::sequence): *this = get_value<Sequence>().add(other); break;
        case(Type::text): error("Attempted to exponentiate expression of type string");
    }
}

void TypedValue::operator-=(const TypedValue& other)
{
    switch(get_type()) {
        case(Type::number): *this = get_value<Number>().subtract(other); break;
        case(Type::sequence): *this = get_value<Sequence>().subtract(other); break;
        case(Type::text): error("Attempted to exponentiate expression of type string");
    }
}

void TypedValue::operator*=(const TypedValue& other)
{
    switch(get_type()) {
        case(Type::number): *this = get_value<Number>().multiply(other); break;
        case(Type::sequence): *this = get_value<Sequence>().multiply(other); break;
        case(Type::text): error("Attempted to exponentiate expression of type string");
    }
}

void TypedValue::operator/=(const TypedValue& other)
{
    switch(get_type()) {
        case(Type::number): *this = get_value<Number>().divide(other); break;
        case(Type::sequence): *this = get_value<Sequence>().divide(other); break;
        case(Type::text): error("Attempted to exponentiate expression of type string");
    }
}

void TypedValue::operator^=(const TypedValue& other)
{
    switch(get_type()) {
        case(Type::number): *this = get_value<Number>().exponentiate(other); break;
        case(Type::sequence): *this = get_value<Sequence>().exponentiate(other); break;
        case(Type::text): error("Attempted to exponentiate expression of type string");
    }
}

TypedValue& TypedValue::operator-()
{
    switch(get_type()) {
        case(Type::number): *this = -this->get_value<Number>(); break;
        case(Type::sequence): for (auto& value: this->get_value<Sequence>()) value = -value; break;
        case(Type::text): error("Attempted to negate expression of type string");
    }
    return *this;
}


void Program::create_user_object(const std::string& name, const uint inputs, const uint outputs, std::any user_data, const AudioProcessingCallback callback)
{
    if (table.count(name) != 0) error("Symbol '" + name + "' is already used");
    table[name] = std::make_unique<UserObject, ArgumentList, const AudioProcessingCallback&, std::any&>({ TypedValue(inputs), TypedValue(outputs) }, callback, user_data);
}

void Program::check_io_and_connect_objects(const std::string& output_object, const uint output_index,
                                           const std::string& input_object , const uint input_index)
{

    if (table[output_object]->outputs.size() <= output_index) {
        error("Index out of range on output object '" + output_object + "'. Index is: " + std::to_string(output_index));
    }
    
    if (table[input_object]->inputs.size() <= input_index)
        error("Index out of range on input object '" + input_object + "'. Index is: " + std::to_string(input_index));

    table[output_object]->outputs[output_index].connect(table[input_object]->inputs.at(input_index));
}

void Program::expect_to_be_group(const std::string& name) const
{
    if (table.count(name)) error(name + " is an object, not a group");
    else if (!group_sizes.count(name)) error("Group " + name + " has not been declared");
}

void Program::expect_to_be_object(const std::string& name) const
{
    if (group_sizes.count(name)) error(name + " is a group, not an object");
    else if (!table.count(name)) error("Object " + name + " has not been declared");
}

void Program::connect_objects(const std::string& output_object, const uint out,
                              const std::string& input_object, const uint in, const ConnectionType type)
{
    if (type == ConnectionType::one_to_one) {
        expect_to_be_object(output_object);
        expect_to_be_object(input_object);
        check_io_and_connect_objects(output_object, out, input_object, in);
    }
    
    else if (type == ConnectionType::many_to_one) {
        expect_to_be_group(output_object);
        expect_to_be_object(input_object);
        for (std::size_t n = 0; n < group_sizes[output_object]; n++)
            check_io_and_connect_objects("__grp_" + output_object + std::to_string(n), out, input_object, in);
    }
    
    else if (type == ConnectionType::one_to_many) {
        expect_to_be_object(output_object);
        expect_to_be_group(input_object);
        for (std::size_t n = 0; n < group_sizes[input_object]; n++)
            check_io_and_connect_objects(output_object, out, "__grp_" + input_object + std::to_string(n), in);
    }

    else if (type == ConnectionType::biclique) {
        expect_to_be_group(output_object);
        expect_to_be_group(input_object);
        for (std::size_t na = 0; na < group_sizes[output_object]; na++) {
            for (std::size_t nb = 0; nb < group_sizes[input_object]; nb++) {
                check_io_and_connect_objects("__grp_" + output_object + std::to_string(na), out,
                                "__grp_" + input_object + std::to_string(nb), in);
            }
        }
    }
    
    else if (type == ConnectionType::many_to_many) {
        expect_to_be_group(output_object);
        expect_to_be_group(input_object);
        if (group_sizes[output_object] != group_sizes[input_object]) error("Group sizes to be connected in parallel are not identical");
        for (std::size_t n = 0; n < group_sizes[output_object]; n++)
            check_io_and_connect_objects("__grp_" + output_object + std::to_string(n), out,
                            "__grp_" + input_object + std::to_string(n), in);
    }
}

bool Program::object_exists(const std::string& name) const
{
    if (table.count(name) || group_sizes.count(name)) return true;
    return false;
}

void Program::simulate()
{
    for (auto const& entry : table) {
        entry.second->implement();
    }
}

float Program::run(const float sample)
{
    return run( Frame { sample } )[0];
}

Frame Program::run(const Frame sample)
{
    if (inputs) {
        AudioInputObject* object = get_audio_object_raw_pointer<AudioInputObject>("input");
        std::fill(object->data.begin(), object->data.end(), 0.f);
        for (std::size_t n = 0; n < inputs; n++) object->data.push_back(sample.at(n));
    }
    
    simulate();
    Frame out;

    if (outputs) {
        AudioOutputObject* object = get_audio_object_raw_pointer<AudioOutputObject>("output");
        for (std::size_t n = 0; n < outputs; n++) out.push_back(object->data[n]);
        std::fill(object->data.begin(), object->data.end(), 0.f);
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

void Program::add_directive(const std::string& name, const DirectiveFunctor function)
{
    if (!custom_directives.count(name))
        custom_directives[name] = function;
}

void Program::invoke_directive(const std::string& name, const ArgumentList& arguments)
{
    if (!custom_directives.count(name)) error("Unknown directive");

    custom_directives.at(name)(arguments, this);
}

void Program::configure_io(const uint i, const uint o)
{
    inputs = i;
    outputs = o;
}

void Program::add_symbol(const std::string& identifier, const TypedValue& value)
{
    if (symbol_exists(identifier)) error("Identifier '" + identifier + "' is already in use");
    symbol_table[identifier] = value;
}

void Program::remove_symbol(const std::string& identifier)
{
    if (symbol_exists(identifier)) symbol_table.erase(identifier);
}

bool Program::symbol_exists(const std::string& identifier) const
{
    if (identifier == "r") return true;
    return symbol_table.count(identifier) == 1;
}

TypedValue Program::get_symbol_value(const std::string& identifier) const
{
    if (identifier == "r") return distribution(generator);

    return symbol_table.at(identifier);
}


}

