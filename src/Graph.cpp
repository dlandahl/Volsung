
#include <iostream>
#include <memory>
#include <type_traits>
#include <string>
#include <map>

#include "Yggdrasil.h"
#include "Graph.h"
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
