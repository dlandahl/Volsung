
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>
#include <map>

#include "Yggdrasil.h"
#include "Graph.h"
#include "StringFormat.h"
#include "Objects.h"

namespace Yggdrasil {



void Program::create_user_object(std::string name, uint inputs, uint outputs, std::any user_data, callback_functor callback)
{
	bool success = create_object<UserObject>(name, { std::to_string(inputs), std::to_string(outputs) });
	if (!success) return;

	UserObject* object = get_audio_object_raw_pointer<UserObject>(name);
	object->callback = callback;
	object->user_data = user_data;
}

void Program::connect_objects(
	std::unique_ptr<AudioObject> &a, uint out,
    std::unique_ptr<AudioObject> &b, uint in)
{
	if (a->outputs.size() > out && b->inputs.size() > in)
		a->outputs[out].connect(b->inputs[in]);
	else
		log("Index out of range for connection");
}

void Program::connect_objects(Program &st,
                                  std::string a, uint out,
	                              std::string b, uint in)
{
	if (!st.table.count(a)) log("Object " + a + " has not been declared");
	else if (!st.table.count(b)) log("Object " + b + " has not been declared");
	else st.connect_objects(st.table[a], out, st.table[b], in);
}

void Program::make_graph(std::istream &&in_stream)
{
	reset();
	std::string cmd = ";";
	
	while (getline(in_stream, cmd))
	{
		lines_parsed++;
		cmd = split_by(cmd, ';')[0];

        for (uint n = 1; n < cmd.size(); n++) {
            while (cmd[n] == cmd[n-1] == ' ') cmd.erase(n, 1);
        }
        while (cmd.front() == ' ') cmd.erase(0, 1);
        while (cmd.back() == ' ')  cmd.pop_back();

		if (starts_with(cmd, "mk "))
		{
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

void Program::run()
{
	for (auto const& entry : table) {
		entry.second->implement();
	}
}

float Program::run(float sample)
{
	if (inputs) {
		AudioInputObject* object = get_audio_object_raw_pointer<AudioInputObject>("input");
		object->data = { 0 };
		object->data[0] = sample;
	}
	run();
	float out = 0;
	
	if (outputs) {
		AudioOutputObject* object = get_audio_object_raw_pointer<AudioOutputObject>("output");
		out = object->data[0];
	}
	return out;
}

void Program::finish()
{
	for (auto const& entry : table)
		entry.second->finish();
}

void Program::reset()
{
	table.clear();
	
	if (inputs) {
		create_object<AudioInputObject>("input", { std::to_string(inputs) });
	}
	if (outputs) {
		create_object<AudioOutputObject>("output", { std::to_string(outputs) });
	}	
}

void Program::add_directive(std::string name, directive_functor function)
{
	if (!custom_directives.count(name))
		custom_directives[name] = function;
}

void Program::configure_io(uint i, uint o)
{
	inputs = i;
	outputs = o;
}


}
