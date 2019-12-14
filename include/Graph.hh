
#pragma once

#include <variant>
#include <memory>
#include <istream>
#include <any>
#include <random>
#include <chrono>

#include "Volsung.hh"

namespace Volsung {

class Program;
using callback_functor = std::function<void(buf&, buf&, std::any)>;

enum class Type {
	number,
	sequence,
	text
};

enum class ConnectionType {
	one_to_one,
	one_to_many,
	many_to_one,
	many_to_many,
	cross
};

struct Sequence
{
	std::vector<float> data;
	std::size_t size() { return data.size(); }
	
	operator std::string() {
		std::string string = "{ ";
		string += std::to_string(data[0]);
		for (std::size_t n = 1; n < size(); n++) string += ", " + std::to_string(data[n]);
		string += " }\n";
		return string;
	}
};

using Number = float;
using Text = std::string;

using TypedValueBase = std::variant<Number, Text, Sequence>;
class TypedValue : private TypedValueBase
{
	using TypedValueBase::TypedValueBase;
public:
	template<class T>
	T& get_value();

	template<class>
	bool is_type();

	Type get_type();

	void operator+=(TypedValue);
	void operator-=(TypedValue);
	void operator*=(TypedValue);
	void operator/=(TypedValue);
	void operator^=(TypedValue);
	TypedValue operator-();

};

template<class>
std::string type_debug_name() { return ""; }

template<>
inline std::string type_debug_name<float>() { return "Number"; }

template<>
inline std::string type_debug_name<Sequence>() { return "Sequence"; }

template<>
inline std::string type_debug_name<std::string>() { return "Text"; }

using directive_functor = std::function<void(std::vector<TypedValue>, Program*)>;
using SubgraphRepresentation = std::pair<std::string, std::array<float, 2>>;

template <class T>
using SymbolTable = std::unordered_map<std::string, T>;
using Frame = std::vector<float>;

/*! \brief An instance of a Volsung program
 *  
 *  This class stores a representation of a parsed Volsung program.
 *  It includes the symbol tables for constants and unit generators (audio~ objects).
 *  Instantiate one of these for each program you want to interpret and run.
 */

class Program
{
	static inline SymbolTable<directive_functor> custom_directives;

	uint inputs = 0;
	uint outputs = 0;
	SymbolTable<std::unique_ptr<AudioObject>> table;
	SymbolTable<TypedValue> symbol_table;

	std::uniform_real_distribution<float> distribution;
	std::default_random_engine generator;

public:
	SymbolTable<std::size_t> group_sizes;
	SymbolTable<SubgraphRepresentation> subgraphs;
	Program* parent = nullptr;
	
	/*! \brief Used to create audio objects manually
	 *  
	 *  This template can be used to inject audio objects into the symbol table without
	 *  through the interpreter.
	 */

	template<class>
	void create_object(std::string, std::vector<TypedValue>);

	template<class T>
	T* get_audio_object_raw_pointer(std::string);

	void connect_objects(std::unique_ptr<AudioObject>&, uint,
	                     std::unique_ptr<AudioObject>&, uint);

	/*! \brief Used to connect audio objects manually
	 *  
	 *  This template can be used to connect two audio objects by name.
	 */

	void connect_objects(std::string, uint, std::string, uint, ConnectionType = ConnectionType::one_to_one);

	/*! \brief Create a custom user directive
	 *
	 *  Supply a functor to be invoked when parsing an ampersand followed by the name of this directive.
	 */

	static void add_directive(std::string, directive_functor);
	void invoke_directive(std::string, std::vector<TypedValue>);

	/*! \brief Create an ambient user object
	 *
	 *  Create an ambient user object which will persist even when the program is reset and reinterpreted.
	 *  Use this to create input / output objects specific to your frontend.
	 *  User objects will be added to the symbol table upon reset of the program.
	 */

	void create_user_object(std::string, uint, uint, std::any, callback_functor);

	/*! \brief Add inputs and outputs to a program 
	 *
	 *  This will inject "input" and "output" objects into the symbol table.
	 *  The parameters control how many channels each of these will have.
	 *  Read and write data from them through the "run" function.
	 */

	void configure_io(uint, uint);

	/*! \brief Run the program
	 *
	 *  Runs the program by running each audio object (unit generator) in the symbol table in turn.
	 */

	void run();

	/*! \brief Run the program with one input and one output
	 *
	 *  Runs the program by running each audio object (unit generator) in the symbol table in turn.
	 *  Expects a sample which will be written to the "input" object, and returns a float sample from the "output" object, created by configure_io.
	 */
	bool object_exists(std::string);
	void expect_to_be_object(std::string);
	void expect_to_be_group(std::string);
	
	float run(float);
	Frame run(Frame);
	void finish();
	void reset();
	
	auto begin() { return std::begin(table); }
	auto end() { return std::end(table); }

	template<class>
	bool symbol_is_type(std::string);
	
	template<class T>
	T get_symbol_value(std::string);
	
	TypedValue get_symbol_value(std::string);
	void add_symbol(std::string, TypedValue);
	void remove_symbol(std::string);
	bool symbol_exists(std::string);

	Program() :
		distribution(0.f, 1.f), generator(std::chrono::system_clock::now().time_since_epoch().count())
	{ }
};

using Graph = Program;


template<class T>
T* Program::get_audio_object_raw_pointer(std::string name)
{
	static_assert(std::is_base_of<AudioObject, T>::value, "Type must be audio object");
	return static_cast<T*>(table[name].get());
}

template<class Object>
void Program::create_object(std::string name, std::vector<TypedValue> arguments)
{
	if (table.count(name) != 0)	error("Symbol '" + name + "' is already used");
	table[name] = std::make_unique<Object>(arguments);
}

template<class T>
bool Program::symbol_is_type(std::string identifier)
{
	if (!symbol_exists(identifier))
		error("Symbol " + identifier + " does not exist, attempted to verify type");
	return symbol_table[identifier].is_type<T>();
}

template<class T>
T Program::get_symbol_value(std::string identifier)
{
	if (!symbol_exists(identifier))
		error("Symbol " + identifier + " does not exist, attempted to read value");
	else if (!symbol_is_type<T>(identifier))
		error("Symbol " + identifier + " is wrong type");
	return symbol_table[identifier].get_value<T>();
}

template<class T>
T& TypedValue::get_value()
{
	if (!is_type<T>())
		error("Expected type " + type_debug_name<T>());

	return std::get<T>(*this);
}

template<class T>
bool TypedValue::is_type()
{
	return std::holds_alternative<T>(*this);
}

}

