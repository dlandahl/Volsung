
#include "AudioDataflow.hh"
#include "Volsung.hh"

namespace Volsung {

float& AudioBuffer::operator[](size_t n)
{
    return (*data)[n];
}

float AudioBuffer::operator[](size_t n) const
{
    return (*data)[n];
}

float* AudioBuffer::data_pointer()
{
    return (float*) data.get();
}

AudioBuffer::AudioBuffer()
{
    data = std::make_shared<Block>();
    *data = { 0 };
}

const AudioBuffer AudioBuffer::zero;


float& CircularBuffer::operator[](long n)
{
    n += (long) pointer;
    while (n < 0) n += (long) stream.size();
    while (n >= (long) stream.size()) n -= (long) stream.size();
    return stream[n];
}

float CircularBuffer::operator[](long n) const
{
    n += (long) pointer;
    while (n < 0) n += (long) stream.size();
    while (n >= (long) stream.size()) n -= (long) stream.size();
    return stream.at(n);
}

void CircularBuffer::resize_stream(const size_t new_size)
{
    if (new_size >= 2) stream.resize(new_size);
}

void CircularBuffer::increment_pointer()
{
    pointer++;
    if (pointer >= stream.size()) pointer -= stream.size();
}

CircularBuffer::CircularBuffer(size_t size)
{
    resize_stream(size);
}

bool AudioInput::is_connected() const
{
    return bool(connections.size());
}

const AudioBuffer AudioInput::read_buffer() const
{
    switch (connections.size()) {
        case (0): return AudioBuffer::zero;
        case (1): return connections[0]->stored_buffer;
        default: {
            AudioBuffer ret;
            for (size_t n = 0; n < connections.size(); n++) {
                for (size_t s = 0; s < AudioBuffer::blocksize; s++) {
                    ret[s] += connections[n]->stored_buffer[s];
                }
            }
            return ret;
        }
    }

    return AudioBuffer::zero;
}

void AudioOutput::write_buffer(const AudioBuffer buffer)
{
    if (connections.size() == 0) return;
    connections[0]->stored_buffer = buffer;
    for (size_t n = 1; n < connections.size(); n++) {
        connections[n]->stored_buffer = buffer;
    }
}

void AudioOutput::connect(AudioInput &other)
{
    connections.push_back(std::make_shared<AudioConnector>());
    other.connections.push_back(connections.back());
}

}
