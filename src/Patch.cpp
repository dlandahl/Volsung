
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

template<class obj>
bool SymbolTable::create_object(std::string name, std::string args)
{
	if (table.count(name) == 0) {
		table[name] = std::make_unique<obj>(args);
		return true;
	}
	log("Symbol '" + name + "' is already used");
	return false;
}

void SymbolTable::create_user_object(std::string name, uint inputs, uint outputs, std::any user_data, callback_functor callback)
{
	bool success = create_object<UserObject>(name, "mk " + name + " " + name + " " + std::to_string(inputs) + " " + std::to_string(outputs));
	if (!success) return;

	UserObject* object = static_cast<UserObject*>(table[name].get());
	object->callback = callback;
	object->user_data = user_data;
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
		cmd = split_by(cmd, ';')[0];

        for (uint n = 1; n < cmd.size(); n++) {
            while (cmd[n] == ' ' && cmd[n-1] == ' ') cmd.erase(n, 1);
        }
        while (cmd.front() == ' ') cmd.erase(0, 1);
        while (cmd.back() == ' ')  cmd.pop_back();

		if (starts_with(cmd, "mk "))
		{
			auto mk_cmd = split_by(cmd, ' ');
			log("Making:\t\t" + cmd, 1); 

			if (mk_cmd[1] == "osc")
				{ create_object<OscillatorObject>(mk_cmd[2], cmd); }
			else
			if (mk_cmd[1] == "file")
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
			else
			if (mk_cmd[1] == "drive")
				{ create_object<DriveObject>     (mk_cmd[2], cmd); }
				
			else { log("Bad make command"); }

		}

		else if (starts_with(cmd, "ct "))
		{
			cmd        = split_by(cmd, ' ')[1];
			auto c_cmd = split_by(cmd, '>');

			uint in, out;

			out = c_cmd[0][c_cmd[0].size() - 1] - '0'; c_cmd[0].pop_back();
			in  = c_cmd[1][c_cmd[1].size() - 1] - '0'; c_cmd[1].pop_back();

			if (table.count(c_cmd[0]) != 1) log("Cannot connect objects, output symbol not found\n");
			if (table.count(c_cmd[1]) != 1) log("Cannot connect objects, input symbol not found\n");
			else connect_objects(table[c_cmd[0]], out, table[c_cmd[1]], in);
		}

		else if (starts_with(cmd, "type"))
		{
            auto macro_cmd = split_by(cmd, ' ');
            if      (macro_cmd.size() != 3) log("Invalid type header, should be 'type Name ['");
            else if (macro_cmd[2] != "[") log("Invalid type header, should end in an open bracket");
            else {
                std::stringstream body;
                while (std::getline(in_stream, cmd) && !starts_with(cmd, "]"))
                {
                    body << cmd << '\n';
                    lines_parsed++;
                }
                macro_table[macro_cmd[1]] = body.str();
            }
		}

		else if (starts_with(cmd, "&"))
		{
			auto directive_cmd = split_by(cmd, '&')[1];
			std::vector<std::string> args = split_by(directive_cmd, ' ');
			
			auto cmd = args[0];

			if (!custom_directives.count(cmd)) log("No such directive: " + cmd);
			else custom_directives[cmd](args, this);
		}

		else
		{
			cmd = remove_spaces(cmd);
			if (cmd == "done") { return; }
			if (cmd != "") log("Bad command");
		}
	}
}

void SymbolTable::run()
{
	for (auto const& entry : table)
		entry.second->implement();
}

void SymbolTable::finish()
{
	for (auto const& entry : table)
		entry.second->finish();
}

void SymbolTable::reset()
{
	table.clear();
}

void SymbolTable::add_directive(std::string name, directive_functor function)
{
	if (!custom_directives.count(name))
		custom_directives[name] = function;
}


}
