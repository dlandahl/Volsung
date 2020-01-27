
#pragma once

/*! \file */ 

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <functional>
#include <map>

namespace Volsung {

inline float sample_rate = 44100.0f;
#define PI          3.14159265f
#define TAU         6.28318530f

using uint = unsigned;

class CircularBuffer;
using MultichannelBuffer  = std::vector<CircularBuffer>;

template <typename T>
int sign(const T val)
{
    return (T(0) < val) - (val < T(0));
}


/*! \brief Callback for logging and debug messages
 *  
 *  Specify a function to handle error messages during parsing and running of the program.
 */

inline std::function<void(std::string)> debug_callback = [] (std::string message) { std::cout << message; };

inline void log(const std::string& message)
{
    debug_callback(message + '\n');
}


class VolsungException : public std::exception
{
public:
    virtual const char* what() const noexcept override;
};

#undef assert
inline void assert(const bool condition, const std::string& message)
{
    if (!condition) {
        log(message);
        throw VolsungException();
    }
}

inline void error(const std::string& message)
{
    assert(0, message);
}

}

namespace vlsng = Volsung;
