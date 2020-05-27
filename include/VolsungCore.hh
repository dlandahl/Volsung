
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

#define PI          3.14159265f
#define TAU         6.28318530f

using uint = unsigned;

void set_sample_rate(float);
float get_sample_rate();

void log(const std::string&);
void set_debug_callback(std::function<void(std::string)>);


template <typename T>
int sign(const T val)
{
    return (T(0) < val) - (val < T(0));
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
