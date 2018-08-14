
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
#include <map>

#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "AudioObject.h"

#include "OscillatorObject.h"
#include "FileoutObject.h"

#include "AddObject.h"

template<class obj>
void create_object(int name, std::map<int, AudioObject*> &symbols, str args="")
{
	static_assert(std::is_base_of<AudioObject, obj>::value, "no");
	symbols[name] = new obj(args);
}

void connect_objects(AudioObject* a, uint out, AudioObject* b, uint in)
{
	a->outputs[out].connections.push_back({});
	b->inputs [in] .connections.push_back({});
	
	a->outputs[out].connections.back() =
	b->inputs [in] .connections.back() =
		new AudioConnector;
}

int main()
{
	std::map<int, AudioObject*>       symbol_table;
	std::map<int, AudioObject*>& st = symbol_table;

	create_object<OscillatorObject>(1, st, "432");
	create_object<FileoutObject>(34563456, st, "lol.raw");

	connect_objects(st[1], 0, st[34563456], 0);

	for (auto const& x : st)
	{
		for (uint i = 0; i < 1000; i++)
		{
			x.second->run();
		}
		x.second->finish();
	}
}