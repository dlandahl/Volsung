
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>
#include <map>

#include "objects.h"
#include "Yggdrasil.h"
#include "AudioDataflow.h"
#include "AudioObject.h"

template<class obj>
void create_object(std::string name, std::map<std::string, AudioObject*> &symbols)
{
	if(std::is_base_of<AudioObject, obj>::value)
		symbols[name] = new obj;
}

void connect_object(AudioObject &a, AudioObject &b)
{
	a.output.connect(b.input);
}

int main()
{
	std::map<std::string, AudioObject*> symbol_table;

	create_object<OscillatorObject>("osc", symbol_table);
	create_object<BitcrushObject>("bit", symbol_table);
	create_object<FilterObject>("lop", symbol_table);
	create_object<FileoutObject>("out", symbol_table);
	
	connect_object(*symbol_table["osc"], *symbol_table["bit"]);
	connect_object(*symbol_table["bit"], *symbol_table["lop"]);
	connect_object(*symbol_table["lop"], *symbol_table["out"]);

	for (uint i = 0; i < 1000; i++)
	{
		symbol_table["osc"]->run();
		symbol_table["bit"]->run();
		symbol_table["lop"]->run();
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