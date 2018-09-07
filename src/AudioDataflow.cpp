
#include "Block.h"
#include "AudioDataflow.h"

void CircularBuffer::write_value(float val)
{
	while (wval >= BUFFSIZE) wval -= BUFFSIZE;
	while (wval <  0)        wval += BUFFSIZE;
	stream[wval++] = val;
}

float& CircularBuffer::operator[](int n)
{
	if (n < 0) n += BUFFSIZE;
	n %= BUFFSIZE;
	return stream[n];
}

bool AudioInput::is_connected() 
{ 
	return bool(connections.size());
};

Block AudioInput::read_block()
{
	Block b;

	if (is_connected())
	{
		for (uint j = 0; j < connections.size(); j++)
			b = connections[j]->buffer.read_block();
	}

	return b;
}

void AudioOutput::write_block(Block b)
{
	for (uint i = 0; i < connections.size(); i++)
		connections[i]->buffer.write_block(b);
}

void AudioOutput::connect(AudioInput &other)
{
	connections.push_back({ });
	other.connections.push_back({ });

	connections.back() =
	other.connections.back() =
		std::make_shared<AudioConnector>();
};
