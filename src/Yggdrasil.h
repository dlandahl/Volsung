#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#define BLOCKSIZE   200
#define SAMPLE_RATE 44100.0f
#define BUFFSIZE    256000
#define PI          3.14159265f
#define TAU         6.28318530f

class AudioObject;
class CircularBuffer;

using uint = unsigned;
using buf  = std::vector<CircularBuffer>;

using st_type = std::unordered_map<std::string, std::unique_ptr<AudioObject>>;

template <typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}
