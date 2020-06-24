
#include "VolsungCore.hh"

namespace Volsung {


static float sample_rate = 44100.0f;
static std::string library_path = "";

void set_sample_rate(float new_fs)
{
    sample_rate = new_fs;
}

float get_sample_rate()
{
    return sample_rate;
}

void set_library_path(const std::string& path)
{
    library_path = path;
}

std::string get_library_path()
{
    return library_path;
}


static std::function<void(std::string)> debug_callback = [] (std::string message)
{
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
