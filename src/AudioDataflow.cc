
#include "Volsung.hh"
#include "AudioDataflow.hh"

namespace Volsung {
	
float& CircularBuffer::operator[](int n)
{
	n += pointer;
	while (n < 0) n += stream.size();
	while ((long) n >= (long long int) stream.size()) n -= stream.size();
	return stream[n];
}

void CircularBuffer::resize_stream(int new_size)
{
	if (new_size >= 2) stream.resize(new_size);
}

void CircularBuffer::increment_pointer()
{
	pointer++;
	if (pointer >= stream.size()) pointer -= stream.size();
}

bool AudioInput::is_connected() 
{
	return bool(connections.size());
}

float AudioInput::read_value()
{
	float value = 0;

	if (is_connected())
		for (uint n = 0; n < connections.size(); n++)
			value += connections[n]->stored_value;

	return value;
}

void AudioOutput::write_value(float value)
{
	for (uint n = 0; n < connections.size(); n++)
		connections[n]->stored_value = value;
}

void AudioOutput::connect(AudioInput &other)
{
	connections.push_back({ });
	other.connections.push_back({ });

	connections.back() =
	other.connections.back() =
        std::make_shared<AudioConnector>();
}

}
