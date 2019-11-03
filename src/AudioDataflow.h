#pragma once

#include <memory>
#include <array>

namespace Volsung {

struct AudioConnector
{
	float stored_value;
};

class CircularBuffer
{
	std::vector<float> stream = { 0.f, 0.f };
	uint pointer = 0;

public:
	void increment_pointer();
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
