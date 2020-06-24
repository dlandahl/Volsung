
#pragma once

#include <variant>
#include <memory>
#include <istream>
#include <any>
#include <random>
#include <chrono>
#include <array>

#include "VolsungCore.hh"
#include "AudioDataflow.hh"

namespace Volsung {

enum class Type {
    number,
    text,
    sequence,
    procedure
};

inline std::string type_name(Type type)
{
    switch (type) {
        case(Type::number): return "Number"; 
        case(Type::text): return "Text";
        case(Type::sequence): return "Sequence";
        case(Type::procedure): return "Procedure";
   }
   return "";
}


enum class ConnectionType {
    one_to_one,
    one_to_many,
    many_to_one,
    many_to_many,
    series,
    biclique
};

class TypedValue;
class Text;
class Sequence;
class AudioObject;
class Program;

class Number
{
    float real_part = 0;
    float imag_part = 0;

public:
    operator float&();
    operator float() const;
    operator Text() const;

    bool is_complex() const;
    float magnitude() const;
    float angle() const;

    float& real();
    float& imag();

    Number(float);
    Number(float, float);
    Number() = default;

    TypedValue add(const TypedValue&);
    TypedValue subtract(const TypedValue&);
    TypedValue multiply(const TypedValue&);
    TypedValue divide(const TypedValue&);
    TypedValue exponentiate(const TypedValue&);

    Number negated() const;

    Number add_num(const Number&) const;
    Number subtract_num(const Number&) const;
    Number multiply_num(const Number&) const;
    Number divide_num(const Number&) const;
    Number exponentiate_num(const Number&) const;
};

class Text
{
    std::string value;
public:
    void operator=(std::string string) { value = string; }
    Text& operator+(const Text& rhs) {
        value = value + rhs.value;
        return *this;
    }

    operator std::string() const {
        return value;
    }

    Text(std::string string) : value(string) {}
    Text() = default;
};

class Sequence
{
    // const size_t identifier;
    // static inline std::vector<std::vector<Number>> sequence_table;
    // std::vector<Number>* data;
    std::vector<Number> data;

public:
    Number* get_data_pointer();
    size_t size() const;
    operator Text() const;
    void add_element(const Number);
    void perform_range_check(const long long) const;

    Number& operator[](long long);
    const Number& operator[](long long) const;

    TypedValue add(const TypedValue&);
    TypedValue subtract(const TypedValue&);
    TypedValue multiply(const TypedValue&);
    TypedValue divide(const TypedValue&);
    TypedValue exponentiate(const TypedValue&);

    auto begin() { return std::begin(data); }
    auto end() { return std::end(data); }

    Sequence() = default;
    Sequence(const std::vector<float>&);
};

using ArgumentList = std::vector<TypedValue>;
class Procedure
{
public:
    using Implementation = std::function<TypedValue(const ArgumentList&, Program*)>;
    Implementation implementation;

    size_t min_arguments;
    size_t max_arguments;
    bool can_be_mapped;

    TypedValue operator()(const ArgumentList&, Program*) const;
    Procedure(Implementation, size_t, size_t, bool = false);

    Procedure& operator=(const Procedure& proc) = default;

    Procedure(const Procedure& proc) : min_arguments(proc.min_arguments),
                                       max_arguments(proc.max_arguments),
                                       can_be_mapped(proc.can_be_mapped) {
        implementation = proc.implementation;
    }
};

using TypedValueBase = std::variant<Number, Text, Sequence, Procedure>;
class TypedValue : private TypedValueBase
{
    using TypedValueBase::TypedValueBase;
public:
    template<class T>
    T& get_value();

    template<class T>
    const T& get_value() const;

    template<class>
    bool is_type() const;

    Type get_type() const;

    std::string as_string() const;

    void operator+=(const TypedValue&);
    void operator-=(const TypedValue&);
    void operator*=(const TypedValue&);
    void operator/=(const TypedValue&);
    void operator^=(const TypedValue&);
    TypedValue& operator-();
};


template<class>
std::string type_debug_name() { return ""; }

template<>
inline std::string type_debug_name<Number>() { return "Number"; }

template<>
inline std::string type_debug_name<Text>() { return "Text"; }

template<>
inline std::string type_debug_name<Sequence>() { return "Sequence"; }

template<>
inline std::string type_debug_name<Procedure>() { return "Procedure"; }


using DirectiveFunctor        = std::function<void(const ArgumentList&, Program* const)>;
using AudioProcessingCallback = std::function<void(const MultichannelBuffer&, MultichannelBuffer&, std::any)>;
using SubgraphRepresentation  = std::pair<const std::string, const std::array<float, 2>>;

template <class T>
using SymbolTable = std::map<std::string, T>;
using Frame = std::vector<float>;



class Program
{
    static inline SymbolTable<DirectiveFunctor> custom_directives;

    uint inputs = 0;
    uint outputs = 0;
    SymbolTable<std::unique_ptr<AudioObject>> table;
    SymbolTable<TypedValue> symbol_table;
    MultichannelBuffer out;

public:
    static const SymbolTable<Procedure> procedures;

    SymbolTable<const size_t> group_sizes;
    SymbolTable<const SubgraphRepresentation> subgraphs;
    Program* parent = nullptr;

    template<typename>
    void create_object(const std::string&, const ArgumentList&);

    template<typename T>
    T* get_audio_object_raw_pointer(const std::string&) const;

    void check_io_and_connect_objects(const std::string&, const uint,
                                      const std::string&, const uint);


    void connect_objects(const std::string&, const uint, const std::string&, const uint, const ConnectionType = ConnectionType::one_to_one);

    static void add_directive(const std::string&, const DirectiveFunctor);
    void invoke_directive(const std::string&, const ArgumentList&);

    void create_user_object(const std::string&, const uint, const uint, std::any, AudioProcessingCallback);

    void configure_io(const uint, const uint);

    void simulate();
    MultichannelBuffer run();
    MultichannelBuffer run(const MultichannelBuffer);

    bool object_exists(const std::string&) const;
    void expect_to_be_object(const std::string&) const;
    void expect_to_be_group(const std::string&) const;

    void finish();
    void reset();

    const SubgraphRepresentation find_subgraph_recursively(std::string);

    auto begin() { return std::begin(table); }
    auto end() { return std::end(table); }

    template<class>
    bool symbol_is_type(const std::string&) const;

    template<class T>
    T get_symbol_value(const std::string&) const;

    TypedValue get_symbol_value(const std::string&) const;
    void add_symbol(const std::string&, const TypedValue&);
    void remove_symbol(const std::string&);
    bool symbol_exists(const std::string&) const;
};

using Graph = Program;


template<class T>
T* Program::get_audio_object_raw_pointer(const std::string& name) const
{
    static_assert(std::is_base_of<AudioObject, T>::value, "Type must be audio object");
    return static_cast<T*>(table.at(name).get());
}

template<class Object>
void Program::create_object(const std::string& name, const ArgumentList& arguments)
{
    if (table.count(name) != 0) error("Symbol '" + name + "' is already used");
    table[name] = std::make_unique<Object>(arguments);
}

template<class T>
bool Program::symbol_is_type(const std::string& identifier) const
{
    if (!symbol_exists(identifier))
        error("Symbol " + identifier + " does not exist, attempted to verify type");
    return symbol_table.at(identifier).is_type<T>();
}

template<class T>
T Program::get_symbol_value(const std::string& identifier) const
{
    if (!symbol_exists(identifier))
        error("Symbol " + identifier + " does not exist, attempted to read value");
    else if (!symbol_is_type<T>(identifier))
        error("Symbol " + identifier + " is wrong type");
    return symbol_table.at(identifier).get_value<T>();
}

template<class T>
T& TypedValue::get_value()
{
    if (!is_type<T>())
        error("Expected type " + type_debug_name<T>());

    return std::get<T>(*this);
}

template<class T>
const T& TypedValue::get_value() const
{
    if (!is_type<T>())
        error("Expected type " + type_debug_name<T>());

    return std::get<T>(*this);
}

template<class T>
bool TypedValue::is_type() const
{
    return std::holds_alternative<T>(*this);
}

/*
template <typename Callable>
void visit_typed_value(Callable function, TypedValue& value, const TypedValue& other)
{
    switch(value.get_type()) {
        case(Type::number):   value = function(&value.get_value<Number>(), other); break;
        case(Type::sequence): value = function(&value.get_value<Sequence>(), other); break;
        case(Type::text):     value = function(&value.get_value<Text>(), other); break;
    }
}
*/
}
