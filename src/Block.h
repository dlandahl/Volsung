#pragma once

#include <array>
#include <vector>

#include "Yggdrasil.h"

class Block
{
	std::array<float, BLOCKSIZE> sdata = { 0 };
public:
	float& operator[](size_t n);
};

class BlockBuffer
{
	std::vector<Block> bdata;

public:
	Block              read_block();
	void               write_block(Block b);
};
