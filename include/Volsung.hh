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

/*! \brief Callback for logging and debug messages
 *  
 *  Specify a function to handle error messages during parsing and running of the program.
 */

inline std::function<void(std::string)> debug_callback = [] (std::string message) { std::cout << message; };

inline void log(std::string message, int level=0)
{
	if (debug_level >= level) debug_callback(message + '\n');
}


class VolsungException : public std::exception
{
public:
	virtual const char* what() const noexcept override;
};

#undef assert
inline void assert(bool condition, std::string message)
{
	if (!condition) {
		log(message);
		throw VolsungException();
	}
}

inline void error(std::string message)
{
	assert(0, message);
}

}

#include "Graph.hh"
#include "Parser.hh"
#include "AudioObject.hh"
namespace vlsng = Volsung;
