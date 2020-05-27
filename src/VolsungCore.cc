
#include "VolsungCore.hh"

namespace Volsung {


float sample_rate = 44100.0f;

void set_sample_rate(float new_fs)
{
    sample_rate = new_fs;
}

float get_sample_rate()
{
    return sample_rate;
}


static std::function<void(std::string)> debug_callback = [] (std::string message) {
    std::cout << message;
};

void set_debug_callback(std::function<void(std::string)> new_callback)
{
    debug_callback = new_callback;
}

void log(const std::string& message)
{
    debug_callback(message);
}


}
