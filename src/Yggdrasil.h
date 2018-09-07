#pragma once

#include <string>
#include <vector>

#define BLOCKSIZE   200
#define SAMPLE_RATE 44100.0f
#define BUFFSIZE    256000
#define PI          3.14159265f
#define TAU         6.28318530f

class CircularBuffer;

using uint  = unsigned;
using str   = std::string;
using buf   = std::vector<CircularBuffer>;

template <typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}
