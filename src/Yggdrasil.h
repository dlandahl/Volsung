#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>

namespace Yggdrasil {

inline float          SAMPLE_RATE = 44100.0f;
inline unsigned const BLOCKSIZE   = 256;
#define BUFFSIZE    44100
#define PI          3.14159265f
#define TAU         6.28318530f

class AudioObject;
class CircularBuffer;

using uint = unsigned;
using buf  = std::vector<CircularBuffer>;

template <typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

inline int debug_level;

inline void log(std::string message, int level=0)
{
	if (debug_level >= level) std::cout << message << '\n';
}


}

namespace ygg = Yggdrasil;
