
#pragma once

#include <memory>
#include <vector>

namespace Volsung {

struct AudioConnector
{
    float stored_value;
};

class CircularBuffer
{
    std::vector<float> stream = { 0.f, 0.f };
    std::size_t pointer = 0;

public:
    float& operator[](long);
    float operator[](long) const;
    void resize_stream(const std::size_t);
    void increment_pointer();
};

struct AudioInput
{
    std::vector<std::shared_ptr<AudioConnector>> connections;

    bool is_connected() const;
    float read_value() const;
};

struct AudioOutput
{
    std::vector<std::shared_ptr<AudioConnector>> connections;

    void write_value(const float);
    void connect(AudioInput&);
};

}
