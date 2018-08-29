
#include <string>
#include <math.h>

#include "SquareObject.h"

void SquareObject::run()
{
	Block b;

    sample_count = (SAMPLE_RATE / frequency) / 2;
    
    for (uint i = 0; i < BLOCKSIZE; i++) {
        if (sample_index++ > sample_count)
        {
            value *= -1;
            sample_index = 0;
        }
        b[i] = value;
    }
    
    write_block(b, 0);
}

SquareObject::SquareObject(std::string arg) : value(1), sample_index(0)
{
	try         { frequency = std::stoi(arg, nullptr, 10); }
	catch (...) { frequency = 1; }
	
	outputs.push_back({});
	inputs.push_back({});
}
