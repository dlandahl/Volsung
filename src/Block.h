#pragma once

#include <array>
#include <vector>

#include "Yggdrasil.h"

class Block
{
	std::array<float, BLOCKSIZE> sample_data = { 0 };
public:
	float& operator[](uint n);
};

class BlockBuffer
{
	std::vector<Block> block_data;

public:
	Block  read_block();
	void   write_block(Block b);
};
