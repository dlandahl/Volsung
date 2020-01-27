
#pragma once

#include <vector>
#include <string>
#include <map>

#include "Volsung.hh"
#include "AudioDataflow.hh"

namespace Volsung {
class TypedValue;

struct linked_value
{
    float* parameter;
    float  default_value;
    uint input;

    linked_value(float* const p, const float dv, const uint in):
        parameter(p), default_value(dv), input(in)
    { }
};

class AudioObject
{
private:
    MultichannelBuffer in, out;
    std::vector<linked_value> linked_values;
//  std::size_t buffer_size = 1;

protected:
    virtual void process(const MultichannelBuffer&, MultichannelBuffer&) = 0;
    void request_buffer_size(const std::size_t);
    void set_io(const uint, const uint);
    void init(const uint, const uint, std::vector<TypedValue>, std::vector<float*>);
    void set_defval(float* const, const float, const int);

    bool is_gate_high(const uint) const;
    bool gate_opened(const uint) const;
    bool gate_closed(const uint) const;
    bool is_connected(const uint) const;

public:
    void implement();
    static constexpr float gate_threshold = .75f;
    
    std::vector<AudioInput>  inputs;
    std::vector<AudioOutput> outputs;
    AudioObject() = default;

    virtual void finish();
    virtual ~AudioObject() = default;
};

}
