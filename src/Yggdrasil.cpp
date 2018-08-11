
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

//This is a function that creates a new audio object, such as an oscillator or effect,
//	and places it in a std::map which maps strings to pointer-to-AudioObjects.
//It's a template function, and the subclass of AudioObject you want to create
//	 is passed as a template argument.
//It also allows you to pass a name for the object (eg "Osc1") and a string of arguments, for example for the frequency.
//	This string is passed to the constructor of the AudioObject, which may to decide how to interpret it.
template<class obj>
void create_object(str name, std::map<str, AudioObject*> &symbols, str args="")
{
	//This static_assert checks at compile time that the type that has been passed as
	//	the template argument is derived from the abstract AudioObject class.
	static_assert(std::is_base_of<AudioObject, obj>::value, "no");
	
	//This creates an anonymous instance of the type that has been passed into the
	//	template, and maps the name which has been passed to a pointer to that instance.
	symbols[name] = new obj(args);
	
	//This way, individual oscillators or effects that are created don't have a variable or
	//	symbol in the code, but a string name by which they can be accessed from the symbol table.
}

//This function takes in two pointers to AudioObjects and two integers which
//	 are the indeces of the input and the output of each of them you want to connect.
//Thereby it uses an instance of the AudioConnector class to connect the output of *a to the input of *b.
void connect_objects(AudioObject* a, uint out, AudioObject* b, uint in)
{
	//The connections that are connected to an output or input are std::vectors,
	//	 so that any number of connections can be linked to one input or output.
	
	//This code adds a value to the vector of connections on the
	//	right input and output on the two *AudioObjects.
	a->outputs[out].connections.push_back({});
	b->inputs [in] .connections.push_back({});
	
	//This links both the new positions in the vector to a new anonymous
	//	pointer to an AudioConnector, which allows them to communicate.
	a->outputs[out].connections.back() =
	b->inputs [in] .connections.back() =
		new AudioConnector;
}

int main()
{
	//This creates a std::map which links a string to an AudioOBject. This is the symbol table.
	//This allows us to create AudioObjects and look them up at runtime with a string, rather than them
	//	being named in the code with a variable.
	std::map<str, AudioObject*>       symbol_table;
	std::map<str, AudioObject*>& st = symbol_table;

	//The code below here is a hardcoded patch which creates a pitch modulated sine wave.
	//Later, objects will be created and linked using the command line or a graphical interface
	
	//Create an oscillator called "mod" and pass "1" as an argument. This argument is interpreted as
	//	the frequency by the constructor of the OscillatorObject. OscillatorObject is derived from AudioObject.
	//Thereby an OscillatorObject IS an AudioObject.
	create_object<OscillatorObject>("mod", st, "1");
	
	//Create another oscillator with a higher frequency and place it in the symbol table
	//	so it can be referred to later.
	create_object<OscillatorObject>("osc", st, "220");
	
	//Create a FileoutObject. Also derived from AudioObject.
	create_object<FileoutObject>   ("out", st);
	
	//This will connect the 0th output of the 220hz oscillator to the 0th input on the file output object.
	//That way we can hear the sine wave in the file that is generated.
	connect_objects(st["osc"], 0, st["out"], 0);
	
	//Link 0th output on the slow oscillator to the 0th input on the fast oscillator. This makes the slow
	//	oscillator control the pitch of the oscillator that is writing to the file.
	connect_objects(st["mod"], 0, st["osc"], 0);

	//Execute the run function on all objects in the symbol table st ten-thousand times. These
	//	run functions should contain the code that will make the object perform its function.
	for (uint i = 0; i < 10000; i++)
	{
		for (auto const& x : st)
		{
			x.second->run();
		}
	}

	//This code just writes the data from the FileoutObject to a .raw file and saves it.
	std::vector<float> out_data;
	out_data = symbol_table["out"]->data;
	
	std::fstream file("audio.raw", std::fstream::out | std::fstream::binary);
	
	for (uint i = 0; i < out_data.size(); i++) {
		file.write((const char*)&out_data[i], sizeof(float));
	}
	
	file.close();
}
