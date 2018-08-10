
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

template<class obj>
void create_object(str name, std::map<str, AudioObject*> &symbols, str args="")
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
	std::map<str, AudioObject*>       symbol_table;
	std::map<str, AudioObject*>& st = symbol_table;

	create_object<OscillatorObject>("mod", st, "1");
	create_object<OscillatorObject>("osc", st, "220");
	create_object<FileoutObject>   ("out", st);

	connect_objects(st["osc"], 0, st["out"], 0);
	connect_objects(st["mod"], 0, st["osc"], 0);

	for (uint i = 0; i < 10000; i++)
	{
		for (auto const& x : st)
		{
			x.second->run();
		}
	}

	std::vector<float> out_data;
	out_data = symbol_table["out"]->data;
	
	std::fstream file("audio.raw", std::fstream::out | std::fstream::binary);
	
	for (uint i = 0; i < out_data.size(); i++) {
		file.write((const char*)&out_data[i], sizeof(float));
	}
	
	file.close();
}