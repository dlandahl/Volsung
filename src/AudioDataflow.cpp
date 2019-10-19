
#include "Yggdrasil.h"
#include "AudioDataflow.h"

namespace Yggdrasil {

float& Block::operator[](uint n)
{
	return sample_data[n];
}

Block BlockBuffer::read_block()
{
	Block b;

	if (block_data.size())
	{
		b = block_data[0];
		block_data.erase(block_data.begin());
	}
	return b;
}

void BlockBuffer::write_block(Block b)
{
	block_data.push_back(b);
}

CircularBuffer::CircularBuffer()
{
	stream = new std::vector<float>(BLOCKSIZE);
}

CircularBuffer::~CircularBuffer()
{
	delete stream;
}
	
float& CircularBuffer::operator[](int n)
{
	while (n < 0) n += size;
	while (n >= size) n -= size;
	return (*stream)[n];
}

void CircularBuffer::resize_stream(int new_size)
{
	size = new_size;
	stream->resize(new_size);
}

bool AudioInput::is_connected() 
{
	return bool(connections.size());
}

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
}

}
