#pragma once

#include <memory>
#include <istream>
#include <typeindex>
#include <sstream>

#include "StringFormat.h"
#include "Yggdrasil.h"
#include "Objects.h"

namespace Yggdrasil {

using st_type = std::unordered_map<std::string, std::unique_ptr<AudioObject>>;

class Program;
using directive_functor = std::function<void(std::vector<std::string>, Program*)>;

class Program
{
	st_type table;

	uint lines_parsed = 0;
	static inline std::map<std::string, directive_functor> custom_directives;
    Program* parent = nullptr;
    
public:

	template<class>
	bool create_object(std::string, std::string = "");

	void connect_objects(std::unique_ptr<AudioObject>&, uint,
	                     std::unique_ptr<AudioObject>&, uint);

	static void connect_objects(Program&, std::string, uint, std::string, uint);

	static void add_directive(std::string, directive_functor);
	void create_user_object(std::string, uint, uint, std::any, callback_functor);
											  
	void make_graph(std::istream&);

	void run();
	void finish();
	void reset();
	
	auto begin() { return std::begin(table); }
	auto end() { return std::end(table); }
};

using Graph = Program;

}

