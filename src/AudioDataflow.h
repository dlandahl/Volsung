#pragma once

#include <memory>
#include <array>

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
	Block read_block();
	void  write_block(Block b);
};

class AudioConnector
{
public:
	BlockBuffer buffer;
};

class CircularBuffer
{
	std::array<float, BUFFSIZE> stream;
	int wval;

public:
	void  write_value(float);

	float& operator[](int n);
};

class AudioInput
{
public:
	std::vector<std::shared_ptr<AudioConnector>> connections;
	Block read_block();

	bool is_connected();
};

class AudioOutput
{
public:
	std::vector<std::shared_ptr<AudioConnector>> connections;
	void write_block(Block);

	void connect(AudioInput &other);
};
