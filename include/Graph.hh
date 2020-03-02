
#pragma once

#include <variant>
#include <memory>
#include <istream>
#include <any>
#include <random>
#include <chrono>
#include <array>

#include "Volsung.hh"
#include "AudioDataflow.hh"

namespace Volsung {

enum class Type {
    number,
    text,
    sequence
};

enum class ConnectionType {
    one_to_one,
    one_to_many,
    many_to_one,
    many_to_many,
    biclique
};

class TypedValue;
class Text;
class Sequence;
class AudioObject;

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

    Number(float);
    Number(float, float);
    Number() = default;

    TypedValue add(const TypedValue&);
    TypedValue subtract(const TypedValue&);
    TypedValue multiply(const TypedValue&);
    TypedValue divide(const TypedValue&);
    TypedValue exponentiate(const TypedValue&);

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

using TypedValueBase = std::variant<Number, Text, Sequence>;
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


class Program;

using ArgumentList = std::vector<TypedValue>;
using DirectiveFunctor = std::function<void(const ArgumentList&, Program* const)>;
using AudioProcessingCallback = std::function<void(const MultichannelBuffer&, MultichannelBuffer&, std::any)>;
using SubgraphRepresentation = std::pair<const std::string, const std::array<float, 2>>;

template <class T>
using SymbolTable = std::map<std::string, T>;
using Frame = std::vector<float>;


class Procedure
{
    using Implementation = std::function<TypedValue(const ArgumentList&, const Program*)>;
    Implementation implementation;

public:
    const size_t min_arguments;
    const size_t max_arguments;
    const bool can_be_mapped;
    TypedValue operator()(const ArgumentList&, const Program*) const;
    Procedure(Implementation, size_t, size_t, bool = false);
};


/*
class Allocator
{
    void* data;
    std::size_t blocksize_bytes;
    std::size_t num_blocks;
public:
    Allocator(Program* program) {
        
    }
};
*/

/*! \brief An instance of a Volsung program
 *  
 *  This class stores a representation of a parsed Volsung program.
 *  It includes the symbol tables for constants and unit generators (audio~ objects).
 *  Instantiate one of these for each program you want to interpret and run.
 */

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
    
    /*! \brief Used to create audio objects manually
     *  
     *  This template can be used to inject audio objects into the symbol table without
     *  using the interpreter.
     */

    template<typename>
    void create_object(const std::string&, const ArgumentList&);

    template<typename T>
    T* get_audio_object_raw_pointer(const std::string&) const;

    void check_io_and_connect_objects(const std::string&, const uint,
                                      const std::string&, const uint);

    /*! \brief Used to connect audio objects manually
     *  
     *  This template can be used to connect two audio objects by name.
     */

    void connect_objects(const std::string&, const uint, const std::string&, const uint, const ConnectionType = ConnectionType::one_to_one);

    /*! \brief Create a custom user directive
     *
     *  Supply a functor to be invoked when parsing an ampersand followed by the name of this directive.
     */

    static void add_directive(const std::string&, const DirectiveFunctor);
    void invoke_directive(const std::string&, const ArgumentList&);

    /*! \brief Create an ambient user object
     *
     *  Create an ambient user object which will persist even when the program is reset and reinterpreted.
     *  Use this to create input / output objects specific to your frontend.
     *  User objects will be added to the symbol table upon reset of the program.
     */

    void create_user_object(const std::string&, const uint, const uint, std::any, AudioProcessingCallback);

    /*! \brief Add inputs and outputs to a program 
     *
     *  This will inject "input" and "output" objects into the symbol table.
     *  The parameters control how many channels each of these will have.
     *  Read and write data from them through the "run" function.
     */

    void configure_io(const uint, const uint);

    /*! \brief Run the program
     *
     *  Runs the program by running each audio object in the audio processing graph in turn. Doesn't give you back data.
     */

    void simulate();

    /*! \brief Run the program with one input and one output
     *
     *  Runs the program by running each audio object in the audio processing graph in turn.
     *  Expects a sample which will be written to the "input" object, and returns a float sample from the "output" object, created by configure_io.
     */
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
