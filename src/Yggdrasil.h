#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <functional>
#include <map>

namespace Yggdrasil {

inline float          SAMPLE_RATE = 44100.0f;
inline unsigned const BLOCKSIZE   = 1;
#define BUFFSIZE    44100
#define PI          3.14159265f
#define TAU         6.28318530f

class AudioObject;
class CircularBuffer;

using uint = unsigned;
using buf  = std::vector<CircularBuffer>;

inline std::map<std::string, std::string> macro_table;

template <typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

inline int debug_level = 0;

inline std::function<void(std::string)> debug_callback = [] (std::string message) { std::cout << message; };;

inline void log(std::string message, int level=0)
{
	if (debug_level >= level) debug_callback(message + '\n');
}

}

namespace ygg = Yggdrasil;
