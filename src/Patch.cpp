
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>
#include <map>

#include "Yggdrasil.h"
#include "Patch.h"
#include "StringFormat.h"
#include "Objects.h"

namespace Yggdrasil {

namespace {
int debug_level = 0;
uint lines_parsed = 0;
}

void log(std::string message, int level=0)
{
	if (debug_level >= level) std::cout << lines_parsed << ":\t" << message << '\n';
}

template<class obj>
void SymbolTable::create_object(std::string name, std::string args)
{
	log("Name: " + name, 2);
	log("Args: " + args, 2);	
	if (table.count(name) == 0)
		table[name] = std::make_unique<obj>(args);
   	else
		log("Symbol '" + name + "' is already used");
}

void SymbolTable::connect_objects(
	std::unique_ptr<AudioObject> &a, uint out,
    std::unique_ptr<AudioObject> &b, uint in)
{
	if (a->outputs.size() > out && b->inputs.size() > in)
		a->outputs[out].connect(b->inputs[in]);
	else
		log("Index out of range for connection "
			+ a->name + ">" + b->name);
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
		lines_parsed++;
		log("Reading line", 2);
		
		cmd = split_by(cmd, ';')[0];
		if (starts_with(cmd, "mk "))
		{
			auto mk_cmd = split_by(cmd, ' ');
			log("Making:\t\t" + cmd, 1); 
			
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
			else { log("Bad make command"); }
		}

		else if (starts_with(cmd, "ct "))
		{
			log("Connecting:\t" + cmd, 1);
			cmd        = split_by(cmd, ' ')[1];
			auto c_cmd = split_by(cmd, '>');

			uint in, out;

			out = c_cmd[0][c_cmd[0].size() - 1] - '0'; c_cmd[0].pop_back();
			in  = c_cmd[1][c_cmd[1].size() - 1] - '0'; c_cmd[1].pop_back();

			if (table.count(c_cmd[0]) != 1) log("Cannot connect objects, output symbol not found\n");
			if (table.count(c_cmd[1]) != 1) log("Cannot connect objects, input symbol not found\n");
			else connect_objects(table[c_cmd[0]], out, table[c_cmd[1]], in);
		}

		else if (starts_with(cmd, "&"))
		{
			cmd = split_by(cmd, '&')[1];
			if (starts_with(cmd, "debug"))
			{
				auto d_cmd = split_by(cmd, ' ');
				if (d_cmd.size() == 1) debug_level = !debug_level;
				else debug_level = std::stoi(d_cmd[1]);
			}
			else log("Bad directive");
		}
		
		else
		{
			cmd = remove_spaces(cmd);
			if (cmd != "" && cmd != "done")
			{
				log("Bad command");
			}
			if (cmd == "done") return;
		}
	}
}

}
