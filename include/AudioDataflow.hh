
#pragma once

#include <memory>
#include <vector>
#include <array>

namespace Volsung {

class AudioBuffer
{
public:
    const static inline size_t blocksize = 64;
    using Block = std::array<float, blocksize>;
    
    const static AudioBuffer zero;
 
    float& operator[](size_t);
    float operator[](size_t) const;
    float* data_pointer();
    AudioBuffer();

    auto begin() { return std::begin(*data); }
    auto end() { return std::end(*data); }

private:
    std::shared_ptr<Block> data = nullptr;
};
using MultichannelBuffer = std::vector<AudioBuffer>;
using Block = AudioBuffer::Block;

struct AudioConnector
{
    AudioBuffer stored_buffer;
};

class CircularBuffer
{
    std::vector<float> stream = { 0.f, 0.f };
    size_t pointer = 0;

public:
    float& operator[](long);
    float operator[](long) const;
    void resize_stream(const size_t);
    void increment_pointer();

    CircularBuffer() = default;
    CircularBuffer(const size_t);
};



struct AudioInput
{
    std::vector<std::shared_ptr<AudioConnector>> connections;

    bool is_connected() const;
    const AudioBuffer read_buffer() const;
};

struct AudioOutput
{
    std::vector<std::shared_ptr<AudioConnector>> connections;

    void write_buffer(const AudioBuffer);
    void connect(AudioInput&);
};

}
