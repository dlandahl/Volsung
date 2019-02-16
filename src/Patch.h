#pragma once

#include <memory>
#include <istream>

#include "StringFormat.h"
#include "Yggdrasil.h"

namespace Yggdrasil {

using st_type = std::unordered_map<std::string, std::unique_ptr<AudioObject>>;

class SymbolTable
{
	st_type table;

public:
	template<class>
	void create_object(std::string, std::string = "");

	void connect_objects(std::unique_ptr<AudioObject>&, uint,
	                     std::unique_ptr<AudioObject>&, uint);

	static void connect_objects(SymbolTable&, std::string, uint,
	                                          std::string, uint);

	void make_patch(std::istream&);
	void get_samples(float*, int, std::string, uint);

	auto begin() { return std::begin(table); }
	auto end() { return std::end(table); }
};

using Patch = SymbolTable;

}
