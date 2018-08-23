
#include <fstream>
#include <string>
#include <type_traits>
#include <map>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "AudioObject.h"

#include "./objects/OscillatorObject.h"
#include "./objects/FileoutObject.h"
#include "./objects/MultObject.h"
#include "./objects/AddObject.h"
#include "./objects/FilterObject.h"
#include "./objects/NoiseObject.h"

//Symbol table type
using st_type = std::map<str, AudioObject*>;

template<class obj>
void create_object(str name, st_type &symbols, str args = "")
{
	static_assert(std::is_base_of<AudioObject, obj>::value, "no");
	symbols[name] = new obj(args);
}

void connect_objects(AudioObject* a, uint out, AudioObject* b, uint in)
{
	a->outputs[out].connections.push_back({});
	b->inputs[in].connections.push_back({});

	a->outputs[out].connections.back() =
	b->inputs[in].connections.back() =
		new AudioConnector;
}

int main()
{
	st_type       symbol_table;
	st_type& st = symbol_table;

	/*             This will not be hardcoded in the final version             */
	/*  Will either be read from a file or created with a command line or GUI  */
	create_object<OscillatorObject>("mod", st, "3");
	create_object<AddObject>("add", st, "1");
	create_object<MultObject>("mult", st, "500");

	create_object<NoiseObject>("osc", st);
	create_object<FilterObject>("filter1", st);
	create_object<FilterObject>("filter2", st);
	create_object<FileoutObject>("output", st, "output.raw");

	connect_objects(st["mod"], 0, st["add"], 0);
	connect_objects(st["add"], 0, st["mult"], 0);

	connect_objects(st["mult"], 0, st["filter1"], 1);
	connect_objects(st["mult"], 0, st["filter2"], 1);

	connect_objects(st["osc"], 0, st["filter1"], 0);
	connect_objects(st["filter1"], 0, st["filter2"], 0);
	connect_objects(st["filter2"], 0, st["output"], 0);

	for (uint i = 0; i < 1000; i++)
	{
		for (auto const& s : st)
			s.second->run();
	}
		for (auto const& s : st)
			s.second->finish();
}
