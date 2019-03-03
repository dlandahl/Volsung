#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Yggdrasil {

	float   SAMPLE_RATE = 44100.0f;
#define BLOCKSIZE   200
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

}

namespace ygg = Yggdrasil;
