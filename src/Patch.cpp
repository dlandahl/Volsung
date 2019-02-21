
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>

#include "Yggdrasil.h"
#include "Patch.h"
#include "StringFormat.h"
#include "Objects.h"

namespace Yggdrasil {

template<class obj>
void SymbolTable::create_object(std::string name, std::string args)
{
	if (table.count(name) == 0)
		table[name] = std::make_unique<obj>(args);
   	else
		std::cout << "Symbol '" << name << "' is already used\n";
}

void SymbolTable::connect_objects(
	std::unique_ptr<AudioObject> &a, uint out,
    std::unique_ptr<AudioObject> &b, uint in)
{
	if (a->outputs.size() > out && b->inputs.size() > in)
		a->outputs[out].connect(b->inputs[in]);
	else
		std::cout << "Index out of range for connection "
                  << a->name << ">" << b->name << "\n";
}

void SymbolTable::connect_objects(SymbolTable &st,
                                  std::string a, uint out,
	                              std::string b, uint in)
{
	st.connect_objects(st.table[a], out, st.table[b], in);
}

void SymbolTable::make_patch(std::istream &in_stream)
{
	std::string cmd = ";";

	while (getline(in_stream, cmd))
	{
		cmd = split_by(cmd, ';')[0];

		if (starts_with(cmd, "mk "))
		{
			auto mk_cmd = split_by(cmd, ' ');

			if (mk_cmd[1] == "osc")
				{ create_object<OscillatorObject>(mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "out")
				{ create_object<FileoutObject>   (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "add")
				{ create_object<AddObject>       (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "mult")
				{ create_object<MultObject>      (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "noise")
				{ create_object<NoiseObject>     (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "filter")
				{ create_object<FilterObject>    (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "delay")
				{ create_object<DelayObject>     (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "square")
				{ create_object<SquareObject>    (mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "crush")
				{ create_object<CrushObject>     (mk_cmd[2], cmd); }
			else {std::cout << "Bad make command\n"; }
		}

		else if (starts_with(cmd, "ct "))
		{
			cmd        = split_by(cmd, ' ')[1];
			auto c_cmd = split_by(cmd, '>');

			uint in, out;

			out = c_cmd[0][c_cmd[0].size() - 1] - '0'; c_cmd[0].pop_back();
			in  = c_cmd[1][c_cmd[1].size() - 1] - '0'; c_cmd[1].pop_back();

			if (table.count(c_cmd[0]) != 1 || table.count(c_cmd[1]) != 1)
				std::cout << "Cannot connect objects, symbol not found\n";
			else
				connect_objects(table[c_cmd[0]], out, table[c_cmd[1]], in);
		}
		
		else
		{
			cmd = remove_spaces(cmd);
			if (cmd != "" && cmd != "done")
			{
				std::cout << "Bad command\n";
			}
			else return;
		}
	}
}

void SymbolTable::get_samples(float* data, int size,
                              std::string object, uint output)
{
	int samples_generated = 0;
	std::vector<float> out_data(size);

	while(samples_generated < size)
	{
		for (auto const& s : *this)
			s.second->implement();

		samples_generated += BLOCKSIZE;
		auto ringbuffer = table[object]->get_out_buffer(output);
	}

}

}
