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

class SymbolTable
{
	st_type table;

	uint lines_parsed = 0;
	uint debug_length = 100;

public:

	template<class>
	bool create_object(std::string, std::string = "");

	void connect_objects(std::unique_ptr<AudioObject>&, uint,
	                     std::unique_ptr<AudioObject>&, uint);

	static void connect_objects(SymbolTable&, std::string, uint, std::string, uint);

	void create_user_object(std::string, uint, uint, callback_functor);
											  
	void make_patch(std::istream&);

	void run();
	void finish();
	void reset();
	
	auto begin() { return std::begin(table); }
	auto end() { return std::end(table); }
};

using Patch = SymbolTable;

}

