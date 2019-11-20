
#include <iostream>
#include <fstream>
#include <array>

#include "Volsung.hh"

using namespace Volsung;

int main(int argc, char ** argv)
{
	Program prog;

	prog.configure_io(1, 1);

	debug_callback = [] (std::string message) { std::cout << message; };

	uint time = 10;
	Program::add_directive("length", [&time] (std::vector<std::string> arguments, Program*) {
		time = std::stof(arguments[0]);
	});

	std::string code =
R"(

chord: [4] saw~ 100*n

N: 5
Q: 4

carrier_bands: [N] bpf~ transform(n), Q
amps: [N] mult~

chord|0 x> 0|carrier_bands|0 => 0|amps


mod_bands: [N] bpf~ transform(n), Q
mod_followers: [N] env~ 20ms, 20ms

mod_bands{0}     => mod_followers{0}
mod_followers{0} => amps{1}

input{0} <> mod_bands{0}
amps{0}  >> output{0}

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);
	log("Finished parsing");

	for (uint n = 0; n < SAMPLE_RATE * 10; n++) prog.run(0.f);
	prog.finish();
}

