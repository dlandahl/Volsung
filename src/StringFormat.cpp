
#include <iostream>

#include "StringFormat.h"

bool sf::starts_with(str a, str b)
{
	return a.rfind(b, 0) == 0;
}

std::vector<str> sf::split_by(str input, char delim)
{
	bool found_delim;
	std::vector<str> output;
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
