
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
    return value;
}

Number::operator float() const
{
    return value;
}

Number::operator Text() const
{
    return (Text) std::to_string(value);
}

Number::Number(float initial_value) : value(initial_value) {}


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

void Sequence::perform_range_check(const std::size_t n) const
{
    if (n >= size()) error("Sequence index out of range. Index is: " + std::to_string(n) + ", length is: " + std::to_string(size()));
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

void TypedValue::operator+=(const TypedValue& other)
{
    if (is_type<Text>() && other.is_type<Number>())
        *this = get_value<Text>() + (Text) other.get_value<Number>();

    else if (is_type<Number>() && other.is_type<Text>())
        *this = (Text) std::to_string(get_value<Number>()) + other.get_value<Text>();

    else if (is_type<Sequence>() && other.is_type<Text>())
        *this = (Text) get_value<Sequence>() + other.get_value<Text>();

    else if (is_type<Text>() && other.is_type<Sequence>())
        *this = get_value<Text>() + (Text) other.get_value<Sequence>();

    else if (is_type<Number>() && other.is_type<Number>())
        *this = TypedValue(get_value<Number>() + other.get_value<Number>());

    else if (is_type<Sequence>() && other.is_type<Number>()) {
        for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] += other.get_value<Number>();
    }

    else if (is_type<Number>() && other.is_type<Sequence>()) {
        Sequence sequence = other.get_value<Sequence>();

        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++) {
            sequence[n] = get_value<Number>() + sequence[n];
        }

        *this = sequence;
    }

    else if (is_type<Sequence>() && other.is_type<Sequence>()) {
        if (get_value<Sequence>().size() != other.get_value<Sequence>().size()) error("Attempted to sum sequences of inequal size");
        for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] += other.get_value<Sequence>()[n];
    }

    else error("Invalid arguments on + operator");
}

void TypedValue::operator-=(const TypedValue& other)
{
    if (is_type<Number>() && other.is_type<Number>())
        *this = TypedValue(get_value<Number>() - other.get_value<Number>());

    else if (is_type<Sequence>() && other.is_type<Number>()) {
        for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] -= other.get_value<Number>();
    }

    else if (is_type<Number>() && other.is_type<Sequence>()) {
        Sequence sequence = other.get_value<Sequence>();

        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++) {
            sequence[n] = get_value<Number>() - sequence[n];
        }

        *this = sequence;
    }

    else if (is_type<Sequence>() && other.is_type<Sequence>()) {
        assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to subtract sequences of inequal size");
        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] = other.get_value<Sequence>()[n];
    }

    else error("Invalid argument on - operator");
}

/*
template <typename Callable>
void visit_typed_value(Callable function, TypedValue& value)
{
    Type type_id = value.get_type();
    if (value.is_type<Number>())    function(value.get_value<Number>());
    if (value.is_type<Text>())      function(value.get_value<Text>());
    if (value.is_type<Sequence>())  function(value.get_value<Sequence>());
}

void TypedValue::operator-=(const TypedValue& other)
{
    std::visit([&other] (auto&& value) {
        std::visit([&value] (auto&& other) { value.subtract(other); }, other);
    }, *this);
}
*/


void TypedValue::operator*=(const TypedValue& other)
{
    if (is_type<Number>() && other.is_type<Number>())
        *this = TypedValue(get_value<Number>() * other.get_value<Number>());

    else if (is_type<Sequence>() && other.is_type<Number>()) {
        for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] *= other.get_value<Number>();
    }

    else if (is_type<Number>() && other.is_type<Sequence>()) {
        Sequence sequence = other.get_value<Sequence>();

        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++) {
            sequence[n] = get_value<Number>() * sequence[n];
        }

        *this = sequence;
    }

    else if (is_type<Sequence>() && other.is_type<Sequence>()) {
        assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to multiply sequences of inequal size");
        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] *= other.get_value<Sequence>()[n];
    }

    else error("Invalid arguments on * operator");
}

void TypedValue::operator/=(const TypedValue& other)
{
    if (is_type<Number>() && other.is_type<Number>())
        *this = TypedValue(get_value<Number>() / other.get_value<Number>());

    else if (is_type<Sequence>() && other.is_type<Number>()) {
        for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] /= other.get_value<Number>();
    }

    else if (is_type<Number>() && other.is_type<Sequence>()) {
        Sequence sequence = other.get_value<Sequence>();
        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++) {
            sequence[n] = get_value<Number>() / sequence[n];
        }
        *this = sequence;
    }

    else if (is_type<Sequence>() && other.is_type<Sequence>()) {
        assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to divide sequences of inequal size");
        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] /= other.get_value<Sequence>()[n];
    }

    else error("Invalid argument on / operator");
}

void TypedValue::operator^=(const TypedValue& other)
{
    if (is_type<Number>() && other.is_type<Number>())
        *this = std::pow(get_value<Number>(), other.get_value<Number>());

    else if (is_type<Sequence>() && other.is_type<Number>()) {
        for (std::size_t n = 0; n < get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] = std::pow(get_value<Sequence>()[n], other.get_value<Number>());
    }

    else if (is_type<Number>() && other.is_type<Sequence>()) {
        Sequence sequence = other.get_value<Sequence>();
        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++) {
            sequence[n] = std::pow(get_value<Number>(), sequence[n]);
        }
        *this = sequence;
    }

    else if (is_type<Sequence>() && other.is_type<Sequence>()) {
        assert(get_value<Sequence>().size() == other.get_value<Sequence>().size(), "Attempted to exponentiate sequences of inequal size");
        for (std::size_t n = 0; n < other.get_value<Sequence>().size(); n++)
            get_value<Sequence>()[n] = std::pow(other.get_value<Sequence>()[n],
                                                      get_value<Sequence>()[n]);
    }

    else error("Invalid argument on ^ operator");
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


void Program::create_user_object(const std::string& name, const uint inputs, const uint outputs, std::any user_data, const CallbackFunctor callback)
{
    if (table.count(name) != 0) error("Symbol '" + name + "' is already used");
    table[name] = std::make_unique<UserObject, std::vector<TypedValue>, const CallbackFunctor&, std::any&>({ TypedValue(inputs), TypedValue(outputs) }, callback, user_data);
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
        object->data = { };
        for (std::size_t n = 0; n < inputs; n++) object->data.push_back(sample.at(n));
    }
    
    simulate();
    Frame out;

    if (outputs) {
        AudioOutputObject* object = get_audio_object_raw_pointer<AudioOutputObject>("output");
        for (std::size_t n = 0; n < outputs; n++) out.push_back(object->data[n]);
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

void Program::add_directive(const std::string& name, const DirectiveFunctor function)
{
    if (!custom_directives.count(name))
        custom_directives[name] = function;
}

void Program::invoke_directive(const std::string& name, const std::vector<TypedValue>& arguments)
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
    if (identifier == "r") error("Name 'r' is reserved");
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
    float r = distribution(generator);
    if (identifier == "r") return r;

    return symbol_table.at(identifier);
}

}
