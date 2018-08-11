#pragma once

#include <array>
#include <vector>

#include "Yggdrasil.h"

//A block is an array of samples. These block are passed
//	around by audio objects for processing and synthesis.
class Block
{
	std::array<float, BLOCKSIZE> sdata = { 0 };
public:
	float& operator[](size_t n);
};

//A BlockBuffer is a vector of blocks. It is used to buffer and store
//	audio data.
class BlockBuffer
{
	std::vector<Block> bdata;

public:
	Block              read_block();
	void               write_block(Block b);
};
