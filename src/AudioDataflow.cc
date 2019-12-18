
#include "Volsung.hh"
#include "AudioDataflow.hh"

namespace Volsung {

float& CircularBuffer::operator[](long n)
{
    n += pointer;
    while (n < 0) n += stream.size();
    while (n >= (long) stream.size()) n -= stream.size();
    return stream[n];
}

float CircularBuffer::operator[](long n) const
{
    n += pointer;
    while (n < 0) n += stream.size();
    while (n >= (long) stream.size()) n -= stream.size();
    return stream.at(n);
}

void CircularBuffer::resize_stream(const std::size_t new_size)
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
    connections.push_back({ });
    other.connections.push_back({ });

    connections.back() =
    other.connections.back() =
        std::make_shared<AudioConnector>();
}

}
