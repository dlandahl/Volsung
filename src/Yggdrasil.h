#pragma once

#include <string>

#define BLOCKSIZE   64
#define SAMPLE_RATE 44100.0f
#define PI          3.14159265f
#define TAU         6.28318530f

float operator      ""_hz(long double);

typedef unsigned    uint;
typedef std::string str;