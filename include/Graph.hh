
#pragma once

#include <variant>
#include <memory>
#include <istream>
#include <type_traits>
#include <sstream>
#include <any>

#include "Volsung.hh"

namespace Volsung {

using st_type = std::unordered_map<std::string, std::unique_ptr<AudioObject>>;

class Program;
using directive_functor = std::function<void(std::vector<std::string>, Program*)>;
using callback_functor = std::function<void(buf&, buf&, std::any)>;

enum class Type {
	number,
	sequence,
	function,
	string
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
	T get_value();

	template<class>
	bool is_type();

	Type get_type();

	void operator+=(TypedValue);
	void operator-=(TypedValue);
	void operator*=(TypedValue);
	void operator/=(TypedValue);
};

class Program
{
	uint lines_parsed = 0;
	static inline std::map<std::string, directive_functor> custom_directives;
    Program* parent = nullptr;

	uint inputs = 0;
	uint outputs = 0;
	st_type table;
	std::map<std::string, TypedValue> symbol_table;

public:
	template<class>
	bool create_object(std::string, std::vector<TypedValue>);

	template<class T>
	T* get_audio_object_raw_pointer(std::string);

	void connect_objects(std::unique_ptr<AudioObject>&, uint,
	                     std::unique_ptr<AudioObject>&, uint);

	static void connect_objects(Program&, std::string, uint, std::string, uint);

	static void add_directive(std::string, directive_functor);
	void invoke_directive(std::string, std::vector<std::string>);
	
	void create_user_object(std::string, uint, uint, std::any, callback_functor);

	void make_graph(std::istream&&);
	void configure_io(uint, uint);
	
	void run();
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
	return false;
}

template<class T>
bool Program::symbol_is_type(std::string identifier)
{
	if (!symbol_exists(identifier)) {
		log("Symbol " + identifier + " does not exist, attempted to verify type");
		return false;
	}
	return symbol_table[identifier].is_type<T>();
}

template<class T>
T Program::get_symbol_value(std::string identifier)
{
	if (!symbol_exists(identifier))
		log("Symbol " + identifier + " does not exist, attempted to read value");
	if (!symbol_is_type<T>(identifier))
		log("Symbol " + identifier + " is wrong type");
	return symbol_table[identifier].get_value<T>();	
}

template<class T>
T TypedValue::get_value()
{
	return std::get<T>(*this);
}

template<class T>
bool TypedValue::is_type()
{
	return std::holds_alternative<T>(*this);
}

}

