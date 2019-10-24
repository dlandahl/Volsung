#pragma once

#include <memory>
#include <array>

namespace Yggdrasil {

struct AudioConnector
{
	float stored_value;
};

class CircularBuffer
{
	std::vector<float>* stream;
	std::size_t size = BLOCKSIZE;
	int wval;

public:
	CircularBuffer();
	~CircularBuffer();
	void resize_stream(int size);
	float& operator[](int n);
};

struct AudioInput
{
	std::vector<std::shared_ptr<AudioConnector>> connections;
	float read_value();

	bool is_connected();
};

struct AudioOutput
{
	std::vector<std::shared_ptr<AudioConnector>> connections;
	void write_value(float);

	void connect(AudioInput &other);
};

}
