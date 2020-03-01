
#pragma once

#include <vector>
#include <string>
#include <map>

#include "Volsung.hh"
#include "AudioDataflow.hh"

namespace Volsung {

inline static constexpr float gate_threshold = .75f;

class GateListener
{
    // const uint input;
    float last_value = 0.f;

public:
    enum class GateState {
        open        = 1 << 0,
        closed      = 1 << 1,
        just_opened = 1 << 2,
        just_closed = 1 << 3,
    };

    // GateListener(const uint _input): input(_input) { }
    GateState read_gate_state(float next_value);
};

using GateState = GateListener::GateState;

bool operator& (GateState, GateState);
GateState operator| (GateState, GateState);


class TypedValue;
class AudioObject
{
private:
    MultichannelBuffer in, out;

    struct LinkedValue
    {
        float* const parameter;
        const float  default_value;
        const uint input;
    };
    std::vector<LinkedValue> linked_values;

protected:
    virtual void process(const MultichannelBuffer&, MultichannelBuffer&) = 0;
    void set_io(const uint, const uint);
    void init(const uint, const uint, std::vector<TypedValue>, std::vector<float*>);

    void link_value(float* const, const float, const uint);
    void add_gate_listener(bool* const, const size_t);
    void update_parameters(size_t);

    bool is_connected(const uint) const;

public:
    void implement();
        
    std::vector<AudioInput>  inputs;
    std::vector<AudioOutput> outputs;
    AudioObject() = default;

    virtual void finish();
    virtual ~AudioObject() = default;
};

}

