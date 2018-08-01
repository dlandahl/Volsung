#pragma once

#include <array>
#include <vector>

#include "Yggdrasil.h"

class Block
{
	std::array<float, BLOCKSIZE> sdata;
public:
	float& operator[](size_t n);
};

class BlockBuffer
{
	std::vector<Block> bdata;
	uint block_count;

public:
	Block read_block();
	void write_block(Block b);
	uint get_block_count();
	BlockBuffer();

};
