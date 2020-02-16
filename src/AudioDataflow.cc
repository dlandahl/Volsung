
#include "AudioDataflow.hh"

namespace Volsung {

/*
float& AudioBuffer::operator[](size_t n)
{
    return stream[n];
}

float AudioBuffer::operator[](size_t n) const
{
    return stream[n];
}

size_t AudioBuffer::size() const
{
    return stream.size();
}

float* const AudioBuffer::data_pointer()
{
    return stream.data();
}
*/


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





bool AudioInput::is_connected() const
{
    return bool(connections.size());
}

float AudioInput::read_value() const
{
    float value = 0;

    if (is_connected()) {
        for (const auto& connection: connections)
            value += connection->stored_value;
    }

    return value;
}

void AudioOutput::write_value(const float value)
{
    for (auto& connection: connections)
        connection->stored_value = value;
}

void AudioOutput::connect(AudioInput &other)
{
    connections.push_back(std::make_shared<AudioConnector>());
    other.connections.push_back(connections.back());
}

}
