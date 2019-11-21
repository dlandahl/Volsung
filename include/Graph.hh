
#pragma once

#include <variant>
#include <memory>
#include <istream>
#include <type_traits>
#include <sstream>
#include <any>

#include "Volsung.hh"

namespace Volsung {

class Program;
using callback_functor = std::function<void(buf&, buf&, std::any)>;

enum class Type {
	number,
	sequence,
	string
};

enum class ConnectionType {
	one_to_one,
	one_to_many,
	many_to_one,
	many_to_many
};

struct Sequence
{
	std::vector<float> data;
	int size() { return data.size(); }
};

using TypedValueBase = std::variant<float, std::string, Sequence>;
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

/*! \brief An instance of a Volsung program
 *  
 *  This class stores a representation of a parsed Volsung program.
 *  It includes the symbol tables for constants and unit generators (audio~ objects).
 *  Instantiate one of these for each program you want to interpret and run.
 */

class Program
{
	uint lines_parsed = 0;
	static inline std::map<std::string, directive_functor> custom_directives;
    Program* parent = nullptr;

	uint inputs = 0;
	uint outputs = 0;
	std::unordered_map<std::string, std::unique_ptr<AudioObject>> table;
	std::unordered_map<std::string, TypedValue> symbol_table;

public:
	std::unordered_map<std::string, int> group_sizes;

	/*! \brief Used to create audio objects manually
	 *  
	 *  This template can be used to inject audio objects into the symbol table without
	 *  through the interpreter.
	 */

	template<class>
	bool create_object(std::string, std::vector<TypedValue>);

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

	float run(float);
	std::vector<float> run(std::vector<float>);
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
	bool symbol_exists(std::string);
};

using Graph = Program;


template<class T>
T* Program::get_audio_object_raw_pointer(std::string name)
{
	static_assert(std::is_base_of<AudioObject, T>::value, "Type must be audio object");
	return static_cast<T*>(table[name].get());
}

template<class Object>
bool Program::create_object(std::string name, std::vector<TypedValue> arguments)
{
	if (table.count(name) == 0) {
		table[name] = std::make_unique<Object>(arguments);
		return true;
	}
	log("Symbol '" + name + "' is already used");
	throw ParseException();
}

template<class T>
bool Program::symbol_is_type(std::string identifier)
{
	if (!symbol_exists(identifier)) {
		log("Symbol " + identifier + " does not exist, attempted to verify type");
		throw ParseException();
	}
	return symbol_table[identifier].is_type<T>();
}

template<class T>
T Program::get_symbol_value(std::string identifier)
{
	if (!symbol_exists(identifier))
		log("Symbol " + identifier + " does not exist, attempted to read value");
	else if (!symbol_is_type<T>(identifier))
		log("Symbol " + identifier + " is wrong type");
	else return symbol_table[identifier].get_value<T>();
	throw ParseException();
}

template<class T>
T& TypedValue::get_value()
{
	if (!is_type<T>()) {
		log("Expected type " + type_debug_name<T>());
		throw ParseException();
	}
	return std::get<T>(*this);
}

template<class T>
bool TypedValue::is_type()
{
	return std::holds_alternative<T>(*this);
}

}

