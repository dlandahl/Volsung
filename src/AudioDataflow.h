#pragma once

#include <memory>
#include <array>

namespace Yggdrasil {

class Block
{
	std::array<float, BLOCKSIZE> sample_data = { 0 };
public:
	float& operator[](uint n);
};

struct BlockBuffer
{
	std::vector<Block> block_data;

	Block read_block();
	void  write_block(Block b);
};

struct AudioConnector
{
	BlockBuffer buffer;
};

class CircularBuffer
{
	std::array<float, BUFFSIZE> stream;
	int wval;

public:
	float& operator[](int n);
};

struct AudioInput
{
	std::vector<std::shared_ptr<AudioConnector>> connections;
	Block read_block();

	bool is_connected();
};

struct AudioOutput
{
	std::vector<std::shared_ptr<AudioConnector>> connections;
	void write_block(Block);

	void connect(AudioInput &other);
};

}