
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

scale: { 0, 2, 4, 7, 9 }
root: 440
freqs: (2^(1/12))^scale * root


; Trigger and smooth envelope
eg: eg~
vca: mult~
clock: clock~ sf / 4

clock
-> 0|eg
-> filter~ 40
-> 1|vca


; Generate sine signal and output
index: snh~
clock -> 1|index

noise~ -> index
-> *0.5 -> +0.5
-> *4.9
-> seq~ freqs
-> osc~
-> vca
-> file~ "Random.raw"


; Randomise decay time
snh: snh~
clock -> 1|snh

noise~ -> *0.5 -> +0.5
-> * sf/2 + sf/10 -> snh -> 1|eg

)";

	Parser parser;
	parser.source_code = code;
	parser.parse_program(prog);
	log("Finished parsing");

	for (uint n = 0; n < SAMPLE_RATE * 10; n++) prog.run(0.f);
	prog.finish();
}

