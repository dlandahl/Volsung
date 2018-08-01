
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
#include "LFOObject.h"
#include "FileoutObject.h"

template<class obj>
void create_object(std::string name, std::map<std::string, AudioObject*> &symbols)
{
	if(std::is_base_of<AudioObject, obj>::value)
		symbols[name] = new obj;
}

int main()
{
	std::map<std::string, AudioObject*> symbol_table;

	create_object<OscillatorObject>("osc", symbol_table);
	create_object<LFOObject>("lfo", symbol_table);
	create_object<FileoutObject>("out", symbol_table);
	
	AudioConnector connection;
	AudioConnector connection2;
	
	symbol_table["osc"]->outputs[0].connections.push_back({});
	symbol_table["lfo"]->outputs[0].connections.push_back({});
	symbol_table["out"]->inputs [0].connections.push_back({});
	symbol_table["out"]->inputs [1].connections.push_back({});
	
	symbol_table["osc"]->outputs[0].connections[0] = &connection;
	symbol_table["out"]->inputs [0].connections[0] = &connection;
	
	symbol_table["lfo"]->outputs[0].connections[0] = &connection2;
	symbol_table["out"]->inputs [1].connections[0] = &connection2;

	for (uint i = 0; i < 10000; i++)
	{
		symbol_table["osc"]->run();
		symbol_table["lfo"]->run();
		symbol_table["out"]->run();
	}

	std::vector<float> out_data;
	out_data = symbol_table["out"]->data;
	
	std::fstream file("audio.raw", std::fstream::out | std::fstream::binary);
	
	for (uint i = 0; i < out_data.size(); i++) {
		file.write((const char*)&out_data[i], sizeof(float));
	}
	
	file.close();
}