
#include <iostream>
#include <algorithm>

#include "StringFormat.h"

namespace Yggdrasil {

bool starts_with(std::string a, std::string b)
{
	return a.rfind(b, 0) == 0;
}

std::string remove_spaces(std::string in)
{
	in.erase(std::remove (in.begin(), in.end(), ' '), in.end());
	return in;
}

//this is horrid
//i'm not smart enough to do something better
std::vector<std::string> split_by(std::string input, char delim)
{
	bool found_delim;
	std::vector<std::string> output;
	while (input.size())
	{
		found_delim = false;
		for (uint i = 0; i < input.size(); i++)
		{
			if (input[i] == delim)
			{
				found_delim = true;
				output.push_back(input.substr(0, i));
				input.erase(0, i + 1);
				break;
			}
		}
		if (!found_delim)
		{
			break;
		}
	}
	output.push_back(input);
	return output;
}

}
